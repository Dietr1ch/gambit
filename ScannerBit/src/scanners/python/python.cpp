//  GAMBIT: Global and Modular BSM Inference Tool
//  *********************************************
///  \file
///
///  Make an instance of python scanner and define 
///  method to run it
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Gregory Martinez
///          (gregory.david.martinez@gmail.com)
///  \date 2023 May
///
///  \author Andrew Fowlie
///          (andrew.j.fowlie@googlemail.com)
///  \date 2023 May
///
///  \author Anders Kvellestad
///          (anders.kvellestad@fys.uio.no)   
///  \date 2023 May
///
///  \author Anders Kvellestad
///          (anders.kvellestad@fys.uio.no)   
///  \date 2023 May
///
///  \author Pat Scott
///          (patrickcolinscott@gmail.com)   
///  \date 2023 May
///
///  *********************************************

#include "gambit/cmake/cmake_variables.hpp"

#ifdef HAVE_PYBIND11

#ifdef WITH_MPI
    #include "gambit/Utils/begin_ignore_warnings_mpi.hpp"
    #include "mpi.h"
    #include "gambit/Utils/end_ignore_warnings.hpp"
#endif

#include "gambit/Utils/begin_ignore_warnings_pybind11.hpp"
#include <pybind11/embed.h>
#include "gambit/Utils/end_ignore_warnings.hpp"

#include "gambit/Utils/python_interpreter.hpp"
#include "gambit/ScannerBit/scanner_plugin.hpp"

namespace py = pybind11;

namespace Gambit
{
        
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
                bool ret;
                if (YAML::convert<bool>::decode(node, ret))
                {
                    return py::cast(ret);
                }
                else
                {
                    int ret;
                    if (YAML::convert<int>::decode(node, ret))
                    {
                        return py::cast(ret);
                    }
                    else
                    {
                        double ret;
                        if (YAML::convert<double>::decode(node, ret))
                        {
                            return py::cast(ret);
                        }
                        else
                        {
                            return py::cast(node.as<std::string>());
                        }
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
            namespace ScannerPyPlugin 
            {
                pluginData *&pythonPluginData();
            }  // end namespace ScannerPyPlugin
        }  // end namespace Plugins
    }  // end namespace Scanner
}  // end namespace Gambit

scanner_plugin(python, version(1, 0, 0))
{
    reqd_headers("PYTHONLIBS");
    reqd_headers("pybind11");

    Gambit::Utils::python_interpreter_guard g;
    
    /*!
     * Instance of python scanner
     */
    py::object instance;
    py::object run_func;

    /*!
     * Yaml file options
     */
    py::kwargs run_options;
    
    bool use_run_options = false;

    plugin_constructor
    {
        Gambit::Scanner::Plugins::ScannerPyPlugin::pythonPluginData() = &__gambit_plugin_namespace__::myData;

        // get plugin name
        std::string plugin_name = get_inifile_value<std::string>("plugin");

        // get yaml as dict
        py::dict options = yaml_to_dict(get_inifile_node());
        
        // log a warning if both 'init' and 'run' options are missing
        if (!options.contains("init") && !options.contains("run"))
        {
            scan_warn << 
                "Neither an 'init' nor a 'run' section was found in "
                "the YAML options for the scanner " << plugin_name << ", "
                "Some scanner need these options to run." << scan_end;
        }

        // get kwargs
        py::kwargs init_kwargs;
        if (options.contains("init") && py::isinstance<py::dict>(options["init"]))
            init_kwargs = py::dict(options["init"]);
        else
            init_kwargs = options;
        
        if (options.contains("run") && py::isinstance<py::dict>(options["run"]))
        {
            use_run_options = true;
            run_options = py::dict(options["run"]);
        }
        
        // make instance of plugin
        py::module file;
        std::string pkg = get_inifile_value<std::string>("pkg", "");
        try 
        {
            if (pkg == "")
            {
                Gambit::Scanner::Plugins::plugin_info.load_python_plugins();
                decltype(auto) details =  Gambit::Scanner::Plugins::plugin_info.load_python_plugin("scanner", plugin_name);
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
                
                py::list(py::module::import("sys").attr("path")).append(py::cast(GAMBIT_DIR "/ScannerBit/src/scanners/python/plugins"));
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

    int plugin_main() 
    {
        // run scanner
        if (use_run_options)
            run_func(**run_options);
        else
            run_func();
            
        return 0;
    }

    plugin_deconstructor 
    {
    }
}

#endif
