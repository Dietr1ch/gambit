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
///  *********************************************

#ifdef WITH_MPI
#include "gambit/Utils/begin_ignore_warnings_mpi.hpp"
#include "mpi.h"
#include "gambit/Utils/end_ignore_warnings.hpp"
#endif

#include "gambit/ScannerBit/python_scanner.hpp"
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

    /*!
     * Instance of python scanner
     */
    py::object instance;

    /*!
     * Yaml file options
     */
    py::object yaml;

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

        ::Gambit::Scanner::Plugins::ScannerPyPlugin::pythonPluginData() = &__gambit_plugin_namespace__::myData;

        // get plugin name
        std::string plugin_name = get_inifile_value<std::string>("plugin");

        // get yaml as dict
        yaml = yaml_to_dict(get_inifile_node());

        // log a warning if both 'init' and 'run' options are missing
        if (!yaml.contains("init") && !yaml.contains("run"))
        {
            scan_warning().raise(LOCAL_INFO, 
                "Neither an 'init' nor a 'run' section was found in "
                "the YAML options for the scanner " + plugin_name + ", "
                "so no options from the YAML file will be forwarded "
                "to the plugin."
            );
        }

        // get kwargs (if present)
        py::kwargs init_kwargs;
        if (yaml.contains("init"))
        {
            init_kwargs = py::dict(yaml["init"]);
        }

        // make instance of plugin
        Gambit::Scanner::Plugins::PythonScanner scanner(plugin_name);
        instance = scanner.plugin(**init_kwargs);
    }

    int plugin_main()
    {
        // get kwargs (if present)
        py::kwargs run_kwargs;
        if (yaml.contains("run"))
        {
            run_kwargs = py::dict(yaml["run"]);
        }
        // run scanner
        instance.attr("run")(**run_kwargs);
        return 0;
    }

    plugin_deconstructor
    {
        if (guard != nullptr)
        {
            delete guard;
        }
    }
}
