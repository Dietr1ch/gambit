//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Dependency resolution with boost graph library
///
///          unravels the un-unravelable
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Christoph Weniger
///          (c.weniger@uva.nl)
///  \date 2013 May, Jun, Jul, Sep
///  \date 2014 Feb, Mar, Apr
///
///  \author Pat Scott
///          (patrickcolinscott@gmail.com)
///  \date 2013 May, Jul, Aug, Nov
///        2014 Jan, Mar, Apr, Dec
///        2018 Sep, Nov
///        2022 Nov, Dec
///        2023 Jan
///
///  \author Ben Farmer
///          (benjamin.farmer@monash.edu)
///  \date 2013 Sep
///
///  \author Tomas Gonzalo
///          (gonzalo@physik.rwth-aachen.de)
///  \date 2017 June
///        2019 May
///        2020 June
///        2021 Sep
///
///  \author Patrick Stoecker
///          (stoecker@physik.rwth-aachen.de)
///  \date 2020 May
///
///  *********************************************

#include "gambit/Core/depresolver.hpp"
#include "gambit/Core/observable.hpp"
#include "gambit/Core/rule.hpp"
#include "gambit/Models/models.hpp"
#include "gambit/Utils/stream_overloads.hpp"
#include "gambit/Utils/util_functions.hpp"
#include "gambit/Utils/version.hpp"
#include "gambit/Utils/bibtex_functions.hpp"
#include "gambit/Utils/citation_keys.hpp"
#include "gambit/Logs/logger.hpp"
#include "gambit/Backends/backend_singleton.hpp"
#include "gambit/cmake/cmake_variables.hpp"

#include <sstream>
#include <fstream>
#include <iomanip>
#include <regex>

#include <boost/format.hpp>
#include <boost/algorithm/string/replace.hpp>
#ifdef HAVE_GRAPHVIZ
  #include <boost/graph/graphviz.hpp>
#endif

// Dependency types
#define NORMAL_DEPENDENCY 1
#define LOOP_MANAGER_DEPENDENCY 2

// Debug flag
//#define DEPRES_DEBUG

// Verbose flag (not debug per se, just basic dependency resolution information)
//#define VERBOSE_DEP_RES

namespace Gambit
{

  namespace DRes
  {
    using namespace LogTags;

    ///////////////////////
    // Auxiliary classes
    //////////////////////

    /// Default constructor for QueueEntry
    QueueEntry::QueueEntry()
    : toVertex(0),
      obslike(NULL)
    {}

    /// Alternative constructor for QueueEntry
    QueueEntry::QueueEntry(sspair a, DRes::VertexID b, int c, bool d)
    : quantity(a),
      toVertex(b),
      dependency_type(c),
      printme(d),
      obslike(NULL)        
    {}


    ///////////////////////
    // Auxiliary functions
    ///////////////////////

    //
    // Functions that act on a resolved dependency graph
    //

    /// Collect parent vertices recursively (excluding root vertex)
    void getParentVertices(const VertexID & vertex, const
        DRes::MasterGraphType & graph, std::set<VertexID> & myVertexList)
    {
      graph_traits<DRes::MasterGraphType>::in_edge_iterator it, iend;

      for (std::tie(it, iend) = in_edges(vertex, graph);
          it != iend; ++it)
      {
        if (std::find(myVertexList.begin(), myVertexList.end(), source(*it, graph)) == myVertexList.end() )
        {
          myVertexList.insert(source(*it, graph));
          getParentVertices(source(*it, graph), graph, myVertexList);
        }
      }
    }

    /// Sort given list of vertices (according to topological sort result)
    std::vector<VertexID> sortVertices(const std::set<VertexID> & set,
        const std::list<VertexID> & topoOrder)
    {
      std::vector<VertexID> result;
      for(const VertexID& v : topoOrder)
      {
        if (set.find(v) != set.end()) result.push_back(v);
      }
      return result;
    }

    /// Get sorted list of parent vertices
    std::vector<VertexID> getSortedParentVertices(const VertexID & vertex, const
        DRes::MasterGraphType & graph, const std::list<VertexID> & topoOrder)
    {
      std::set<VertexID> set;
      getParentVertices(vertex, graph, set);
      set.insert(vertex);
      return sortVertices(set, topoOrder);
    }

    //
    // Graphviz output
    //

    /// Graphviz output for edges/dependencies
    class edgeWriter
    {
      public:
        edgeWriter(const DRes::MasterGraphType*) {};
        void operator()(std::ostream&, const EdgeID&) const
        {
          //out << "[style=\"dotted\"]";
        }
    };

    /// Graphviz output for individual vertices/nodes/module functions
    class labelWriter
    {
      private:
        const DRes::MasterGraphType * myGraph;
      public:
        labelWriter(const DRes::MasterGraphType * masterGraph) : myGraph(masterGraph) {};
        void operator()(std::ostream& out, const VertexID& v) const
        {
          str type = Utils::fix_type((*myGraph)[v]->type());
          boost::replace_all(type, str("&"), str("&amp;"));
          boost::replace_all(type, str("<"), str("&lt;"));
          boost::replace_all(type, str(">"), str("&gt;"));
          out << "[fillcolor=\"#F0F0D0\", style=\"rounded,filled\", shape=box,";
          out << "label=< ";
          out << "<font point-size=\"20\" color=\"red\">" << (*myGraph)[v]->capability() << "</font><br/>";
          out <<  "Type: " << type << "<br/>";
          out <<  "Function: " << (*myGraph)[v]->name() << "<br/>";
          out <<  "Module: " << (*myGraph)[v]->origin();
          out << ">]";
        }
    };


    //
    // Misc
    //

    /// Return runtime estimate for a set of nodes
    double getTimeEstimate(const std::set<VertexID> & vertexList, const DRes::MasterGraphType &graph)
    {
      double result = 0;
      for (const VertexID& v : vertexList)
      {
        result += graph[v]->getRuntimeAverage();
      }
      return result;
    }



    ///////////////////////////////////////////////////
    // Public definitions of DependencyResolver class
    ///////////////////////////////////////////////////

    /// Constructor
    DependencyResolver::DependencyResolver(const gambit_core &core,
                                           const Models::ModelFunctorClaw &claw,
                                           const IniParser::IniFile &iniFile,
                                           const Utils::type_equivalency &equiv_classes,
                                                 Printers::BasePrinter &printer)
     : boundCore(&core),
       boundClaw(&claw),
       boundTEs(&equiv_classes),
       boundPrinter(&printer),
       boundIniFile(&iniFile),
       obslikes(boundIniFile->getObservables()),
       module_rules(boundIniFile->getModuleRules()),
       backend_rules(boundIniFile->getBackendRules()),
       index(get(vertex_index,masterGraph)),
       activeFunctorGraphFile(Utils::runtime_scratch()+"GAMBIT_active_functor_graph.gv")
    {
      addFunctors();
      logger() << LogTags::dependency_resolver << endl;
      logger() << "#######################################"   << endl;
      logger() << "#  List of Type Equivalency Classes   #"   << endl;
      logger() << "#######################################";
      for (const auto& equiv_class : boundTEs->equivalency_classes) logger() << endl << equiv_class;
      logger() << EOM;
    }


    //
    // Initialization stage
    //

    /// Main dependency resolution
    void DependencyResolver::doResolution()
    {
      // Queue of dependencies to be resolved
      std::queue<QueueEntry> resolutionQueue;

      // Set up list of target ObsLikes
      logger() << LogTags::dependency_resolver << endl;
      logger() << "#######################################"   << endl;
      logger() << "#        List of Target ObsLikes      #"   << endl;
      logger() << "#                                     #"   << endl;
      logger() << "# format: Capability (Type) [Purpose] #"   << endl;
      logger() << "#######################################";
      for (const Observable& obslike : obslikes)
      {
        // Format output
        logger() << LogTags::dependency_resolver << endl << obslike.capability << " (" << obslike.type << ") [" << obslike.purpose << "]";
        QueueEntry target;  
        target.quantity.first = obslike.capability;
        target.quantity.second = obslike.type;
        target.obslike = &obslike;
        target.printme = obslike.printme;
        resolutionQueue.push(target);
      }
      logger() << EOM;

      // Activate functors compatible with model we scan over (and deactivate the rest)
      makeFunctorsModelCompatible();

      // Generate dependency tree (the core of the dependency resolution)
      generateTree(resolutionQueue);

      // Find one execution order for activated vertices that is compatible
      // with dependency structure
      function_order = run_topological_sort();

      // Loop manager initialization: Notify them about their nested functions
      for (const std::pair<const VertexID, std::set<VertexID>>& keyvalpair : loopManagerMap)
      {
        // Generate topologically sorted list of vertex IDs that are nested
        // within loop manager keyvalpair ...
        std::vector<VertexID> vertexList = sortVertices(keyvalpair.second, function_order);
        // ... convert that list into functor pointers...
        std::vector<functor*> functorList;
        for (const VertexID& v : vertexList)
        {
          functorList.push_back(masterGraph[v]);
        }
        // ...and store it into loop manager functor
        masterGraph[keyvalpair.first]->setNestedList(functorList);
      }

      // Initialise the printer object with a list of functors that are set to print
      initialisePrinter();

      #ifdef HAVE_GRAPHVIZ
        // Generate graphviz plot if running in dry-run mode.
        if (boundCore->show_runorder)
        {
          std::ofstream outf(activeFunctorGraphFile);
          write_graphviz(outf, masterGraph, labelWriter(&masterGraph), edgeWriter(&masterGraph));
        }
      #endif

      // Pre-compute the individually ordered vertex lists for each of the ObsLike entries.
      std::vector<VertexID> order = getObsLikeOrder();
      for(const auto& v : order)
      {
        SortedParentVertices[v] = getSortedParentVertices(v, masterGraph, function_order);
      }

      // Print list of backends required
      if (boundCore->show_backends)
      {
        printRequiredBackends();
      }

      // Get BibTeX key entries for backends, modules, etc
      getCitationKeys();

      // Get the scanID
      set_scanID();

      // Done
    }

    /// List of masterGraph content
    void DependencyResolver::printFunctorList()
    {
      // Activate functors compatible with model we scan over (and deactivate the rest)
      makeFunctorsModelCompatible();

      graph_traits<DRes::MasterGraphType>::vertex_iterator vi, vi_end;
      const str formatString = "%-20s %-32s %-32s %-32s %-15s %-7i %-5i %-5i\n";
      logger() << LogTags::dependency_resolver << endl << "Vertices registered in masterGraph" << endl;
      logger() << "----------------------------------" << endl;
      logger() << boost::format(formatString)%
       "MODULE (VERSION)"% "FUNCTION"% "CAPABILITY"% "TYPE"% "PURPOSE"% "STATUS"% "#DEPs"% "#BE_REQs";
      for (std::tie(vi, vi_end) = vertices(masterGraph); vi != vi_end; ++vi)
      {
        logger() << boost::format(formatString)%
         ((*masterGraph[*vi]).origin() + " (" + (*masterGraph[*vi]).version() + ")") %
         (*masterGraph[*vi]).name()%
         (*masterGraph[*vi]).capability()%
         (*masterGraph[*vi]).type()%
         (*masterGraph[*vi]).purpose()%
         (*masterGraph[*vi]).status()%
         (*masterGraph[*vi]).dependencies().size()%
         (*masterGraph[*vi]).backendreqs().size();
      }
      logger() <<  "Registered Backend vertices" << endl;
      logger() <<  "---------------------------" << endl;
      logger() << printGenericFunctorList(boundCore->getBackendFunctors());
      logger() << EOM;
    }

