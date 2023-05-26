/*! \brief Represent python scanner
 *
 * Method for loading scanners, building GAMBIT style doc strings,
 * and checking which python scanners are available.
 */


#ifndef PYTHON_SCANNER_HPP
#define PYTHON_SCANNER_HPP

#include <vector>
#include <string>
#include <iostream>

#include "gambit/Utils/table_formatter.hpp"
#include "gambit/Utils/begin_ignore_warnings_pybind11.hpp"
#include <pybind11/embed.h>
#include "gambit/Utils/end_ignore_warnings.hpp"


namespace py = pybind11;

namespace Gambit {
namespace Scanner {
namespace Plugins {

class PythonScanner {
 public:
    std::string plugin_name;
    std::string version;
    std::string loc;
    bool implements_abc;

    /*! 
     * Doc strings from scanner
     */
    std::string class_doc;
    std::string init_doc;
    std::string run_doc;

    /*! 
     * Python scanner object
     */
    py::object plugin;

    py::scoped_interpreter *guard = nullptr;

    explicit PythonScanner(std::string plugin_name) : plugin_name(plugin_name) {
        /*! 
         * Retrieve and inspect a python scanner
         */

        // add python dirs to path
        std::string path = GAMBIT_DIR "/ScannerBit/src/scanners/python";
        py::list(py::module::import("sys").attr("path")).append(py::cast(path));

        try {
            guard = new py::scoped_interpreter();
        } catch(std::exception &) {
            guard = nullptr;
        }

        // import plugins
        py::module plugins_module = py::module::import("plugins");
        py::dict plugins = plugins_module.attr("plugins");
        plugin = plugins[plugin_name.c_str()];

        // inspect plugin
        py::module inspect_module = py::module::import("inspect");
        loc = inspect_module.attr("getsourcefile")(plugin).cast<std::string>();
        class_doc = inspect_module.attr("getdoc")(plugin).cast<std::string>();
        init_doc = inspect_module.attr("getdoc")(plugin.attr("__init__")).cast<std::string>();
        run_doc = inspect_module.attr("getdoc")(plugin.attr("run")).cast<std::string>();
        version = plugin.attr("version").cast<std::string>();

        // implements the abstract base class
        py::module builtins = py::module::import("builtins");
        py::object scanner_abc = plugins_module.attr("Scanner");
        implements_abc = (inspect_module.attr("isclass")(plugin).cast<bool>() &&
                          !inspect_module.attr("isabstract")(plugin).cast<bool>() &&
                          builtins.attr("issubclass")(plugin, scanner_abc).cast<bool>());
    }

    ~PythonScanner() {
        if (guard != nullptr) {
            delete guard;
        }
    }

    bool passes_checks() const {
        return implements_abc;
    }

    std::string status() const {
        if (!implements_abc) {
            return "does not implement ABC";
        }
        return "implements ABC";
    }

    std::string doc() const {
        /*! 
         * Generate GAMBIT style doc string
         */
        std::stringstream doc;

        doc << std::endl
            << "\x1b[01m\x1b[04mGENERAL PLUGIN INFO\x1b[0m" << std::endl
            << "name:     " << plugin_name << std::endl
            << "type:     python scanner" << std::endl
            << "version:  " << version << std::endl;

        if (passes_checks()) {
            doc << "status:   \x1b[32;01m" << status() << "\x1b[0m" << std::endl;
        } else {
            doc << "status:   \x1b[32;01m" << status() << "\x1b[0m" << std::endl;
        }

        doc << "location: " << loc << std::endl << std::endl
            << "\x1b[01m\x1b[04mDESCRIPTION\x1b[0m" << std::endl
            << class_doc << std::endl << std::endl
            << "\x1b[01m\x1b[04mINIT DESCRIPTION\x1b[0m" << std::endl
            << init_doc << std::endl << std::endl
            << "\x1b[01m\x1b[04mRUN DESCRIPTION\x1b[0m" << std::endl
            << run_doc << std::endl << std::endl;

        return doc.str();
    }

    void add_table_row(table_formatter &table) const {
        /*! 
         * Append row about scanner to GAMBIT style table
         */
        table << plugin_name << version;
        if (passes_checks()) {
            table.green() << status();
        } else {
            table.red() << status();
        }
    }
};

std::vector<std::string> python_scanner_names() {
    /*! 
     * Names of recognized Python scanners
     */

    // add python dirs to path
    std::string path = GAMBIT_DIR "/ScannerBit/src/scanners/python";
    py::list(py::module::import("sys").attr("path")).append(py::cast(path));

    // python interpreter guard
    py::scoped_interpreter *guard;

    try {
        guard = new py::scoped_interpreter();
    } catch(std::exception &) {
        guard = nullptr;
    }

    // fetch plugin names
    py::module plugins_module = py::module::import("plugins");
    py::list plugin_names = plugins_module.attr("plugins").attr("keys")();

    // cast to c type
    std::vector<std::string> c_plugin_names;

    for (auto n : plugin_names) {
        c_plugin_names.push_back(n.cast<std::string>());
    }

    if (guard != nullptr) {
        delete guard;
    }

    return c_plugin_names;
}

}  // end namespace Plugins
}  // end namespace Scanner
}  // end namespace Gambit

#endif  // PYTHON_SCANNER_HPP
