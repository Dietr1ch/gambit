/*! \brief Use a python scanner
 *
 * Make an instance of python scanner and define method to run it
 */

#ifdef WITH_MPI
#include "gambit/Utils/begin_ignore_warnings_mpi.hpp"
#include "mpi.h"
#include "gambit/Utils/end_ignore_warnings.hpp"
#endif

#include "gambit/ScannerBit/python_scanner.hpp"
#include "gambit/ScannerBit/scanner_plugin.hpp"

namespace py = pybind11;

namespace Gambit {
namespace Scanner {
namespace Plugins {
namespace ScannerPyPlugin {
pluginData *&pythonPluginData();
}  // end namespace ScannerPyPlugin
}  // end namespace Plugins
}  // end namespace Scanner
}  // end namespace Gambit

scanner_plugin(python, version(1, 0, 0)) {
    reqd_headers("PYTHONLIBS");
    reqd_headers("pybind11");

    /*!
     * Instance of python scanner
     */
    py::object instance;

    py::scoped_interpreter *guard = nullptr;

    plugin_constructor {
        try {
            guard = new py::scoped_interpreter();
        } catch(std::exception &) {
            guard = nullptr;
        }

        ::Gambit::Scanner::Plugins::ScannerPyPlugin::pythonPluginData() = &__gambit_plugin_namespace__::myData;

        // get kwargs
        py::dict init_kwargs = py::dict();  // get_inifile_value<py::object>("init", py::dict());

        // make instance of plugin
        std::string plugin_name = get_inifile_value<std::string>("plugin");
        Gambit::Scanner::Plugins::PythonScanner scanner(plugin_name);
        instance = scanner.plugin(**init_kwargs);
    }

    int plugin_main() {
        // get kwargs
        py::dict run_kwargs = py::dict();  // get_inifile_value<py::object>("run", py::dict());
        // run scanner
        instance.attr("run")(**run_kwargs);
        return 0;
    }

    plugin_deconstructor {
        if (guard != nullptr) {
            delete guard;
        }
    }
}