    /// Pretty print function evaluation order
    void DependencyResolver::printFunctorEvalOrder(bool toterminal)
    {
      // Running this lets us check the order of execution. Also helps
      // to verify that we actually have pointers to all the required
      // functors.

      // Get order of evaluation
      std::set<VertexID> parents;
      std::set<VertexID> done; //set of vertices already accounted for
      std::vector<VertexID> order = getObsLikeOrder();

      str formatString  = "%-5s %-25s %-25s %-25s\n";
      // Might need to check if terminal supports unicode characters...
      str formatString0 = "%-7s %-23s %-25s %-25s %-25s %-6s\n";  // header
      str formatString1a= "%-9s %-21s %-25s %-25s %-25s %-6s\n";  // target functors
      str formatString1b= "%-4s \u2514\u2500\u2500> %-21s %-25s %-25s %-25s %-6s\n";  // target functors
      str formatString2a= "     \u250C\u2500 %-23s %-25s %-25s %-25s %-6s\n";  // parents
      str formatString2b= "     \u251C\u2500 %-23s %-25s %-25s %-25s %-6s\n";
      str formatString3a= "     \u250CX %-23s %-25s %-25s %-25s %-6s\n"; // "already done" parents
      str formatString3b= "     \u251CX %-23s %-25s %-25s %-25s %-6s\n";

      int i = 0;

      // Show the order in which the target functors will be attacked.
      std::ostringstream ss;
      ss << endl << "Initial target functor evaluation order" << endl;
      ss << "----------------------------------" << endl;
      ss << boost::format(formatString)% "#"% "FUNCTION"% "CAPABILITY"% "ORIGIN";

      for (const VertexID& v : order)
      {
        ss << boost::format(formatString)%
         i%
         masterGraph[v]->name()%
         masterGraph[v]->capability()%
         masterGraph[v]->origin();
        i++;
      }

      ss << endl;

      i = 0; // Reset counter
      // Do another loop to show the full initial sequence of functor evaluation
      // This doesn't figure out the sequence within each target functor group; I'm not 100% sure where that is determined. This does, however, show which groups get evaluated first, and which functors are already evaluated.
      ss << endl << "Full initial functor evaluation order" << endl;
      ss << "----------------------------------" << endl;
      ss << boost::format(formatString0)% "#"% "FUNCTION"% "CAPABILITY"% "TYPE"% "ORIGIN"% "PRINT?";

      for (const VertexID& v : order)
      {
        // loop through parents of each target functor
        parents.clear();
        getParentVertices(v, masterGraph, parents);
        parents.insert(v);
        bool first = true;
        for (const VertexID& v2 : parents)
        {
            str formatstr;
            bool dowrite = false;
            // Check if parent functor has been ticked off the list
            bool is_done = done.find(v2) != done.end();
            if( (not is_done) and (v != v2) )
            {
                formatstr = formatString2b;
                if (first) formatstr = formatString2a;
                dowrite = true;
            }
            else if( v != v2)
            {
                // Might be better to just do nothing here, i.e. set dowrite=false. For now just flagging functor as done with a special format string.
                formatstr = formatString3b;
                if (first) formatstr = formatString3a;
                dowrite = true;
            }

            if (dowrite)
            {
              ss << boost::format(formatstr)%
                masterGraph[v2]->name()%
                masterGraph[v2]->capability()%
                masterGraph[v2]->type()%
                masterGraph[v2]->origin()%
                masterGraph[v2]->requiresPrinting();
            }
            done.insert(v2); // tick parent functor off the list
            first = false;
        }

        // Now show target functor info
        str formatstr;
        if(parents.size()==1) { formatstr = formatString1a; }
        else { formatstr = formatString1b; }
        ss << boost::format(formatstr)%
         i%
         masterGraph[v]->name()%
         masterGraph[v]->capability()%
         masterGraph[v]->type()%
         masterGraph[v]->origin()%
         masterGraph[v]->requiresPrinting();
        i++;

        done.insert(v); // tick this target functor off the list

      }
      ss << "(\"X\" indicates that the functor is pre-evaluated before the marked position)" << endl << endl;

      if (toterminal)
      {
        // There is a command line flag to get this information, since it is very
        // handy to check before launching a full job. It can always be checked via
        // the logs, but this feature is more convenient.
        cout << ss.str();
        #ifdef HAVE_GRAPHVIZ
          cout << "To get postscript plot of active functors, please run: " << endl;
          cout << GAMBIT_DIR << "/Core/scripts/./graphviz.sh " << activeFunctorGraphFile << " no-loners" << endl;
        #else
          cout << "To get postscript plot of active functors, please install graphviz, rerun cmake and remake GAMBIT." << endl << endl;
        #endif
      }

      logger() << LogTags::dependency_resolver << ss.str() << EOM;
    }

    /// Print the list of required backends
    void DependencyResolver::printRequiredBackends()
    {
      // Lists the required backends, indicating where several backends
      // can fulfil the same requirement
      std::stringstream ss;

      ss << endl << "Required backends to run file " << boundIniFile->filename() << std::endl;
      ss << "At least one backend candidate per row is required" << std::endl;
      ss << "--------------------------------------------------" << std::endl << std::endl;

      for(auto reqs : backendsRequired)
      {
        for(auto backend : reqs)
        {
          ss << boost::format("%-25s")%("("+backend.first+", "+backend.second+")");
        }
        ss << std::endl;
      }
      ss << std::endl;

      // Print to terminal
      std::cout << ss.str();

      // Print to logs
      logger() << LogTags::dependency_resolver << ss.str() << EOM;
    }

    /// Print the BibTeX citation keys
    void DependencyResolver::printCitationKeys()
    {

      // If the list is empty do not print anything
      if(citationKeys.empty()) return;

      std::stringstream ss;

      // Location of the bibtex file
      str bibtex_file_location = boundIniFile->getValueOrDef<str>(GAMBIT_DIR "/config/bibtex_entries.bib", "dependency_resolution", "bibtex_file_location");

      ss << "The scan you are about to run uses backends. Please make sure to cite all of them in your work." << std::endl;

      // Create a list of entries in the bibtex file
      BibTeX bibtex_file(bibtex_file_location);
      std::vector<str> entries = bibtex_file.getBibTeXEntries();

      // Make sure that each key has an entry on the bibtex file
      for(const auto& key : citationKeys)
      {
        // Now find each key in the list of entries
        if(std::find(entries.begin(), entries.end(), key) == entries.end())
        {
          std::ostringstream errmsg;
          errmsg << "The reference with key " << key << " cannot be found in the bibtex file " << bibtex_file_location << endl;
          errmsg << "Please make sure that the bibtex file contains the relevant bibtex entries." << endl;
          dependency_resolver_error().raise(LOCAL_INFO,errmsg.str());
        }
      }

      // Drop a bibtex file with the citation entries
      str bibtex_output_file = boundIniFile->getValueOrDef<str>("GAMBIT.bib", "dependency_resolution", "bibtex_output_file");
      bibtex_file.dropBibTeXFile(citationKeys, bibtex_output_file);

      // Drop a sample TeX file citing all backens
      str tex_output_file = boundIniFile->getValueOrDef<str>("GAMBIT.tex", "dependency_resolution", "tex_output_file");
      bibtex_file.dropTeXFile(citationKeys, tex_output_file, bibtex_output_file);

      ss << "You can find the list of references to include in " << bibtex_output_file << ". And and example TeX file in " << tex_output_file << std::endl << std::endl;

      // Print to terminal
      std::cout << ss.str();

      // Print to logs
      logger() << LogTags::dependency_resolver << ss.str() << EOM;

    }

    //
    // Runtime
    //

    /// Returns list of ObsLike vertices in order of runtime
    std::vector<VertexID> DependencyResolver::getObsLikeOrder()
    {
      std::vector<VertexID> unsorted;
      std::vector<VertexID> sorted;
      std::set<VertexID> parents, colleagues, colleagues_min;
      // Copy unsorted vertexIDs --> unsorted
      for (const OutputVertex& ov : outputVertices)
      {
        unsorted.push_back(ov.vertex);
      }
      // Sort iteratively (unsorted --> sorted)
      while (unsorted.size() > 0)
      {
        double t2p_now;
        double t2p_min = -1;
        std::vector<VertexID>::iterator it_min;
        for (std::vector<VertexID>::iterator it = unsorted.begin(); it !=
            unsorted.end(); ++it)
        {
          parents.clear();
          getParentVertices(*it, masterGraph, parents);
          parents.insert(*it);
          // Remove vertices that were already calculated from the ist
          for (const auto& colleague : colleagues) parents.erase(colleague);
          t2p_now = (double) getTimeEstimate(parents, masterGraph);
          t2p_now /= masterGraph[*it]->getInvalidationRate();
          if (t2p_min < 0 or t2p_now < t2p_min)
          {
            t2p_min = t2p_now;
            it_min = it;
            colleagues_min = parents;
          }
        }
        // Extent list of calculated vertices
        colleagues.insert(colleagues_min.begin(), colleagues_min.end());
        double prop = masterGraph[*it_min]->getInvalidationRate();
        logger() << LogTags::dependency_resolver << "Estimated T [s]: " << t2p_min*prop << EOM;
        logger() << LogTags::dependency_resolver << "Estimated p: " << prop << EOM;
        sorted.push_back(*it_min);
        unsorted.erase(it_min);
      }
      return sorted;
    }

    /// Evaluates ObsLike vertex, and everything it depends on, and prints results
    void DependencyResolver::calcObsLike(VertexID vertex)
    {
      if (SortedParentVertices.find(vertex) == SortedParentVertices.end())
        core_error().raise(LOCAL_INFO, "Tried to calculate a function not in or not at top of dependency graph.");
      std::vector<VertexID> order = SortedParentVertices.at(vertex);

      for (const VertexID& v : order)
      {
        std::ostringstream ss;
        ss << "Calling " << masterGraph[v]->name() << " from " << masterGraph[v]->origin() << "...";
        logger() << LogTags::dependency_resolver << LogTags::info << LogTags::debug << ss.str() << EOM;
        masterGraph[v]->calculate();
        if (boundIniFile->getValueOrDef<bool>(
              false, "dependency_resolution", "log_runtime") )
        {
          double T = masterGraph[v]->getRuntimeAverage();
          logger() << LogTags::dependency_resolver << LogTags::info <<
            "Runtime, averaged over multiple calls [s]: " << T << EOM;
        }
        invalid_point_exception* e = masterGraph[v]->retrieve_invalid_point_exception();
        if (e != NULL) throw(*e);
      }
      // Reset the cout output precision, in case any backends have messed with it during the ObsLike evaluation.
      cout << std::setprecision(boundCore->get_outprec());
    }

    /// Prints the results of an ObsLike vertex
    void DependencyResolver::printObsLike(VertexID vertex, const int pointID)
    {
      // pointID is supplied by the scanner, and is used to tell the printer which model
      // point the results should be associated with.

      if (SortedParentVertices.find(vertex) == SortedParentVertices.end())
        core_error().raise(LOCAL_INFO, "Tried to calculate a function not in or not at top of dependency graph.");
      std::vector<VertexID> order = SortedParentVertices.at(vertex);

      for (const VertexID& v : order)
      {
        std::ostringstream ss;
        ss << "Printing " << masterGraph[v]->name() << " from " << masterGraph[v]->origin() << "...";
        logger() << LogTags::dependency_resolver << LogTags::info << LogTags::debug << ss.str() << EOM;

        if (not typeComp(masterGraph[v]->type(),  "void", *boundTEs))
        {
          // Note that this prints from thread index 0 only, i.e. results created by
          // threads other than the main one need to be accessed with
          //   masterGraph[*it]->print(boundPrinter,pointID,index);
          // where index is some integer s.t. 0 <= index <= number of hardware threads.
          // At the moment GAMBIT only prints results of thread 0, under the expectation
          // that nested module functions are all designed to gather their results into
          // thread 0.
          masterGraph[v]->print(boundPrinter,pointID);
        }
      }
    }

