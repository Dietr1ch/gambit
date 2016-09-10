//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Base class for ini-file parsers using yaml-cpp
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///   
///  \author Christoph Weniger
///          (c.weniger@uva.nl)
///  \date 2013 May, June, July
///
///  \author Pat Scott
///          (patscott@physics.mcgill.ca)
///  \date 2014 Mar
///
///  *********************************************

#include <iostream>

#include "gambit/Utils/yaml_parser_base.hpp"
#include "gambit/Utils/util_functions.hpp"
#include "gambit/Utils/mpiwrapper.hpp"
#include "gambit/Logs/logger.hpp"
#include "gambit/Logs/logmaster.hpp"


namespace Gambit
{

  namespace IniParser
  {

    /// Recursive import
    int importRound(YAML::Node node)
    {
      int counter = 0;
      if (node.IsScalar())
      {
        if ( node.Tag() == "!import" )
        {
          #ifdef WITH_MPI
            int rank = GMPI::Comm().Get_rank();
          #else
            rank = 0;
          #endif
          YAML::Node import;
          std::string filename = node.as<std::string>();
          if (rank == 0) std::cout << "Importing: " << filename << std::endl;
          try
          { 
            import = YAML::LoadFile(filename);
          } 
          catch (YAML::Exception &e)
          {
            std::ostringstream msg;
            msg << "Error reading Inifile \""<<filename<<"\" recursively! ";
            msg << "Please check that file exist!" << endl;
            msg << "(yaml-cpp error: "<<e.what()<<" )";
            inifile_error().raise(LOCAL_INFO,msg.str());
          }
          node = import;
          return 1;
        }
        return 0;
      }
      if (node.IsSequence())
      {
        for (unsigned int i = 0; i<node.size(); ++i)
        {
          counter += importRound(node[i]);
        }
        return counter;
      }
      if (node.IsMap())
      {
        for (YAML::const_iterator it = node.begin(); it != node.end(); ++it)
        {
          counter += importRound(it->second);  // Only values are processed
        }
        return counter;
      }
      return 0;
    }

    void recursiveImport(YAML::Node node)
    {
      int import_counter = 0;
      int last_import_counter = 0;
      for ( int i = 0; i < 10; ++i)
      {
        last_import_counter = importRound(node);
        import_counter += last_import_counter;
      }
      if (last_import_counter > 0)
      {
        #ifdef WITH_MPI
          int rank = GMPI::Comm().Get_rank();
        #else
          rank = 0;
        #endif
        if (rank == 0)
        {
          std::cout << last_import_counter << std::endl;
          std::cout << "WARNING: YAML imports were truncated after 10 recursions." << std::endl;
        }
      }
    }


    // Implementations of main inifile class

    void Parser::readFile(std::string filename)
    {
      YAML::Node root = filename_to_node(filename);
      basicParse(root,filename);
    }

    YAML::Node Parser::filename_to_node(std::string filename)
    {
      YAML::Node root;
      // Read inifile file
      try
      { 
        root = YAML::LoadFile(filename);
      } 
      catch (YAML::Exception &e)
      {
        std::ostringstream msg;
        msg << "Error reading Inifile \""<<filename<<"\"! ";
        msg << "Please check that file exist!" << endl;
        msg << "(yaml-cpp error: "<<e.what()<<" )";
        inifile_error().raise(LOCAL_INFO,msg.str());
      }
      return root;
    }

