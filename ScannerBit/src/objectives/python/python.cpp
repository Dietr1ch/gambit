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

#include "gambit/cmake/cmake_variables.hpp"
#ifdef HAVE_PYBIND11
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

#include "gambit/Utils/begin_ignore_warnings_pybind11.hpp"
#include <pybind11/embed.h>
#include "gambit/Utils/end_ignore_warnings.hpp"

#include "gambit/ScannerBit/objective_plugin.hpp"

namespace py = pybind11;

namespace Gambit
{
    
    class gambit_scoped_interpreter
    {
    private:
        static size_t count;
        
    public:
        gambit_scoped_interpreter();
        
        ~gambit_scoped_interpreter();
    };
    
    namespace Scanner 
    {
        
        inline py::object yaml_to_dict(const YAML::Node &node)
        {
            if (node.IsMap())
            {
                py::dict d;
                for (auto &&n : node)
                {
                    d[py::cast(n.first.as<std::string>())] = yaml_to_dict(n.second);
                }
                
                return d;
            }
            else if (node.IsSequence())
            {
                py::list l;
                
                for (auto &&n : node)
                {
                    l.append(yaml_to_dict(n));
                }
                
                return l;
            }
            else if (node.IsScalar())
            {
                int ret;
                if (YAML::convert<int>::decode(node, ret))
                    return py::cast(ret);
                else 
                {
                    double ret;
                    if (YAML::convert<double>::decode(node, ret))
                        return py::cast(ret);
                    else
                    {
                        return py::cast(node.as<std::string>());
                    }
                }
            }
            else
            {
                return py::object();
            }
        }
        
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
    reqd_headers("PYTHONLIBS");
    reqd_headers("pybind11");
    
    ::Gambit::gambit_scoped_interpreter guard;
    
    /*!
     * Instance of python scanner
     */
    py::object instance;
    py::object run_func;

    /*!
     * Yaml file options
     */
    //py::kwargs run_options;
    
    //bool use_run_options = false;

    plugin_constructor 
    {
        ::Gambit::Scanner::Plugins::ObjPyPlugin::pythonPluginData() = &__gambit_plugin_namespace__::myData;

        // get yaml as dict
        py::dict options = yaml_to_dict(get_inifile_node());

        // get kwargs
        py::kwargs init_kwargs;
        if (options.contains("init") && py::isinstance<py::dict>(options["init"]))
            init_kwargs = py::dict(options["init"]);
        else
            init_kwargs = options;
        
        /*if (options.contains("run") && py::isinstance<py::dict>(options["run"]))
        {
            use_run_options = true;
            run_options = py::dict(options["run"]);
        }*/
        // make instance of plugin
        py::module file;
        std::string pkg = get_inifile_value<std::string>("pkg", "");
        std::string plugin_name = get_inifile_value<std::string>("plugin");
        try 
        {
            if (pkg == "")
            {
                Gambit::Scanner::Plugins::plugin_info.load_python_plugins();
                decltype(auto) details =  Gambit::Scanner::Plugins::plugin_info.load_python_plugin("objective", plugin_name);
                py::list(py::module::import("sys").attr("path")).append(py::cast(details.loc));
                file = py::module::import(details.package.c_str());
            }
            else
            {
                std::string::size_type pos = pkg.rfind("/");
                std::string pkg_name;
                if (pos != std::string::npos)
                {
                    pkg_name = pkg.substr(pos+1);
                    while(pos != 0 && pkg[pos-1] == '/') --pos;
                    std::string path = pkg.substr(0, pos);
                    py::list(py::module::import("sys").attr("path")).append(py::cast(path));
                }
                else
                    pkg_name = pkg;
                
                py::list(py::module::import("sys").attr("path")).append(py::cast(GAMBIT_DIR "/ScannerBit/src/objectives/python/plugins"));
                file = py::module::import(pkg_name.c_str());
            }
            
            instance = py::dict(file.attr("__plugins__"))[plugin_name.c_str()](**init_kwargs);
            run_func = instance.attr("run");
        }
        catch (std::exception &ex)
        {
            scan_err << "Error loading plugin \"" << plugin_name << "\": " << ex.what() << scan_end;
        }
    }
    
    double plugin_main(std::unordered_map<std::string, double> &map)
    {
        return ::Gambit::Scanner::Plugins::ObjPyPlugin::run(run_func, map);
    }
    
    plugin_deconstructor
    {
    }
}

#endif