    /// Getter for print_timing flag (used by LikelihoodContainer)
    bool DependencyResolver::printTiming() { return print_timing; }

    /// Get the functor corresponding to a single VertexID
    functor* DependencyResolver::get_functor(VertexID id)
    {
      graph_traits<DRes::MasterGraphType>::vertex_iterator vi, vi_end;
      for (std::tie(vi, vi_end) = vertices(masterGraph); vi != vi_end; ++vi)
      {
        if (*vi == id) return masterGraph[id];
      }
      return NULL;
    }

    /// Ensure that the type of a given vertex is equivalent to at least one of a provided list, and return the match.
    str DependencyResolver::checkTypeMatch(VertexID vertex, const str& purpose, const std::vector<str>& types)
    {
      for (const auto& t : types)
      {
        if (typeComp(t, masterGraph[vertex]->type(), *boundTEs)) return t;
      }
      std::stringstream msg;
      msg << "All quantities with purpose \"" << purpose << "\" in your yaml file must have one " << endl
          << "of the following types: " << endl << "  " << types << endl
          << "You have tried to assign this purpose to " << masterGraph[vertex]->origin() << "::"
          << masterGraph[vertex]->name() << "," << endl << "which has capability: " << endl
          << "  " << masterGraph[vertex]->capability() << endl << "and result type: " << endl
          << "  [" << masterGraph[vertex]->type() << "]" << endl << "Please assign a different purpose to this entry.";
      core_error().raise(LOCAL_INFO, msg.str());
      return "If you make core errors non-fatal you deserve what you get.";
    }

    /// Return the purpose associated with a given functor.
    const str& DependencyResolver::getPurpose(VertexID v)
    {
      for (const OutputVertex& ov : outputVertices)
      {
        if (ov.vertex == v) return ov.purpose;
      }
      /// '__no_purpose' if the functor does not correspond to an ObsLike entry in the ini file.
      static const str none("__no_purpose");
      return none;
    }

    /// Tell functor that it invalidated the current point in model space (due to a large or NaN contribution to lnL)
    void DependencyResolver::invalidatePointAt(VertexID vertex, bool isnan)
    {
      if (isnan)
      {
        masterGraph[vertex]->notifyOfInvalidation("NaN returned for likelihood value.");
      }
      else
      {
        masterGraph[vertex]->notifyOfInvalidation("Cumulative log-likelihood pushed below threshold.");
      }
    }

    /// Reset all active functors and delete existing results.
    void DependencyResolver::resetAll()
    {
      graph_traits<DRes::MasterGraphType>::vertex_iterator vi, vi_end;
      for (std::tie(vi, vi_end) = vertices(masterGraph); vi != vi_end; ++vi)
      {
        if (masterGraph[*vi]->status() == 2) masterGraph[*vi]->reset();
      }
    }


    ////////////////////////////////////////////////////
    // Private definitions of DependencyResolver class
    ////////////////////////////////////////////////////

    str DependencyResolver::printQuantityToBeResolved(const QueueEntry& entry)
    {
        str s = entry.quantity.first + " (" + entry.quantity.second + ")";
        s += ", required by ";
        if ( entry.obslike == NULL )
        {
            s += masterGraph[entry.toVertex]->capability() + " (";
            s += masterGraph[entry.toVertex]->type() + ") [";
            s += masterGraph[entry.toVertex]->name() + ", ";
            s += masterGraph[entry.toVertex]->origin() + "]";
        }
        else
            s += "ObsLike section of yaml file.";
        return s;
    }

    /// Generic printer of the contents of a vector of functors as vertices
    str DependencyResolver::printGenericFunctorList(const std::vector<VertexID> & vertexIDs)
    {
        std::vector<functor*> functorList;
        for (const auto& vid : vertexIDs)
        {
          functorList.push_back(masterGraph[vid]);
        }
        return printGenericFunctorList(functorList);
    }

    /// Generic printer of the contents of a vector of functors
    str DependencyResolver::printGenericFunctorList(const std::vector<functor*>& functorList)
    {
      const str formatString = "%-20s %-32s %-48s %-32s %-7i\n";
      std::ostringstream stream;
      stream << boost::format(formatString)%"ORIGIN (VERSION)"% "FUNCTION"% "CAPABILITY"% "TYPE"% "STATUS";
      for (const functor* f : functorList)
      {
        stream << boost::format(formatString)%
         (f->origin() + " (" + f->version() + ")") %
         f->name()%
         f->capability()%
         f->type()%
         f->status();
      }
      return stream.str();
    }

    /// Add module and primary model functors in bound core to class-internal
    /// masterGraph object
    void DependencyResolver::addFunctors()
    {
      // Add primary model functors to masterGraph
      for (const auto& f : boundCore->getPrimaryModelFunctors()) 
      {
        // Ignore functors with status set to 0 or less in order to ignore primary_model_functors
        // that are not to be used for the scan.
        if ( f->status() > 0 )
        {
          boost::add_vertex(f, this->masterGraph);
        }
      }
      // Add module functors to masterGraph
      for (const auto& f : boundCore->getModuleFunctors())
      {
          boost::add_vertex(f, this->masterGraph);
      }
    }

    /// Activate functors that are allowed to be used with one or more of the models being scanned.
    /// Also activate the model-conditional dependencies and backend requirements of those functors.
    void DependencyResolver::makeFunctorsModelCompatible()
    {
      // Run just once
      static bool already_run = false;
      if (already_run) return;

      graph_traits<DRes::MasterGraphType>::vertex_iterator vi, vi_end;
      std::set<str> modelList = boundClaw->get_activemodels();

      // Activate those module functors that match the combination of models being scanned.
      for (std::tie(vi, vi_end) = vertices(masterGraph); vi != vi_end; ++vi)
      {
        if (masterGraph[*vi]->status() >= 0 and masterGraph[*vi]->modelComboAllowed(modelList))
        {
          for (const str& model : modelList)
          {
            masterGraph[*vi]->notifyOfModel(model);
            masterGraph[*vi]->setStatus(1);
          }
        }
      }

      // Activate those backend functors that match one of the models being scanned.
      for (const str& model : modelList)
      {
        for (functor* f : boundCore->getBackendFunctors())
        {
          // Activate if the backend vertex permits the model and has not been (severely) disabled by the backend system
          if ( f->status() >= 0 and f->modelAllowed(model) )
          {
            f->setStatus(1);
          }
        }
      }
      already_run = true;
    }

    /// Set up printer object
    /// (i.e. give it the list of functors that need printing)
    void DependencyResolver::initialisePrinter()
    {
      // Send the state of the "print_unitcube" flag to the printer
      boundPrinter->set_printUnitcube(print_unitcube);

      std::vector<int> functors_to_print;
      graph_traits<MasterGraphType>::vertex_iterator vi, vi_end;
      //IndexMap index = get(vertex_index, masterGraph); // Now done in the constructor
      //Err does that make sense? There is nothing in masterGraph at that point surely... maybe put this back.
      //Ok well it does seem to work in the constructor, not sure why though...

      for (std::tie(vi, vi_end) = vertices(masterGraph); vi != vi_end; ++vi)
      {
        // Inform the active functors of the vertex ID that the masterGraph has assigned to them
        // (so that later on they can pass this to the printer object to identify themselves)
        //masterGraph[*vi]->setVertexID(index[*vi]); // Ugh cannot do this, needs to be consistent with get_param_id
        std::string label = masterGraph[*vi]->label();
        masterGraph[*vi]->setVertexID(Printers::get_param_id(label));
        // Same for timing output ID, but get ID number from printer system
        std::string timing_label = masterGraph[*vi]->timingLabel();
        masterGraph[*vi]->setTimingVertexID(Printers::get_param_id(timing_label));

        // Check for non-void type and status==2 (after the dependency resolution) to print only active, printable functors.
        // TODO: this doesn't currently check for non-void type; that is done at the time of printing in calcObsLike.
        if( masterGraph[*vi]->requiresPrinting() and (masterGraph[*vi]->status()==2) )
        {
          functors_to_print.push_back(index[*vi]); // TODO: Probably obsolete
          boundPrinter->addToPrintList(label); // Needed mainly by postprocessor.
          // Trigger a dummy print call for all printable functors. This is used by some printers
          // to set up buffers for each of these output streams.
          //logger() << LogTags::dependency_resolver << "Triggering dummy print for functor '"<<masterGraph[*vi]->capability()<<"' ("<<masterGraph[*vi]->type()<<")..." << EOM;

          //masterGraph[*vi]->print(boundPrinter,-1);
        }
      }

      // Force-reset the printer to erase the dummy calls
      // (but don't do this if we are in resume mode!)
      //if(not boundCore->resume) boundPrinter->reset(true);
      //boundPrinter->reset(true); // Actually *do* do it in resume mode as well. Printers should only reset new data, not destroy old data.

      // sent vector of ID's of functors to be printed to printer.
      // (if we want to only print functor output sometimes, and dynamically
      // switch this on and off, we'll have to rethink the strategy here a
      // little... for now if the print function of a functor does not get
      // called, it is up to the printer how it deals with the missing result.
      // Similarly for extra results, i.e. from any functors not in this
      // initial list, whose "requiresPrinting" flag later gets set to 'true'
      // somehow.)
      boundPrinter->initialise(functors_to_print); // TODO: Probably obsolete
    }

    std::vector<DRes::VertexID> DependencyResolver::closestCandidateForModel(std::vector<DRes::VertexID> candidates)
    {
      // In case of doubt (and if not explicitely disabled in the ini-file), prefer functors
      // that are more specifically tailored for the model being scanned. Do not consider functors
      // that are accessible via INTERPRET_AS_X links, as these are all considered to be equally 'far'
      // from the model being scanned, with the 'distance' being one step further than the most distant
      // ancestor.

      // Work up the model ancestry one step at a time, and stop as soon as one or more valid model-specific functors is
      // found at a given level in the hierarchy.
      std::vector<DRes::VertexID> newCandidates;
      std::set<str> s = boundClaw->get_activemodels();
      std::vector<str> parentModelList(s.begin(), s.end());
      while (newCandidates.size() == 0 and not parentModelList.empty())
      {
        for (str& model : parentModelList)
        {
          // Test each vertex candidate to see if it has been explicitly set up to work with the model
          for (const DRes::VertexID& candidate : candidates)
          {
            if (masterGraph[candidate]->modelExplicitlyAllowed(model)) newCandidates.push_back(candidate);
          }
          // Step up a level in the model hierarchy for this model.
          model = boundClaw->get_parent(model);
        }
        parentModelList.erase(std::remove(parentModelList.begin(), parentModelList.end(), "none"), parentModelList.end());
      }
      if (newCandidates.size() != 0)
        return newCandidates;
      else
        return candidates;
    }