    void Parser::basicParse(YAML::Node root, std::string filename)
    {
      recursiveImport(root);
      parametersNode = root["Parameters"];
      priorsNode = root["Priors"];
      printerNode = root["Printer"];
      scannerNode = root["Scanner"];
      logNode = root["Logger"];
      keyValuePairNode = root["KeyValues"];

      // Set default output path
      std::string defpath;
      if(hasKey("default_output_path"))
      {
         defpath = getValue<std::string>("default_output_path");
      }
      else
      {
         // Assign a default default (;)) path based on the yaml file name
         // Ridiculously we have to parse manually in C++ since no
         // standard library tools for doing this exist...
         // Assumes that file extension has only one dot, or that
         // there is no file extension. Should work anyway if more
         // dots, will just get a directory name with a dot in it.
         size_t fname_start = filename.find_last_of("/\\");
         size_t fname_end   = filename.find_last_of(".");
         str fname = filename.substr(fname_start+1,fname_end);
         defpath = "runs/" + fname + "/";
      }
      scannerNode["default_output_path"] = Utils::ensure_path_exists(defpath+"/scanner_plugins/");
      logNode    ["default_output_path"] = Utils::ensure_path_exists(defpath+"/logs/");
      printerNode["options"]["default_output_path"] = Utils::ensure_path_exists(defpath+"/samples/");

      // Pass on minimum recognised lnlike to Scanner
      scannerNode["model_invalid_for_lnlike_below"] = getValue<double>("likelihood", "model_invalid_for_lnlike_below");

      // Set fatality of exceptions
      if (hasKey("exceptions"))
      {       
        // Iterate over the map of all recognised exception objects
        std::map<const char*,exception*>::const_iterator iter;
        for (iter = exception::all_exceptions().begin(); iter != exception::all_exceptions().end(); ++iter)
        {
          // Check if the exception has an entry in the YAML file
          if (hasKey("exceptions",iter->first))
          { 
            // Retrieve the entry and set the exception's 'fatal' flag accordingly.
            str value = getValue<str>("exceptions",iter->first);
            if (value == "fatal")
            {
              iter->second->set_fatal(true);
            }
            else if (value == "non-fatal")
            {
              iter->second->set_fatal(false);
            }
            else
            {
              str error_msg = "Unrecognised entry \"" + value + "\" for exceptions key \"" + iter->first + "\" in input file.";
              inifile_error().raise(LOCAL_INFO,error_msg);
            }
          }
        }
      }

      // Parse the logging setup node, and initialise the LogMaster object
      std::string prefix;
      if(logNode["prefix"])
      {
         prefix = logNode["prefix"].as<std::string>();
      }
      else
      {
         prefix = logNode["default_output_path"].as<std::string>()+"/";
      }

      // map storing info used to set up logger objects
      std::map<std::set<std::string>,std::string> loggerinfo;
      if(logNode["redirection"])
      {
         YAML::Node redir = logNode["redirection"];
         for(YAML::const_iterator it=redir.begin(); it!=redir.end(); ++it) 
         {
             std::set<std::string> tags;
             std::string filename;
             // Iterate through tags and add them to the set 
             YAML::Node yamltags = it->first;
             for(YAML::const_iterator it2=yamltags.begin();it2!=yamltags.end();++it2)         
             {
               tags.insert( it2->as<std::string>() );
             }
             filename = (it->second).as<std::string>();
    
             // Add entry to the loggerinfo map
             if((filename=="stdout") or (filename=="stderr"))
             {
               // Special cases to trigger redirection to standard output streams
               loggerinfo[tags] = filename;
             }
             else
             {
               // The logger won't be able to create the log files if the prefix 
               // directory doesn't exist, so let us now make sure that it does
               loggerinfo[tags] = Utils::ensure_path_exists(prefix + filename);
             }
         }
      }
      else
      {
         // Use default log file only
         std::set<std::string> tags;
         std::string filename;
         tags.insert("Default");
         filename = "default.log"; 
         loggerinfo[tags] = Utils::ensure_path_exists(prefix + filename);
     }
      // Initialise global LogMaster object
      bool master_debug = (keyValuePairNode["debug"]) ? keyValuePairNode["debug"].as<bool>() : false; 
      bool logger_debug = (logNode["debug"])          ? logNode["debug"].as<bool>()          : false; 
      logger().set_log_debug_messages(master_debug or logger_debug);
      logger().initialise(loggerinfo);

      // Parse the Parameters node and expand out some shorthand syntax
      // e.g.
      //  model1
      //    param1: 5.678
      // expands to
      //  model1
      //    param1:
      //      fixed_value: 5.678
      // Parameter must have no entries besides the value for this syntax to be valid
    }

    /// Getters for key/value section
    /// @{
    YAML::Node Parser::getParametersNode()   const {return parametersNode;}
    YAML::Node Parser::getPriorsNode()       const {return priorsNode;}
    YAML::Node Parser::getPrinterNode()      const {return printerNode;}
    YAML::Node Parser::getScannerNode()      const {return scannerNode;}
    YAML::Node Parser::getLoggerNode()       const {return logNode;}
    YAML::Node Parser::getKeyValuePairNode() const {return keyValuePairNode;}
    /// @}

    /// Getters for model/parameter section
    /// @{
    bool Parser::hasModelParameterEntry(std::string model, std::string param, std::string key) const
    {
      return parametersNode[model][param][key];
    }

    /// Return list of model names (without "adhoc" model!)
    const std::set<str> Parser::getModelNames() const
    {
      std::set<str> result;
      for (YAML::const_iterator it = parametersNode.begin(); it!=parametersNode.end(); ++it)
      {
        if (it->first.as<std::string>() != "adhoc")
          result.insert( it->first.as<std::string>() );
      }
      return result;
    }

    const std::vector<std::string> Parser::getModelParameters(std::string model) const
    {
      std::vector<std::string> result;
      if (parametersNode[model])
      {
        for (YAML::const_iterator it = parametersNode[model].begin(); it!=parametersNode[model].end(); ++it)
        {
          result.push_back( it->first.as<std::string>() );
        }
      }
      return result;
    }

    /// Getter for options
    const Options Parser::getOptions(std::string key) const
    {
      if (hasKey(key, "options"))
      {
        return Options(keyValuePairNode[key]["options"]);
      }
      else
      {
        return Options(keyValuePairNode[key]);
      }
    }

    /// @}


  }
}
