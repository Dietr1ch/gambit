//  GAMBIT: Global and Modular BSM Inference Tool
//  *********************************************
//
//  Dependecy resolution with boost graph library
//
//  *********************************************
//
//  Authors
//  =======
//
//  (add name and date if you modify)
//
//  Christoph Weniger
//  May 03 2013
//  Pat Scott 
//  May 03 2013
//
//  *********************************************
//
//  NOTE: 
//    - "vertex" = module function
//    - "edge" = dependency

#include <logcore.hpp>
#include <graphs.hpp>
#include <ini_parser.hpp>

namespace GAMBIT
{
  namespace Graphs
  {

    // Saved calling order for functions
    list<int> function_order;

    // pushes dependencies of vertex into parameter queue
    void fill_parQueue(queue<pair<sspair, Graphs::VertexID> > *parQueue,
        Graphs::VertexID vertex) {
      (*Graphs::masterGraph[vertex]).setStatus(2);
      cout << "Adding " << (*Graphs::masterGraph[vertex]).name() 
        << " to parameter queue, with dependencies" << endl;
      vector<sspair> vec = (*Graphs::masterGraph[vertex]).dependencies();
      for (vector<sspair>::iterator it = vec.begin(); it != vec.end(); ++it) {
        cout << "  " << (*it).first << " (" << (*it).second << ")" << endl;
        (*parQueue).push(*(new pair<sspair, Graphs::VertexID> (*it, vertex)));
      }
    }

    // initializes (capability, type) --> vertex multimap
    multimap<sspair, Graphs::VertexID> initialize_capMap() {
      cout << endl << "Filling capability map with" << endl;
      multimap<sspair, Graphs::VertexID> capMap;
      graph_traits<Graphs::MasterGraphType>::vertex_iterator vi, vi_end;
      pair<sspair, Graphs::VertexID> ins;
      for (tie(vi, vi_end) = vertices(Graphs::masterGraph); vi != vi_end; ++vi) {
        ins.first.first = (*Graphs::masterGraph[*vi]).capability();
        ins.first.second = (*Graphs::masterGraph[*vi]).type();
        ins.second = *vi;
        cout << "  " << ins.first.first << " (" << ins.first.second << ")" << endl;
        capMap.insert(ins);
      };
      return capMap;
    }

    // *** Main dependency resolution ***
    // This function produces an acyclic dependency tree as edges of
    // masterGraph
    // TODO: 
    // - Dependency resolution in non-unique cases
    // - Conditional dependencies
    // - Map edges on dependency pointers
    void initialize_edges(queue<pair<sspair, Graphs::VertexID> > parQueue, multimap<sspair, Graphs::VertexID> capMap) {
      cout << endl << "Setting up edges/dependencies:" << endl;
      int key_multiplicity;
      bool ok;
      Graphs::VertexID fromVertex, toVertex;
      sspair var;
      Graphs::EdgeID current_edge;
      while (not parQueue.empty()) {
        var = parQueue.front().first;
        toVertex = parQueue.front().second;
        cout << var.first << " (" << var.second << ")" << ": ";
        key_multiplicity = capMap.count(var);
        if ( key_multiplicity == 0 ) {
          cout << "Cannot resolve dependency." << endl;
          cout << "---> QUIT" << endl;
        }
        if ( key_multiplicity > 1 ) {
          cout << "Non-unique dependency resolution." << endl;
          // TODO: Dependency resolution
          exit(0);
        }
        fromVertex = (*capMap.find(var)).second;
        cout << "resolved." << endl;
        if ( (*Graphs::masterGraph[fromVertex]).status() != 2 ) {
          fill_parQueue(&parQueue, fromVertex);
        }
        cout << "Adding edge: ";
        cout << (*Graphs::masterGraph[fromVertex]).name() << " --> ";
        cout << (*Graphs::masterGraph[toVertex]).name() << endl;
        tie(current_edge, ok) = add_edge(fromVertex, toVertex, Graphs::masterGraph);
        (*Graphs::masterGraph[toVertex]).resolveDependency(Graphs::masterGraph[fromVertex]);
        Graphs::masterGraph[current_edge].variable = var;
        parQueue.pop();
      }
      cout << endl;
    }

    // The boost lib topolical sort
    list<int> run_topological_sort() {
      list<int> topo_order;
      topological_sort(Graphs::masterGraph, front_inserter(topo_order));
      return topo_order;
    }

    // Lists all vertices in correct order
    void list_functions(list<int> topo_order) {
      cout << "Dependency resolver says:  I will run the module functions in this order (and only the active ones):" << endl;
      for(list<int>::const_iterator i = topo_order.begin();
          i != topo_order.end();
          ++i)
      {
        cout << "  " << (*Graphs::masterGraph[*i]).name() ;
        if ( (*Graphs::masterGraph[*i]).status() == 0 ) cout << " (disabled)" << endl;
        if ( (*Graphs::masterGraph[*i]).status() == 1 ) cout << " (available)" << endl;
        if ( (*Graphs::masterGraph[*i]).status() == 2 ) cout << " (ACTIVE)" << endl;
      }
    }

    // Executes active vertices in correct order
    void execute_functions(list<int> topo_order) {
      cout << "Dependency resolver says: now I will actually run them." << endl;
      for(list<int>::const_iterator i = topo_order.begin();
          i != topo_order.end();
          ++i)
      {
        if ( (*Graphs::masterGraph[*i]).status() == 2 )
        {
          (*Graphs::masterGraph[*i]).calculate();
        }
      }
    }

    // Return active vertices in correct order
    std::vector<functor*> get_functors() {
      std::vector<functor*> functor_list;
      cout << "Dependency resolver says: now I will actually run them." << endl;
      for(list<int>::const_iterator i = function_order.begin();
          i != function_order.end();
          ++i)
      {
        if ( (*Graphs::masterGraph[*i]).status() == 2 )
        {
          functor_list.push_back(Graphs::masterGraph[*i]);
        }
      }
      return functor_list;
    }

    // Overload for calling without arguments
    void execute_functions() { execute_functions(function_order); }

    // Convenience function
    void list_graphs_content() 
    {
      graph_traits<Graphs::MasterGraphType>::vertex_iterator vi, vi_end;
      cout << "List of registered vertices" << endl;
      for (tie(vi, vi_end) = vertices(Graphs::masterGraph); vi != vi_end; ++vi) {
        cout << "  " << (*Graphs::masterGraph[*vi]).name() << endl;
        cout << "    Capability: " << (*Graphs::masterGraph[*vi]).capability();
        cout << " (" << (*Graphs::masterGraph[*vi]).type() << ")" << endl;
        cout << "    Module: " << (*Graphs::masterGraph[*vi]).origin();
        cout << " (" << (*Graphs::masterGraph[*vi]).version() << ")" << endl;
      };
    };

    int identify_vertex(ini_parser::ObservableType observable)
    {
      multimap<sspair, Graphs::VertexID> capMap = initialize_capMap();
      // TODOCW Check capability + type
    };

    // Main dependency resolution
    void dependency_resolution(std::vector<int> &pars)
    {
      list_graphs_content();
      queue<pair<sspair, Graphs::VertexID> > parQueue;
      multimap<sspair, Graphs::VertexID> capMap = initialize_capMap();

      for (std::vector<int>::iterator it = pars.begin() ; it != pars.end(); ++it)
      {
        fill_parQueue(&parQueue, vertex(*it, Graphs::masterGraph));
      }

      initialize_edges(parQueue, capMap);
      function_order = run_topological_sort();
      list_functions(function_order);
    };
  };
};