    /// Collect ini options
    Options DependencyResolver::collectIniOptions(const DRes::VertexID & vertex)
    {
      YAML::Node nodes;
      YAML::Node zlevels;

      #ifdef DEPRES_DEBUG
        cout << "Searching options for " << masterGraph[vertex]->capability() << endl;
      #endif

      // TODO Iterate over all the rules that this functor has matched, and copy their options over 
           
      

      const IniParser::ObservablesType & entries = boundIniFile->getRules();
      for (IniParser::ObservablesType::const_iterator it =
          entries.begin(); it != entries.end(); ++it)
      {
        if (moduleFuncMatchesIniEntry(masterGraph[vertex], *it, *boundTEs))
        {
          #ifdef DEPRES_DEBUG
            cout << "Getting option from: " << it->capability << " " << it->type << endl;
          #endif
          for (auto jt = it->options.begin(); jt != it->options.end(); ++jt)
          {
            if ( not nodes[jt->first.as<std::string>()] )
            {
              #ifdef DEPRES_DEBUG
                cout << jt->first.as<std::string>() << ": " << jt->second << endl;
              #endif
              nodes[jt->first.as<std::string>()] = jt->second;
              zlevels[jt->first.as<std::string>()] = getEntryLevelForOptions(*it);
            }
            else
            {
              if ( zlevels[jt->first.as<std::string>()].as<int>() < getEntryLevelForOptions(*it) )
              {
                #ifdef DEPRES_DEBUG
                  cout << "Replaced : " << jt->first.as<std::string>() << ": " << jt->second << endl;
                #endif
                zlevels[jt->first.as<std::string>()] = getEntryLevelForOptions(*it);
                nodes[jt->first.as<std::string>()] = jt->second;
              }
              else if ( zlevels[jt->first.as<std::string>()].as<int>() == getEntryLevelForOptions(*it) )
              {
                std::ostringstream errmsg;
                errmsg << "ERROR! Multiple option entries with same level for key: " << jt->first.as<std::string>();
                dependency_resolver_error().raise(LOCAL_INFO,errmsg.str());
              }
            }
          }
        }
      }
      return Options(nodes);
    }

    /// Collect sub-capabilities
    Options DependencyResolver::collectSubCaps(const DRes::VertexID & vertex)
    {
      #ifdef DEPRES_DEBUG
        cout << "Searching for subcaps of " << masterGraph[vertex]->capability() << endl;
      #endif

      YAML::Node nodes;
      
      // Iterate over the ObsLikes entries
      for (auto it = observables.begin(); it != observables.end(); ++it)
      {
        // Select only those entries that match the current graph vertex (i.e. module function)
        if (moduleFuncMatchesIniEntry(masterGraph[vertex], *it, *boundTEs) and not it->subcaps.IsNull())
        {
          #ifdef DEPRES_DEBUG
            cout << "Found subcaps for " << it->capability << " " << it->type << " " << it->module << ":" << endl;
          #endif
          // The user has given just a single entry as a subcap
          if (it->subcaps.IsScalar())
          {
            str key = it->subcaps.as<str>();
            if (nodes[key]) dependency_resolver_error().raise(LOCAL_INFO,"Duplicate sub-capability for " + key + ".");
            nodes[key] = YAML::Node();
          }
          // The user has passed a simple list of subcaps
          else if (it->subcaps.IsSequence())
          {
            for (auto jt = it->subcaps.begin(); jt != it->subcaps.end(); ++jt)
            {
              if (not jt->IsScalar())
               dependency_resolver_error().raise(LOCAL_INFO,"Attempt to pass map using sequence syntax for subcaps of "+it->capability+".");
              str key = jt->as<str>();
              if (nodes[key]) dependency_resolver_error().raise(LOCAL_INFO,"Duplicate sub-capability for " + key + ".");
              nodes[key] = YAML::Node();
            }
          }
          // The user has passed some more complicated subcap structure than just a list of strings
          else if (it->subcaps.IsMap())
          {
            for (auto jt = it->subcaps.begin(); jt != it->subcaps.end(); ++jt)
            {
              str key = jt->first.as<str>();
              if (nodes[key]) dependency_resolver_error().raise(LOCAL_INFO,"Duplicate sub-capability for " + key + ".");
              nodes[key] = jt->second.as<YAML::Node>();
            }
          }
          #ifdef DEPRES_DEBUG
            cout << nodes << endl;
          #endif
        }
      }
      return Options(nodes);
    }

    /// Helper function to update vertex candidate lists in resolveDependencyFromRules
    void DependencyResolver::updateCandidates(const DRes::VertexID& v, std::vector<DRes::VertexID>& allowed, std::vector<DRes::VertexID>& disabled)
    {
      // Add the vertex to the active list of vertex candidates if a) vertex is not disabled in any way;
      if (masterGraph[v]->status() > 0 or
       // b) we only want the list of backends, and the vertex comes from an ini function;           
       (masterGraph[v]->status() == -4 and boundCore->show_backends) or
       // c) we only want the list of backends, and the vertex comes from a funtion that relies on classes from a disabled backend.
       (masterGraph[v]->status() == -3 and boundCore->show_backends))
      {
        //#pragma omp critical (allowedVertexCandidates)
        allowed.push_back(v);
      }
      // Otherwise, the vertex would have been fine except that it is disabled, so save it for printing in diagnostic messages.
      else
      {
        //#pragma omp critical (disabledVertexCandidates)
        disabled.push_back(v);
      }
    }

    /// Resolve dependency by matching capability, type pair of input queue entry, ensuring consistency with all obslike entries and subjugate rules.
    /// As non-subjugate rules have global applicability, all (strong) instances are assumed to have already been applied before this function is called. 
    DRes::VertexID DependencyResolver::resolveDependencyFromRules(const QueueEntry& entry, const std::vector<DRes::VertexID>& vertexCandidates)
    {
      // Candidate vertices after applying rules
      std::vector<DRes::VertexID> allowedVertexCandidates;
      std::vector<DRes::VertexID> disabledVertexCandidates;
 
      // If the dependency to be resolved comes from the ObsLike section, apply the conditions found in its ObsLike entry. 
      if (entry.obslike != NULL)
      {
        // Iterate over all candidates
        //# pragma omp parallel for 
        for (const DRes::VertexID& v : vertexCandidates)
        {
          // Require match to entry.quantity, and forbid self-resolution 
          if (v != entry.toVertex and entry.obslike->matches(masterGraph[v], *boundTEs)) 
           updateCandidates(v, allowedVertexCandidates, disabledVertexCandidates);
        }       
      }
      else
      {
        // If this dependency does not come from an ObsLike entry, make a temporary rule to filter 
        // vertexCandidates down to only those that match the passed quantity. This rule has the format
        // if: <anything>
        // then:
        //   capability: quantity.first
        //   type: quantity.second
        ModuleRule dep_rule;
        dep_rule.has_if = dep_rule.has_then = dep_rule.then_capability = dep_rule.then_type = true;
        dep_rule.capability = entry.quantity.first;
        dep_rule.type = entry.quantity.second;
        // Don't let functors log this rule when it is matched, as it is only a temporary rule.
        dep_rule.log_matches = false;
        
        // Iterate over all candidates
        //# pragma omp parallel for 
        for (const DRes::VertexID& v : vertexCandidates)
        {
          // Require match to quantity, and forbid self-resolution 
          if (v != entry.toVertex and dep_rule.allows(masterGraph[v], *boundTEs))
           updateCandidates(v, allowedVertexCandidates, disabledVertexCandidates);
        }
      }
      
      // Bail now if we are already down to zero candidates.
      if (allowedVertexCandidates.size() == 0)
      {
        std::ostringstream errmsg;
        errmsg << "No candidates found while trying to resolve:" << endl;
        errmsg << printQuantityToBeResolved(entry) << endl;
        if (disabledVertexCandidates.size() != 0)
        {
          errmsg << "\nNote that potentially viable candidates exist that have been disabled:\n"
                 << printGenericFunctorList(disabledVertexCandidates)
                 << endl
          << "Status flags:" << endl
          << " 0: This function is not compatible with any model you are scanning." << endl
          << "-3: This function requires a BOSSed class that is missing. The " << endl
          << "    backend that provides the class is missing (most likely), the " << endl
          << "    class is missing from the backend, or the factory functions" << endl
          << "    for this class have not been BOSSed and loaded correctly." << endl;
        }
        errmsg << "Please check your yaml file for typos, and make sure that the" << endl
        << "models you are scanning are compatible with at least one function" << endl
        << "that provides this capability (they may all have been deactivated" << endl
        << "due to having ALLOW_MODELS declarations that are" << endl
        << "incompatible with the models selected for scanning)." << endl;
        dependency_resolver_error().raise(LOCAL_INFO,errmsg.str());
      }

      logger() << LogTags::dependency_resolver;
      logger() << "List of candidate vertices:" << endl;
      logger() << printGenericFunctorList(allowedVertexCandidates) << EOM;

      // Apply any conditions imposed by subjugate rules and function chains. 
      // Note that it is not possible to write a subjugate rule nor a functionChain
      // that constrains the identity of the functor used to resolve an ObsLike entry. 
      if (entry.obslike == NULL)
      {
        std::vector<DRes::VertexID> temp_candidates;
       
        //# pragma omp parallel for 
        for (const DRes::VertexID& v : allowedVertexCandidates)
        {
          bool allowed = true;

          // Iterate over all obslikes that matched the entry.toVertex.
          for (const DRes::Observable* match : masterGraph[entry.toVertex]->getMatchedObservables())
          {          
            // Allow only candidates that are allowed by all subjugate module rules of all rules that matched the entry.toVertex
            allowed = allowed and match->dependencies_allow(masterGraph[v], *boundTEs);
            // Check that the candidate is consistent with any functionChain included in the obslike entry.
            allowed = allowed and match->function_chain_allows(masterGraph[v], masterGraph[entry.toVertex], *boundTEs);
          }

          // Iterate over all rules that matched the entry.toVertex.
          for (const DRes::ModuleRule* match : masterGraph[entry.toVertex]->getMatchedModuleRules())
          {          
            // Allow only candidates that match all subjugate module rules of all rules that matched the entry.toVertex
            allowed = allowed and match->dependencies_allow(masterGraph[v], *boundTEs);
            // Check that the candidate is consistent with any functionChain included in the rule.
            allowed = allowed and match->function_chain_allows(masterGraph[v], masterGraph[entry.toVertex], *boundTEs);
          }

          if (allowed)
          {
              //# pragma omp critical (temp_candidates) 
              temp_candidates.push_back(v);
          }
        }
        allowedVertexCandidates = temp_candidates;            
      }       

      logger() << LogTags::dependency_resolver;
      logger() << "List of candidate vertices after applying subjugate rules and functionChain constraints:" << endl;
      logger() << printGenericFunctorList(allowedVertexCandidates) << EOM;


      // Apply model-specific filter
      unsigned int remaining = allowedVertexCandidates.size();
      if (remaining > 1 and boundIniFile->getValueOrDef<bool>(true, "dependency_resolution", "prefer_model_specific_functions"))
      {
        allowedVertexCandidates = closestCandidateForModel(allowedVertexCandidates);
        if (allowedVertexCandidates.size() < remaining)
        {
          logger() << "A subset of vertex candidates is tailor-made for the scanned model." << endl;
          logger() << "After using this as an additional constraint, the remaining vertices are:" << endl;
          logger() << printGenericFunctorList(allowedVertexCandidates) << EOM;
        }
      }

      // As a last resort, try applying weak rules (both subjugate and non-subjugate).
      if (allowedVertexCandidates.size() > 1)
      {
        logger() << "Applying rules declared as '!weak' in final attempt to resolve dependency." << endl;

        if (entry.obslike == NULL)
        {
          std::vector<DRes::VertexID> temp_candidates;
         
          //# pragma omp parallel for 
          for (const DRes::VertexID& v : allowedVertexCandidates)
          {
            bool allowed = true;
  
            // Filter out vertices that fail any non-subjugate (undirected) rules.
            for (const ModuleRule& rule : module_rules)
            {
              if (rule.weakrule and allowed) allowed = rule.allows(masterGraph[v], *boundTEs, false); 
            }

            // Iterate over all obslikes that matched the entry.toVertex.
            for (const DRes::Observable* match : masterGraph[entry.toVertex]->getMatchedObservables())
            {          
              // Allow only candidates that are allowed by all subjugate module rules of all rules that matched the entry.toVertex
              allowed = allowed and match->dependencies_allow(masterGraph[v], *boundTEs, false);
              // Check that the candidate is consistent with any functionChain included in the obslike entry.
              allowed = allowed and match->function_chain_allows(masterGraph[v], masterGraph[entry.toVertex], *boundTEs);
            }
  
            // Iterate over all rules that matched the entry.toVertex.
            for (const DRes::ModuleRule* match : masterGraph[entry.toVertex]->getMatchedModuleRules())
            {          
              // Allow only candidates that match all subjugate module rules of all rules that matched the entry.toVertex
              if (match->weakrule and allowed) allowed = match->dependencies_allow(masterGraph[v], *boundTEs, false);
              // Check that the candidate is consistent with any functionChain included in the rule.
              if (match->weakrule and allowed) allowed = match->function_chain_allows(masterGraph[v], masterGraph[entry.toVertex], *boundTEs, false);
            }
  
            if (allowed)
            {
                //# pragma omp critical (temp_candidates) 
                temp_candidates.push_back(v);
            }
          }
          allowedVertexCandidates = temp_candidates;            

          logger() << "Candidate vertices after applying weak rules:" << endl;
          logger() << printGenericFunctorList(allowedVertexCandidates) << EOM;
        }       
      }


      if (allowedVertexCandidates.size() > 0)
      {
        logger() << "Candidate vertices that fulfill all rules:" << endl;
        logger() << printGenericFunctorList(allowedVertexCandidates) << EOM;
      }

      // Nothing left?
      if (allowedVertexCandidates.size() == 0)
      {
        str errmsg = "None of the vertex candidates for";
        errmsg += "\n" + printQuantityToBeResolved(entry);
        errmsg += "\nfulfills all rules in the YAML file.";
        errmsg += "\nPlease check your YAML file for contradictory rules, and";
        errmsg += "\nensure that you have built GAMBIT in the first place with";
        errmsg += "\nall of the components that you are trying to use.";
        dependency_resolver_error().raise(LOCAL_INFO,errmsg);
      }

      // Did we get down to one vertex?
      if (allowedVertexCandidates.size() == 1) return allowedVertexCandidates[0];

      str errmsg = "Unfortunately, the dependency resolution for";
      errmsg += "\n" + printQuantityToBeResolved(entry);
      errmsg += "\nis still ambiguous.\n";
      errmsg += "\nThe candidate vertices are:\n";
      errmsg += printGenericFunctorList(allowedVertexCandidates) +"\n";
      errmsg += "See logger output for details on the attempted (but failed) dependency resolution.\n";
      errmsg += "\nAn entry in the ObsLike or Rules section of your YAML file that would";
      errmsg += "\ne.g. select the first of the above candidates could read ";
      if (entry.obslike == NULL)
      {
        errmsg += "as a targeted rule:\n";
        errmsg += "\n  - capability: "+masterGraph[entry.toVertex]->capability();
        errmsg += "\n    function: "+masterGraph[entry.toVertex]->name();
        errmsg += "\n    dependencies:";
        errmsg += "\n      - capability: " +masterGraph[allowedVertexCandidates[0]]->capability();
        errmsg += "\n        function: " +masterGraph[allowedVertexCandidates[0]]->name();
        errmsg += "\n        module: " +masterGraph[allowedVertexCandidates[0]]->origin() +"\n\nor ";
        errmsg += "as an untargeted rule:\n";
      }
      errmsg += "\n  - capability: "+masterGraph[allowedVertexCandidates[0]]->capability();
      errmsg += "\n    type: "+masterGraph[allowedVertexCandidates[0]]->type();
      errmsg += "\n    function: "+masterGraph[allowedVertexCandidates[0]]->name();
      errmsg += "\n    module: " +masterGraph[allowedVertexCandidates[0]]->origin() +"\n";

      dependency_resolver_error().raise(LOCAL_INFO,errmsg);

      return 0;
    }


