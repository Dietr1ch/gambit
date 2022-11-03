//  GAMBIT: Global and Modular BSM Inference Tool
//  *********************************************
///  \file
///
///  Grid sampler.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
//
///  \author Gregory Martinez
///          (gregory.david.martinez@gmail.com)
///  \date 2013 August
///
///  *********************************************

#ifdef WITH_MPI
#include "gambit/Utils/begin_ignore_warnings_mpi.hpp"
#include "mpi.h"
#include "gambit/Utils/end_ignore_warnings.hpp"
#endif

#include <vector>
#include <string>
#include <cmath>
#include <iostream>
#include <sstream>

#include <pybind11/embed.h>

#include "gambit/ScannerBit/objective_plugin.hpp"

namespace py = pybind11;

namespace Gambit
{
    
    namespace Scanner 
    {
        
        namespace Plugins
        {
            
            namespace ObjPyPlugin
            {
                
                pluginData *&pythonPluginData();
                double run(py::object, std::unordered_map<std::string,double> &);
                
            }
            
        }
        
    }
    
}

objective_plugin(python, version(1, 0, 0))
{
    reqd_headers("pybind11");
    py::module_ file;
    py::object pyplugin;
    py::object main_func;
    py::scoped_interpreter *guard = nullptr;
    
    plugin_constructor
    {
        try
        {
            guard = new py::scoped_interpreter();
        }
        catch(std::exception &)
        {
            guard = nullptr;
        }

        ::Gambit::Scanner::Plugins::ObjPyPlugin::pythonPluginData() = &__gambit_plugin_namespace__::myData;

        std::string fname = get_inifile_value<std::string>("pyplugin");
        if (fname.substr(fname.size() - 3) == ".py")
            fname = fname.substr(0, fname.size() - 3);
        if (fname.substr(fname.size() - 4) == ".pyc")
            fname = fname.substr(0, fname.size() - 4);
        
        std::string path = get_inifile_value<std::string>("dir", GAMBIT_DIR "/ScannerBit/src/objectives/python");
        
        py::list(py::module::import("sys").attr("path")).append(py::cast(path));

        file = py::module::import(fname.c_str());
        
        if (!py::hasattr(file, "objective_plugin"))
            scan_err << "\"objective_plugin\" has not been defined in \"" << fname << "\"." << scan_end;
        
        pyplugin = file.attr("objective_plugin")();
        
        if (!py::hasattr(pyplugin, "plugin_main"))
            scan_err << "\"plugin_main\" has not been defined in \"" << fname << "\"." << scan_end;
        
        main_func = pyplugin.attr("plugin_main");
    }

    double plugin_main(std::unordered_map<std::string, double> &map)
    {
        return ::Gambit::Scanner::Plugins::ObjPyPlugin::run(main_func, map);
    }
    
    plugin_deconstructor
    {
        if (guard != nullptr)
            delete guard;
    }
}
