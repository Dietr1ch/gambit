//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Simple container used for storing info about
///  backends during initialisation time.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Pat Scott
///          (patscott@physics.mcgill.ca)
///  \date 2014 Aug
///  \date 2015 May
///
///  *********************************************

#ifndef __backend_info_hpp__
#define __backend_info_hpp__

#include <map>
#include "gambit/Utils/util_types.hpp"
#include "yaml-cpp/yaml.h"

namespace Gambit
{

  namespace Backends
  {

    /// Structure providing some basic info on backend libraries
    struct backend_info
    {

      public:

        /// Constructor
        backend_info();

        /// Indicate whether a custom backend locations file exists
        bool custom_locations_exist() const;

        /// Return the path to any custom user backend locations file
        str backend_locations() const;

        /// Return the path to the default backend locations file
        str default_backend_locations() const;

        /// Return the path to a backend library
        str path(str, str) const;

        /// Return the path to a backend library with GAMBIT_DIR expanded
        str corrected_path(str, str) const;

        /// Return the path to the folder in which a backend library resides
        str path_dir(str, str) const;

        /// Key: backend name + version
        std::map<str,str> dlerrors;

        /// Key: backend name (map from BOSSed backend names to their default safe versions)
        std::map<str, str> default_safe_versions;

        /// Key: backend name + version
        std::map<str,bool> works;

        /// Key: backend name + version
        std::map<str,bool> classloader;

        /// Key: backend name + version
        std::map<str,bool> classes_OK;

        /// Key: backend name + version
        std::map<str,std::set<str> > classes;

        /// Key: backend name + version + class name
        std::map<str,std::set<str> > factory_args;

        /// Key: backend name + version + class name + factory args
        std::map<str,str> constructor_status;

        /// Given a backend and a safe version (with no periods), return the true version
        str version_from_safe_version (str, str) const;

        /// Given a backend and a true version (with periods), return the safe version
        str safe_version_from_version (str, str) const;

        /// Link a backend's version and safe version
        void link_versions(str, str, str);

        /// Override a backend's config file location
        void override_path(str&, str&, str);

        /// Get the default version of a BOSSed backend.
        str default_version(const str& be) const;

        /// Get all versions of a given backend that are successfully loaded.
        std::vector<str> working_versions(const str&);

        /// Get all safe versions of a given backend that are successfully loaded.
        std::vector<str> working_safe_versions(const str&);


      private:

        /// Map from backend names to maps between version and safe version
        std::map<str,std::pair<std::map<str,str>,std::map<str,str> > > safe_version_map;

        /// Map from backend names to maps between version and paths found by dlinfo
        std::map<str, std::map<str, str> > bepathoverrides;

        /// Filename in which to find the user's custom backend locations configuration file.
        const str filename;

        /// Filename in which to find the default backend locations configuration file.
        const str default_filename;

        /// YAML node corresponding to user custom backend locations configuration file.
        YAML::Node bepathfile;

        /// YAML node corresponding to default backend locations configuration file.
        YAML::Node default_bepathfile;

        /// Flag indicating whether or not the user has a custom backend locations file
        bool custom_bepathfile_exists;
    };

  }

}


#endif // defined __backend_info_hpp__