    /// Set up dependency tree
    void DependencyResolver::generateTree(std::queue<QueueEntry>& resolutionQueue)
    {
      OutputVertex outVertex;
      VertexID fromVertex;
      EdgeID edge;
      bool ok;

      logger() << LogTags::dependency_resolver << endl;
      logger() << "################################################" << endl;
      logger() << "#         Starting dependency resolution       #" << endl;
      logger() << "#                                              #" << endl;
      logger() << "# format: Capability (Type) [Function, Module] #" << endl;
      logger() << "################################################" << EOM;

      // Print something to stdout as well
      #ifdef DEPRES_DEBUG
        std::cout << "Resolving dependency graph..." << std::endl;
      #endif

      // Read ini entries
      print_timing   = boundIniFile->getValueOrDef<bool>(false, "print_timing_data");
      print_unitcube = boundIniFile->getValueOrDef<bool>(false, "print_unitcube");

      if ( print_timing   ) logger() << "Will output timing information for all functors (via printer system)" << EOM;
      if ( print_unitcube ) logger() << "Printing of unitCubeParameters will be enabled." << EOM;

      // Generate a list of functors able to participate in dependency resolution.  
      std::vector<DRes::VertexID> vertexCandidates;
      graph_traits<DRes::MasterGraphType>::vertex_iterator vi, vi_end;
      //#pragma omp parallel for
      for (std::tie(vi, vi_end) = vertices(masterGraph); vi != vi_end; ++vi)
      {
        bool allowed = true;

        for (const ModuleRule& rule : module_rules)
        {
          // Filter out vertices that fail any non-subjugate (undirected) rules.
          allowed = allowed and rule.allows(masterGraph[*vi], *boundTEs); 
        }

        if (allowed)
        {
          //#pragma omp critical (vertexCandidates)
          vertexCandidates.push_back(*vi);
        }
      }

      //
      // Main loop: repeat until dependency queue is empty
      //

      while (not resolutionQueue.empty())
      {

        // Retrieve dependency of interest
        const QueueEntry& entry = resolutionQueue.front(); 

        // Print information about required quantity and dependent vertex
        logger() << LogTags::dependency_resolver;
        logger() << "Resolving ";
        logger() << printQuantityToBeResolved(entry) << endl << endl;

        // Extra verbose output to terminal
        #ifdef VERBOSE_DEP_RES
          std::cout << "Resolving dependency "<<printQuantityToBeResolved(entry)<<"..." <<std::endl;
        #endif

        // Figure out how to resolve dependency
        fromVertex = resolveDependencyFromRules(entry, vertexCandidates);

        // Print user info.
        logger() << LogTags::dependency_resolver;
        logger() << "Resolved by: [";
        logger() << (*masterGraph[fromVertex]).name() << ", ";
        logger() << (*masterGraph[fromVertex]).origin() << "]" << endl;

        // Extra verbose output to terminal
        #ifdef VERBOSE_DEP_RES
          std::cout << "   ...resolved by ["<<(*masterGraph[fromVertex]).name()<<", "<<(*masterGraph[fromVertex]).origin()<<"]"<<std::endl;
        #endif

        // Check if we wanted to output this observable to the printer system.
        if (entry.obslike != NULL) masterGraph[fromVertex]->setPrintRequirement(entry.printme);
        // Check if the flag to output timing data is set
        if(print_timing) masterGraph[fromVertex]->setTimingPrintRequirement(true);

        // Apply resolved dependency to masterGraph and functors
        if (entry.obslike == NULL)
        {
          // Resolve dependency on functor level...
          //
          // In case the fromVertex is a loop manager, store nested function
          // temporarily in loopManagerMap (they have to be sorted later)
          if (entry.dependency_type == LOOP_MANAGER_DEPENDENCY)
          {
            // Check whether fromVertex is allowed to manage loops
            if (not masterGraph[fromVertex]->canBeLoopManager())
            {
              str errmsg = "Trying to resolve dependency on loop manager with\n"
               "module function that is not declared as loop manager.\n"
               + printGenericFunctorList(initVector<functor*>(masterGraph[fromVertex]));
              dependency_resolver_error().raise(LOCAL_INFO,errmsg);
            }
            std::set<DRes::VertexID> v;
            if (loopManagerMap.count(fromVertex) == 1)
            {
              v = loopManagerMap[fromVertex];
            }
            v.insert(entry.toVertex);
            loopManagerMap[fromVertex] = v;
            (*masterGraph[entry.toVertex]).resolveLoopManager(masterGraph[fromVertex]);

            // Take any dependencies of loop-managed vertices that have already been resolved,
            // and add them as "hidden" dependencies to this loop manager.
            if (edges_to_force_on_manager.find(entry.toVertex) != edges_to_force_on_manager.end())
            {
              for (auto it = edges_to_force_on_manager.at(entry.toVertex).begin();
                   it != edges_to_force_on_manager.at(entry.toVertex).end(); ++it)
              {
                logger() << "Dynamically adding dependency of " << (*masterGraph[fromVertex]).origin()
                         << "::" << (*masterGraph[fromVertex]).name() << " on "
                         << (*masterGraph[*it]).origin() << "::" << (*masterGraph[*it]).name() << endl;
                std::tie(edge, ok) = add_edge(*it, fromVertex, masterGraph);
              }
            }
          }
          // Default is to resolve dependency on functor level of entry.toVertex
          else
          {
            (*masterGraph[entry.toVertex]).resolveDependency(masterGraph[fromVertex]);
          }
          // ...and on masterGraph level.
          std::tie(edge, ok) = add_edge(fromVertex, entry.toVertex, masterGraph);

          // In the case that entry.toVertex is a nested function, add fromVertex to
          // the edges of entry.toVertex's loop manager.
          str to_lmcap = (*masterGraph[entry.toVertex]).loopManagerCapability();
          str to_lmtype = (*masterGraph[entry.toVertex]).loopManagerType();
          str from_lmcap = (*masterGraph[fromVertex]).loopManagerCapability();
          str from_lmtype = (*masterGraph[fromVertex]).loopManagerType();
          bool is_same_lmcap = to_lmcap == from_lmcap;
          bool is_same_lmtype = to_lmtype == "any" or from_lmtype == "any" or to_lmtype == from_lmtype;
          if (to_lmcap != "none")
          {
            // This function runs nested.  Check if its loop manager has been resolved yet.
            if ((*masterGraph[entry.toVertex]).loopManagerName() == "none")
            {
              // entry.toVertex's loop manager has not yet been determined.
              // Add the edge to the list to deal with when the loop manager dependency is resolved,
              // as long as entry.toVertex and fromVertex cannot end up inside the same loop.
              if (!is_same_lmcap or !is_same_lmtype)
              {
                if (edges_to_force_on_manager.find(entry.toVertex) == edges_to_force_on_manager.end())
                 edges_to_force_on_manager[entry.toVertex] = std::set<DRes::VertexID>();
                edges_to_force_on_manager.at(entry.toVertex).insert(fromVertex);
              }
            }
            else
            {
              // entry.toVertex's loop manager has already been resolved.
              // If fromVertex is not the manager itself, and is not
              // itself a nested function that has the possibility to
              // end up in the same loop as entry.toVertex, then add
              // fromVertex as an edge of the manager.
              str name = (*masterGraph[entry.toVertex]).loopManagerName();
              str origin = (*masterGraph[entry.toVertex]).loopManagerOrigin();
              bool is_itself = (name == (*masterGraph[fromVertex]).name() and origin == (*masterGraph[fromVertex]).origin());
              if (!is_itself and (!is_same_lmcap or !is_same_lmtype) )
              {
                // Hunt through the edges of entry.toVertex and find the one that corresponds to its loop manager.
                graph_traits<DRes::MasterGraphType>::in_edge_iterator ibegin, iend;
                std::tie(ibegin, iend) = in_edges(entry.toVertex, masterGraph);
                if (ibegin != iend)
                {
                  DRes::VertexID managerVertex;
                  for (; ibegin != iend; ++ibegin)
                  {
                    managerVertex = source(*ibegin, masterGraph);
                    if ((*masterGraph[managerVertex]).name() == name and
                        (*masterGraph[managerVertex]).origin() == origin) break;
                  }
                  logger() << "Dynamically adding dependency of " << (*masterGraph[managerVertex]).origin()
                           << "::" << (*masterGraph[managerVertex]).name() << " on "
                           << (*masterGraph[fromVertex]).origin() << "::" << (*masterGraph[fromVertex]).name() << endl;
                  std::tie(edge, ok) = add_edge(fromVertex, managerVertex, masterGraph);
                }
                else
                {
                  dependency_resolver_error().raise(LOCAL_INFO, "entry.toVertex has no edges! So its loop manager hasn't been added as a dependency?!");
                }
              }
            }
          }
        }
        else // if output vertex
        {
          const Observable* iniEntry = findIniEntry(quantity, boundIniFile->getObservables(), "ObsLike");
          outVertex.vertex = fromVertex;
          outVertex.purpose = iniEntry->purpose;
          outputVertices.push_back(outVertex);
          // Don't need subcaps during dry-run
          if (not boundCore->show_runorder)
          {
            Options mySubCaps = collectSubCaps(fromVertex);
            masterGraph[fromVertex]->notifyOfSubCaps(mySubCaps);
          }
        }

        // If fromVertex is new, activate it
        if ( (*masterGraph[fromVertex]).status() != 2 )
        {
          logger() << LogTags::dependency_resolver << "Activate new module function" << endl;
          masterGraph[fromVertex]->setStatus(2); // activate node
          resolveVertexBackend(fromVertex);
          resolveVertexClassLoading(fromVertex);

          // Don't need options during dry-run, so skip this (just to simplify terminal output)
          if(not boundCore->show_runorder)
          {
            Options myOptions = collectIniOptions(fromVertex);
            masterGraph[fromVertex]->notifyOfIniOptions(myOptions);
          }
          // Fill parameter queue with dependencies of fromVertex
          fillResolutionQueue(resolutionQueue, fromVertex);
        }

        // Done.
        logger() << EOM;
        resolutionQueue.pop();
      }
    }

    /// Push module function dependencies onto the parameter queue
    void DependencyResolver::fillResolutionQueue(std::queue<QueueEntry>& resolutionQueue, DRes::VertexID vertex)
    {
      // Set the default printing flag for functors to pass to the resolutionQueue constructor.
      bool printme_default = false;

      // Tell the logger what the following messages are about.
      logger() << LogTags::dependency_resolver;

      // Digest capability of loop manager (if defined)
      str lmcap = masterGraph[vertex]->loopManagerCapability();
      str lmtype = masterGraph[vertex]->loopManagerType();
      if (lmcap != "none")
      {
        logger() << "Adding module function loop manager to resolution queue:" << endl;
        logger() << lmcap << " ()" << endl;
        resolutionQueue->push(QueueEntry(sspair(lmcap, lmtype), vertex, LOOP_MANAGER_DEPENDENCY, printme_default));
      }

      // Digest regular dependencies
      std::set<sspair> s = masterGraph[vertex]->dependencies();
      if (s.size() > 0) logger() << "Add dependencies of new module function to queue" << endl;
      for (const sspair& ss : s)
      {
        // If the loop manager requirement exists and is type-specific, it is a true depencency,
        // and thus appears in the output of functor.dependencies(). So, we need to take care
        // not to double-count it for entry into the resolutionQueue.
        if (lmcap == "none" or lmtype == "any" or lmcap != ss.first or lmtype != ss.second)
        {
          logger() << ss.first << " (" << ss.second << ")" << endl;
          resolutionQueue->push(QueueEntry(ss, vertex, NORMAL_DEPENDENCY, printme_default));
        }
      }

      // Tell the logger we're done here.
      logger() << EOM;
    }

    /// Boost lib topological sort
    std::list<VertexID> DependencyResolver::run_topological_sort()
    {
      std::list<VertexID> topo_order;
      topological_sort(masterGraph, front_inserter(topo_order));
      return topo_order;
    }

    /// Node-by-node backend resolution
    void DependencyResolver::resolveVertexBackend(VertexID vertex)
    {
      functor* solution;
      std::vector<functor*> previous_successes;
      std::set<str> remaining_groups;
      std::set<sspair> remaining_reqs;
      bool allow_deferral = true;

      // If there are no backend requirements, and thus nothing to do, return.
      if (masterGraph[vertex]->backendreqs().size() == 0) return;

      // Get started.
      logger() << LogTags::dependency_resolver << "Doing backend function resolution..." << EOM;

      // Check whether this vertex is mentioned in the inifile.
      const IniParser::ObservableType * auxEntry = findIniEntry(masterGraph[vertex], boundIniFile->getRules(), "Rules", *boundTEs);

      // Collect the list of groups that the backend requirements of this vertex exist in.
      std::set<str> groups = masterGraph[vertex]->backendgroups();

      // Collect the list of orphan (i.e. groupless) backend requirements.
      std::set<sspair> orphan_reqs = masterGraph[vertex]->backendreqs("none");

      // Loop until no further backend resolutions are possible, or no more are required.
      while ( not ( groups.empty() and orphan_reqs.empty() ) )
      {

        // Loop over all groups, including the null group (group="none").
        for (const str& group : groups)
        {
          // Switch depending on whether this is a real group or not.
          if (group == "none")
          {
            // Loop over all the orphan requirements.
            for (const sspair& req : orphan_reqs)
            {
              logger() << LogTags::dependency_resolver;
              logger() << "Resolving ungrouped requirement " << req.first;
              logger() << " (" << req.second << ")..." << EOM;

              // Find a backend function that fulfills the backend requirement.
              std::set<sspair> reqsubset;
              reqsubset.insert(req);
              solution = solveRequirement(reqsubset,auxEntry,vertex,previous_successes,allow_deferral);

              // Check if a valid solution has been returned
              if (solution != NULL)
              {
                // It has, so resolve the backend requirement with that function and add it to the list of successful resolutions.
                resolveRequirement(solution,vertex);
                previous_successes.push_back(solution);

                // If *req is in remaining_reqs, remove it
                if (remaining_reqs.find(req) != remaining_reqs.end())
                {
                  remaining_reqs.erase(req);
                }
              }
              else // No valid solution found, but deferral has been suggested - so defer resolution of this group until later.
              {
                remaining_reqs.insert(req);
                logger() << LogTags::dependency_resolver;
                logger() << "Resolution of ungrouped requirement " << req.first;
                logger() << " (" << req.second << ") deferred until later." << EOM;
              }
            }
            if (not remaining_reqs.empty()) remaining_groups.insert(group);
          }
          else
          {
            logger() << LogTags::dependency_resolver;
            logger() << "Resolving from group " << group << "..." << EOM;

            // Collect the list of backend requirements in this group.
            std::set<sspair> reqs = masterGraph[vertex]->backendreqs(group);

            // Find a backend function that fulfills one of the backend requirements in the group.
            solution = solveRequirement(reqs,auxEntry,vertex,previous_successes,allow_deferral,group);

            // Check if a valid solution has been returned
            if (solution != NULL)
            {
              // It has, so resolve the backend requirement with that function and add it to the list of successful resolutions.
              resolveRequirement(solution,vertex);
              previous_successes.push_back(solution);
            }
            else // No valid solution found, but deferral has been suggested - so defer resolution of this group until later.
            {
              remaining_groups.insert(group);
              logger() << LogTags::dependency_resolver;
              logger() << "Resolution from group " << group;
              logger() << "deferred until later." << EOM;
            }
          }
        }

        // If there has been no improvement this round, turn off deferral and make the next round the last attempt.
        if (orphan_reqs == remaining_reqs and groups == remaining_groups)
        {
          allow_deferral = false;
        }
        else // Otherwise try again to resolve the remaining groups and orphan requirements, now that some others are known.
        {
          orphan_reqs = remaining_reqs;
          groups = remaining_groups;
          remaining_reqs.clear();
          remaining_groups.clear();
        }

      }

    }

    /// Find a backend function that matches any one of a vector of capability-type pairs.
    functor* DependencyResolver::solveRequirement(std::set<sspair> reqs,
     const IniParser::ObservableType * auxEntry, VertexID vertex, std::vector<functor*> previous_successes,
     bool allow_deferral, str group)
    {
      std::vector<functor*> vertexCandidates;
      std::vector<functor*> vertexCandidatesWithIniEntry;
      std::vector<functor*> disabledVertexCandidates;

      // Loop over all existing backend vertices, and make a list of
      // functors that are available and fulfill the backend requirement
      for (const std::vector<functor *>::const_iterator
          itf  = boundCore->getBackendFunctors().begin();
          itf != boundCore->getBackendFunctors().end();
          ++itf)
      {
        const IniParser::ObservableType * reqEntry = NULL;
        bool entryExists = false;

        // Find relevant iniFile entry from Rules section
        if ( auxEntry != NULL ) reqEntry = findIniEntry((*itf)->quantity(), (*auxEntry).backends, "backend");
        if ( reqEntry != NULL) entryExists = true;

        // Look for a match to at least one backend requirement, taking into account type equivalency classes.
        bool simple_match = false;
        for (std::set<sspair>::const_iterator
             itr  = reqs.begin();
             itr != reqs.end();
             ++itr)
        {
          if ((*itf)->capability() == itr->first and typeComp((*itf)->type(), itr->second, *boundTEs))
          {
            simple_match = true;
            break;
          }
        }

        // If there is a relevant inifile entry, we also check for a match to the capability, type, function name and backend name in that entry.
        if ( simple_match and ( entryExists ? backendFuncMatchesIniEntry(*itf, *reqEntry, *boundTEs) : true ) )
        {

          // Has the backend vertex already been disabled by the backend system?
          bool disabled = ( (*itf)->status() <= 0 );

          // Is it permitted to be used to fill this backend requirement?
          // First we create the backend-version pair for the backend vertex and its semi-generic form (where any version is OK).
          sspair itf_signature((*itf)->origin(), (*itf)->version());
          sspair itf_generic((*itf)->origin(), "any");
          // Then we find the set of backend-version pairs that are permitted.
          std::set<sspair> permitted_bes = masterGraph[vertex]->backendspermitted((*itf)->quantity());
          // Then we see if any match.  First we test for generic matches, where any version of any backend is allowed.
          bool permitted = ( permitted_bes.empty()
          // Next we test for semi-generic matches, where the backend matches and any version of that backend is allowed.
          or std::find(permitted_bes.begin(), permitted_bes.end(), itf_generic) != permitted_bes.end()
          // Finally we test for specific matches, where both the backend and version match what is allowed.
          or std::find(permitted_bes.begin(), permitted_bes.end(), itf_signature) != permitted_bes.end() );

          // If the backend vertex is able and allowed,
          if (permitted and not disabled)
          {
            // add it to the overall vertex candidate list
            vertexCandidates.push_back(*itf);
            // if it has an inifile entry, add it to the candidate list with inifile entries
            if (entryExists) vertexCandidatesWithIniEntry.push_back(*itf);
          }
          else if (permitted and boundCore->show_backends) // If the backend is able and we only want to show the list of backends
          {
             // add it to the overall vertex candidate list
            vertexCandidates.push_back(*itf);
            // if it has an inifile entry, add it to the candidate list with inifile entries
            if (entryExists) vertexCandidatesWithIniEntry.push_back(*itf);
          }
          else
          {
            // otherwise, add it to disabled vertex candidate list
            if (not disabled) (*itf)->setStatus(1);
            disabledVertexCandidates.push_back(*itf);
          }
        }
      }

      // If too many candidates, prefer those with entries in the inifile.
      if (vertexCandidates.size() > 1 and vertexCandidatesWithIniEntry.size() >= 1)
      {
        // Loop over the remaining candidates, and disable those without entries in the inifile.
        for (const functor* f: vertexCandidates)
        {
          if (std::find(vertexCandidatesWithIniEntry.begin(), vertexCandidatesWithIniEntry.end(), *it) == vertexCandidatesWithIniEntry.end() )
          {
            disabledVertexCandidates.push_back(f);
          }
        }
        // Set the new list of vertex candidates to be only those with inifile entries.
        vertexCandidates = vertexCandidatesWithIniEntry;
      }

      // Purge all candidates that conflict with a backend-matching rule.
      // Start by making a new vector to hold the candidates that survive the purge.
      std::vector<functor *> survivingVertexCandidates;
      // Loop over the current candidates.
      for (const functor* f : vertexCandidates)
      {
        // Set up a flag to keep track of whether anything has indicated that the candidate should be thrown out.
        bool keeper = true;
        // Retrieve the tags of the candidate.
        std::set<str> tags = masterGraph[vertex]->backendreq_tags(f->quantity());
        // Loop over the tags
        for (const str& tag : tags)
        {
          // Find out which other backend requirements exhibiting this tag must be filled from the same backend as the req this candidate would fill.
          std::set<sspair> must_match = masterGraph[vertex]->forcematchingbackend(tag);
          // Set up a flag to keep track of whether any of the other backend reqs have already been filled.
          bool others_filled = false;
          // Set up a string to keep track of which backend the other backend reqs have been filled from (if any).
          str common_backend_and_version;
          // Loop over the other backend reqs.
          for (const sspair& bereq_must_match : must_match)
          {
            // Set up a flag to indicate if the other backend req in question has been filled yet.
            bool other_filled = false;
            // Set up a string to keep track of which backend the other backend req in question has been filled from (if any).
            str filled_from;
            // Loop over the backend functors that have successfully filled backend reqs already for this funcition
            for (const functor* previous_success : previous_successes)
            {
              // Check if the current previous successful resolution (itf) was of the same backend requirement as the
              // current one of the backend requirements (mit) that must be filled from the same backend as the current candidate (it).
              if (previous_success->quantity() == bereq_must_match)
              {
                // Note that bereq_must_match (the current backend req that must be filled from the same backend as the current candidate) has indeed been filled, by previous_success
                other_filled = true;
                // Note which backend bereq_must_match has been filled from (i.e. where does previous_success come from?)
                filled_from = previous_success->origin() + " v" + previous_success->version();
                break;
              }
            }
            // If the other req has been filled, updated the tracker of whether any of the reqs linked to this flag have been filled,
            // and compare the filling backend to the one used to fill any other reqs associated with this tag.
            if (other_filled)
            {
              others_filled = true;
              if (common_backend_and_version.empty()) common_backend_and_version = filled_from; // Save the filling backend
              if (filled_from != common_backend_and_version) // Something buggy has happened and the rule is already broken(!)
              {
                str errmsg = "A backend-matching rule has been violated!";
                errmsg  += "\nFound whilst checking which backends have been used"
                           "\nto fill requirements with tag " + tag + " in function "
                           "\n" + masterGraph[vertex]->name() + " of " + masterGraph[vertex]->origin() + "."
                           "\nOne requirement was filled from " + common_backend_and_version + ", "
                           "\nwhereas another was filled from " + filled_from + "."
                           "\nThis should not happen and is probably a bug in GAMBIT.";
                dependency_resolver_error().raise(LOCAL_INFO,errmsg);
              }
            }
          }
          // Try to keep this candidate if it comes from the same backend as those already filled, or if none of the others are filled yet.
          keeper = (not others_filled or common_backend_and_version == f->origin() + " v" + f->version());
          if (not keeper) break;
        }
        if (keeper) survivingVertexCandidates.push_back(f); else disabledVertexCandidates.push_back(f);
      }
      // Replace the previous list of candidates with the survivors.
      vertexCandidates = survivingVertexCandidates;

      // Only print the status flags -5 or -6 if any of the disabled vertices has it
      bool printMathematicaStatus = false;
      for(unsigned int j=0; j < disabledVertexCandidates.size(); j++)
        if(disabledVertexCandidates[j]->status() == -5)
          printMathematicaStatus = true;
      bool printPythonStatus = false;
      for(unsigned int j=0; j < disabledVertexCandidates.size(); j++)
        if(disabledVertexCandidates[j]->status() == -6)
          printPythonStatus = true;

      // No candidates? Death.
      if (vertexCandidates.size() == 0)
      {
        std::ostringstream errmsg;
        errmsg
          << "Found no candidates for backend requirements of "
          << masterGraph[vertex]->origin() << "::" << masterGraph[vertex]->name() << ":\n"
          << reqs << "\nfrom group: " << group;
        if (disabledVertexCandidates.size() != 0)
        {
          errmsg << "\nNote that viable candidates exist but have been disabled:\n"
                 <<     printGenericFunctorList(disabledVertexCandidates)
                 << endl
                 << "Status flags:" << endl
                 << " 1: This function is available, but the backend version is not compatible with all your requests." << endl
                 << " 0: This function is not compatible with any model you are scanning." << endl
                 << "-1: The backend that provides this function is missing." << endl
                 << "-2: The backend is present, but function is absent or broken." << endl;
         if(printMathematicaStatus)
            errmsg << "-5: The backend requires Mathematica, but Mathematica is absent." << endl;
         if(printPythonStatus)
            errmsg << "-6: The backend requires Python, but pybind11 is absent." << endl;
          errmsg << endl
                 << "Make sure to check your YAML file, especially the rules" << endl
                 << "pertaining to backends."  << endl
                 << endl
                 << "Please also check that all shared objects exist for the"  << endl
                 << "necessary backends, and that they contain all the"  << endl
                 << "necessary functions required for this scan.  You may"  << endl
                 << "check the status of different backends by running"  << endl
                 << "  ./gambit backends"  << endl
                 << "You may also wish to check the specified search paths for each" << endl
                 << "backend shared library in "  << endl;
          if (Backends::backendInfo().custom_locations_exist())
          {
            errmsg << "  " << Backends::backendInfo().backend_locations()  << endl << "and"  << endl;
          }
          errmsg << "  " << Backends::backendInfo().default_backend_locations()  << endl;
        }
        dependency_resolver_error().raise(LOCAL_INFO,errmsg.str());
      }

      // Still more than one candidate...
      if (vertexCandidates.size() > 1)
      {
        // Check whether any of the remaining candidates is subject to a backend-matching rule,
        // and might therefore be uniquely chosen over the other(s) if resolution for this req is attempted again, after
        // another of the reqs subject to the same rule is resolved.
        bool rule_exists = false;
        // Loop over the remaining candidates.
        for (const functor* f : vertexCandidates)
        {
          // Retrieve the tags of the candidate.
          std::set<str> tags = masterGraph[vertex]->backendreq_tags(f->quantity());
          // Loop over the tags
          for (const str& tag : tags)
          {
            // Find if there is a backend-matching rule associated with this tag.
            rule_exists = not masterGraph[vertex]->forcematchingbackend(tag).empty();
            if (rule_exists) break;
          }
          if (rule_exists) break;
        }

        // If deferral is allowed and appears to be potentially useful, defer resolution until later.
        if (allow_deferral and rule_exists)
        {
          return NULL;
        }

        // If not, we have just one more trick up our sleeves... use the models scanned to narrow things down.
        if (boundIniFile->getValueOrDef<bool>(true, "dependency_resolution", "prefer_model_specific_functions"))
        {
          // Prefer backend functors that are more specifically tailored for the model being scanned. Do not
          // consider backend functors that are accessible via INTERPRET_AS_X links, as these are all considered
          // to be equally 'far' from the model being scanned, with the 'distance' being one step further than
          // the most distant ancestor.
          std::vector<functor*> newCandidates;
          std::set<str> s = boundClaw->get_activemodels();
          std::vector<str> parentModelList(s.begin(), s.end());
          while (newCandidates.size() == 0 and not parentModelList.empty())
          {
            for (const str& model : parentModelList)
            {
              // Test each vertex candidate to see if it has been explicitly set up to work with the model *mit
              for (const functor* f : vertexCandidates)
              {
                if (f->modelExplicitlyAllowed(model)) newCandidates.push_back(f);
              }
              // Step up a level in the model hierarchy for this model.
              model = boundClaw->get_parent(model);
            }
            parentModelList.erase(std::remove(parentModelList.begin(), parentModelList.end(), "none"), parentModelList.end());
          }
          if (newCandidates.size() != 0) vertexCandidates = newCandidates;
        }

        // Still more than one candidate, so the game is up.
        // Don't worry about too many candidates if we only want the list of required backends
        if (vertexCandidates.size() > 1 and not boundCore->show_backends)
        {
          str errmsg = "Found too many candidates for backend requirement ";
          if (reqs.size() == 1) errmsg += reqs.begin()->first + " (" + reqs.begin()->second + ")";
          else errmsg += "group " + group;
          errmsg += " of module function " + masterGraph[vertex]->origin() + "::" + masterGraph[vertex]->name()
           + "\nViable candidates are:\n" + printGenericFunctorList(vertexCandidates);
          errmsg += "\nIf you don't need all the above backends, you can resolve the ambiguity simply by";
          errmsg += "\nuninstalling the backends you don't use.";
          errmsg += "\n\nAlternatively, you can add an entry in your YAML file that selects which backend";
          errmsg += "\nthe module function " + masterGraph[vertex]->origin() + "::" + masterGraph[vertex]->name() + " should use. A YAML file entry";
          errmsg += "\nthat selects e.g. the first candidate above could read\n";
          errmsg += "\n  - capability: "+masterGraph[vertex]->capability();
          errmsg += "\n    function: "+masterGraph[vertex]->name();
          errmsg += "\n    backends:";
          errmsg += "\n      - {capability: "+vertexCandidates.at(0)->capability()+", type: "
                                             +vertexCandidates.at(0)->type()+", backend: "
                                             +vertexCandidates.at(0)->origin()+", version: "
                                             +vertexCandidates.at(0)->version()+"}\n";
          dependency_resolver_error().raise(LOCAL_INFO,errmsg);
        }
      }

      // Store the resolved backend requirements
      std::vector<sspair> resolvedBackends;
      for(auto vertex : vertexCandidates)
      {
        sspair backend(vertex->origin(), vertex->version());
        resolvedBackends.push_back(backend);
      }

      bool found = false;
      for(const auto& br : backendsRequired)
      {
        found = true;
        for(auto backend : resolvedBackends)
        {
          if(std::find(br.begin(), br.end(), backend) == br.end())
            found = false;
        }
        if(found) break;
      }
      if(not found)
      {
        backendsRequired.push_back(resolvedBackends);
      }

      // Just one candidate.  Jackpot.
      return vertexCandidates[0];
    }

    /// Resolve a backend requirement of a specific module function using a specific backend function.
    void DependencyResolver::resolveRequirement(functor* func, VertexID vertex)
    {
      masterGraph[vertex]->resolveBackendReq(func);
      logger() << LogTags::dependency_resolver;
      logger() << "Resolved by: [" << func->name() << ", ";
      logger() << func->origin() << " (" << func->version() << ")]";
      logger() << EOM;
    }

    /// Check for unused rules and options
    void DependencyResolver::checkForUnusedRules(int mpi_rank)
    {
      std::vector<Rule> unusedRules;

      const IniParser::ObservablesType & entries = boundIniFile->getRules();
      for(IniParser::ObservableType entry : entries)
      {
        #ifdef DEPRES_DEBUG
          std::cout << "checking rule with capability " << entry.capability << std::endl;
        #endif
        graph_traits<DRes::MasterGraphType>::vertex_iterator vi, vi_end;
        bool unused = true;
        for (std::tie(vi, vi_end) = vertices(masterGraph); vi != vi_end; ++vi)
        {
          // Check only for enabled functors
          if (masterGraph[*vi]->status() == 2)
          {
            if ( matchesRules(masterGraph[*vi], Rule(entry)) )
            {
              #ifdef DEPRES_DEBUG
                std::cout << "rule for capability " << entry.capability <<" used by vertex " << masterGraph[*vi]->capability() << std::endl;
              #endif
              unused = false;
              continue;
            }
          }
        }
        if (unused) unusedRules.push_back(Rule(entry));
      }

      if(unusedRules.size() > 0)
      {
        std::stringstream msg;
        msg << "The following rules and options are not used in the current scan. This will not affect the results of the scan, but if you wish to avoid this warning you must remove all unused rules and options from the yaml file." << endl;
        for(const auto& rule :unusedRules)
        {
          if(not rule.capability.empty()) msg << "  capability: " << rule.capability << endl;
          if(not rule.function.empty())   msg << "  function: " << rule.function<< endl;
          if(not rule.module.empty())     msg << "  module: " << rule.module << endl;
          if(not rule.type.empty())       msg << "  type: " << rule.type << endl;
          if(not rule.backend.empty())    msg << "  backend: " << rule.backend << endl;
          if(not rule.version.empty())    msg << "  version: " << rule.version << endl;
          if (rule.options.getNames().size() > 0)
          {
            msg << "  options:" << endl;
            msg << rule.options.toString(2);
          }
          msg << endl;
        }
        logger() << msg.str() << EOM;
        if(mpi_rank == 0) std::cout << msg.str() << std::endl;
      }
    }

    /// Construct metadata information from used observables, rules and options
    /// Note: No keys can be identical (or differing only by capitalisation)
    ///       to those printed in the main file, otherwise the sqlite printer fails
    map_str_str DependencyResolver::getMetadata()
    {
      map_str_str metadata;

      // Gambit version
      metadata["GAMBIT"] = gambit_version();

      // Date
      auto now = std::chrono::system_clock::now();
      auto in_time_t = std::chrono::system_clock::to_time_t(now);

      std::stringstream ss;
      ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M");
      metadata["Date"] =  ss.str();

      // scanID
      if (boundIniFile->getValueOrDef<bool>(true, "print_scanID"))
      {
        ss.str("");
        ss << scanID;
        metadata["Scan_ID"] = ss.str();
      }

      // Parameters
      YAML::Node parametersNode = boundIniFile->getParametersNode();
      Options(parametersNode).toMap(metadata, "Parameters");

      // Priors
      YAML::Node priorsNode = boundIniFile->getPriorsNode();
      Options(priorsNode).toMap(metadata, "Priors");

      // Printer
      YAML::Node printerNode = boundIniFile->getPrinterNode();
      Options(printerNode).toMap(metadata, "Printer");

      // Scanners
      YAML::Node scanNode = boundIniFile->getScannerNode();
      str scanner = scanNode["use_scanner"].as<str>();
      metadata["Scanner::scanner"] = scanner;
      for(const auto& entry : scanNode)
      {
        const str key = entry.first.as<str>(); 
        if(key == "scanners") Options(scanNode["scanners"][scanner]).toMap(metadata, "Scanner::options");
        else if(key != "use_scanner") Options(entry).toMap(metadata, "Scanner::" + key);
      }

      // ObsLikes
      for (const Observable& obslike : obslikes)
      {
        str key = "ObsLikes::" + obslike.capability;
        metadata[key + "::capability"] = obslike.capability;
        if(not obslike.purpose.empty())  metadata[key + "::purpose"] = obslike.purpose;
        if(not obslike.function.empty()) metadata[key + "::function"] = obslike.function;
        if(not obslike.type.empty())     metadata[key + "::type"] = obslike.type;
        if(not obslike.module.empty())   metadata[key + "::module"] = obslike.module;
        if(obslike.subcaps.size() and obslike.subcaps.IsSequence())
        {
          std::stringstream subcaps;
          subcaps << "[";
          for (const auto& sc : obslike.subcaps) subcaps << sc << ",";
          subcaps << "]";
          metadata[key + "::subcaps"] = subcaps.str();
        }
      }

      // Used rules and options
      const IniParser::ObservablesType &rules = boundIniFile->getRules();
      for (IniParser::ObservableType rule : rules)
      {
        graph_traits<DRes::MasterGraphType>::vertex_iterator vi, vi_end;
        for (std::tie(vi, vi_end) = vertices(masterGraph); vi != vi_end; ++vi)
        {
          // Check only for enabled functors
          if (masterGraph[*vi]->status() == 2)
          {
            if (matchesRules(masterGraph[*vi], Rule(rule)))
            {
              str key = "Rules";
              if(not rule.capability.empty())
              {
                key += "::" + rule.capability;
                metadata[key + "::capability"] = rule.capability;
              }
              if(not rule.function.empty())
              {
                if (not rule.capability.empty()) key += "::" + rule.function;
                metadata[key+"::function"] = rule.function;
              }
              if(not rule.module.empty())     metadata[key+"::module"] = rule.module;
              if(not rule.type.empty())       metadata[key+"::type"] = rule.type;
              if(not rule.backend.empty())    metadata[key+"::backend"] = rule.backend;
              if(not rule.version.empty())    metadata[key+"::version"] = rule.version;
              if(rule.options.getNames().size()) rule.options.toMap(metadata, key+"::options");
            }
          }
        }
      }

      // Logger
      YAML::Node logNode = boundIniFile->getLoggerNode();
      Options(logNode).toMap(metadata,"Logger");

      // KeyValues
      YAML::Node keyvalue = boundIniFile->getKeyValuePairNode();
      Options(keyvalue).toMap(metadata,"KeyValue");

      // YAML file
      ss.str("");
      ss << boundIniFile->getYAMLNode();
      metadata["YAML"] = ss.str();

      return metadata;

    }

    // Resolve a dependency on backend classes
    void DependencyResolver::resolveVertexClassLoading(VertexID vertex)
    {
      // If there are no backend class loading requirements, and thus nothing to do, return.
      if (masterGraph[vertex]->backendclassloading().size() == 0) return;

      // If the backend is not present, this vertex has already been disabled, so from now just assume it hasn't
      // Unless the list of required backends is requested, in which case it is enabled, but it won't run a scan, so no worries

      // Add to the logger
      logger() << LogTags::dependency_resolver << "Doing backend class loading resolution..." << EOM;

      // Add the backends to list of required backends
      std::vector<sspair> resolvedBackends;
      for(auto backend : masterGraph[vertex]->backendclassloading())
        resolvedBackends.push_back(backend);

      bool found = false;
      for(const auto& br : backendsRequired)
      {
        found = true;
        for(auto backend : resolvedBackends)
        {
          if(std::find(br.begin(), br.end(), backend) == br.end())
            found = false;
        }
        if(found) break;
      }
      if(not found)
      {
        backendsRequired.push_back(resolvedBackends);
      }

    }

    // Set the Scan ID
    void DependencyResolver::set_scanID()
    {
      // Get the scanID from the yaml node.
      scanID = boundIniFile->getValueOrDef<int>(-1, "scanID");

      // If scanID is supplied by user, use that
      if (scanID != -1)
      {
        return;
      }
      else
      {
        const std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
        std::time_t in_time_t = std::chrono::system_clock::to_time_t(now);
        std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds> (now.time_since_epoch()) - std::chrono::duration_cast<std::chrono::seconds> (now.time_since_epoch());
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%H%M%S");
        ss << ms.count();
        ss >> scanID;
      }
    }

    // Get BibTeX citation keys for backends, modules, etc
    void DependencyResolver::getCitationKeys()
    {
      // First add the necessary citation keys to use GAMBIT
      citationKeys.insert(citationKeys.end(), gambit_citation_keys.begin(), gambit_citation_keys.end());

      // Get the keys for the required backends
      for(auto backend : backendsRequired)
      {
        str bibkey = "";

        // Run over references of loaded backends
        for(auto beref : boundCore->getBackendCitationKeys())
        {
          str origin = beref.first.first;
          str version = beref.first.second;
          if (backend[0].first == origin and backend[0].second == version)
          {
            bibkey = beref.second;
            if (bibkey != "" and bibkey != "REFERENCE")
            {
              logger() << LogTags::dependency_resolver << "Found bibkey for backend " << origin << " version " << version << ": " << bibkey << EOM;
              BibTeX::addCitationKey(citationKeys, bibkey);
            }
          }
        }
        if (bibkey == "" or bibkey == "REFERENCE")
        {
          std::ostringstream errmsg;
          errmsg << "Missing reference for backend " << backend[0].first << "(" << backend[0].second << ")." << endl;
          errmsg << "Please add the bibkey to the frontend header, and full bibtex entry to ";
          errmsg << boundIniFile->getValueOrDef<str>("config/bibtex_entries.bib", "dependecy_resolution", "bibtex_file_location") << "." << endl;
          dependency_resolver_error().raise(LOCAL_INFO,errmsg.str());
        }
      }

      // Now look over activated vertices in the mastergraph and add any references to module, module functions, etc
      for (const VertexID& vertex : getObsLikeOrder())
      {
        std::set<VertexID> parents;
        getParentVertices(vertex, masterGraph, parents);
        parents.insert(vertex);
        for (const VertexID& vertex2 : parents) 
        {

          // Add citation key for used modules
          for(const auto& key : boundCore->getModuleCitationKeys())
          {
            if(key.first == masterGraph[vertex2]->origin())
            {
              BibTeX::addCitationKey(citationKeys, key.second);
            }
          }

          // Add citation key for specific module functions
          if(masterGraph[vertex2]->citationKey() != "")
          {
            BibTeX::addCitationKey(citationKeys, masterGraph[vertex2]->citationKey());
          }

        }

      }
    }

  }

}
