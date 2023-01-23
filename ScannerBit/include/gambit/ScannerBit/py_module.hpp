#ifdef __GAMBIT_CPP__

#include <unordered_map>
#include <pybind11/embed.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include "gambit/ScannerBit/factory_defs.hpp"
#include "gambit/ScannerBit/plugin_defs.hpp"
#include "gambit/ScannerBit/scanner_utils.hpp"

#define SCAN_PLUGIN_GET_INIFILE_VALUE                                                                   \
[](const std::string &val, py::kwargs args) -> py::object                                               \
{                                                                                                       \
    if (args.contains("default"))                                                                       \
    {                                                                                                   \
        if (is_type<py::int_>(args, "dtype"))                                                           \
            return py::cast(get_inifile_value<int>(val, args["default"].cast<int>()));                  \
        else if (is_type<py::str>(args, "dtype"))                                                       \
            return py::cast(get_inifile_value<std::string>(val, args["default"].cast<std::string>()));  \
        else if (is_type<py::list>(args, "dtype"))                                                      \
        {                                                                                               \
            py::list def = args["default"].cast<py::list>();                                            \
            py::list list;                                                                              \
                                                                                                        \
            if (is_type<py::int_>(args, "etype"))                                                       \
            {                                                                                           \
                std::vector<int> defv;                                                                  \
                for (auto &&l : def)                                                                    \
                    defv.push_back(l.cast<int>());                                                      \
                                                                                                        \
                std::vector<int> ret = get_inifile_value<std::vector<int>>(val, defv);                  \
                                                                                                        \
                for (auto &&r : ret)                                                                    \
                    list.append(r);                                                                     \
            }                                                                                           \
            else if (is_type<py::str>(args, "etype"))                                                   \
            {                                                                                           \
                std::vector<std::string> defv;                                                          \
                for (auto &&l : def)                                                                    \
                    defv.push_back(l.cast<std::string>());                                              \
                                                                                                        \
                std::vector<std::string> ret = get_inifile_value<std::vector<std::string>>(val, defv);  \
                                                                                                        \
                for (auto &&r : ret)                                                                    \
                    list.append(r);                                                                     \
            }                                                                                           \
            else                                                                                        \
            {                                                                                           \
                std::vector<double> defv;                                                               \
                for (auto &&l : def)                                                                    \
                    defv.push_back(l.cast<double>());                                                   \
                                                                                                        \
                std::vector<double> ret = get_inifile_value<std::vector<double>>(val, defv);            \
                                                                                                        \
                for (auto &&r : ret)                                                                    \
                    list.append(r);                                                                     \
            }                                                                                           \
                                                                                                        \
            return list;                                                                                \
        }                                                                                               \
        else                                                                                            \
            return py::cast(get_inifile_value<double>(val, args["default"].cast<double>()));            \
    }                                                                                                   \
    else                                                                                                \
    {                                                                                                   \
        if (is_type<py::int_>(args, "dtype"))                                                           \
            return py::cast(get_inifile_value<int>(val));                                               \
        else if (is_type<py::str>(args, "dtype"))                                                       \
            return py::cast(get_inifile_value<std::string>(val));                                       \
        else if (is_type<py::list>(args, "dtype"))                                                      \
        {                                                                                               \
            py::list list;                                                                              \
                                                                                                        \
            if (is_type<py::int_>(args, "etype"))                                                       \
            {                                                                                           \
                std::vector<int> ret = get_inifile_value<std::vector<int>>(val);                        \
                                                                                                        \
                for (auto &&r : ret)                                                                    \
                    list.append(r);                                                                     \
            }                                                                                           \
            else if (is_type<py::str>(args, "etype"))                                                   \
            {                                                                                           \
                std::vector<std::string> ret = get_inifile_value<std::vector<std::string>>(val);        \
                                                                                                        \
                for (auto &&r : ret)                                                                    \
                    list.append(r);                                                                     \
            }                                                                                           \
            else                                                                                        \
            {                                                                                           \
                std::vector<double> ret = get_inifile_value<std::vector<double>>(val);                  \
                                                                                                        \
                for (auto &&r : ret)                                                                    \
                    list.append(r);                                                                     \
            }                                                                                           \
                                                                                                        \
            return list;                                                                                \
        }                                                                                               \
        else                                                                                            \
            return py::cast(get_inifile_value<double>(val));                                            \
    }                                                                                                   \
}                                                                                                       \

namespace py = pybind11;

typedef std::unordered_map<std::string, double> map_doub_type_;
typedef std::vector<std::string> vec_str_type_;

namespace Gambit
{
    
    namespace Scanner 
    {
        
        namespace Plugins
        {
            
            namespace Utils
            {
                
                inline std::string pytype(py::handle o)
                {
                    return o.attr("__class__").attr("__name__").cast<std::string>();
                }
                
                template<typename T>
                bool is_type(py::kwargs args, const std::string &type = "dtype", bool def_type = false)
                {
                    if (args.contains(type.c_str())) 
                    {
                        auto arg = args[type.c_str()];
                        
                        if (pytype(arg) == "type" ? py::handle(arg).is(T().get_type()) : 
                            pytype(arg) == "str" || pytype(arg) == "unicode" ? arg.cast<std::string>() == pytype(T()) : false)
                            return true;
                        else
                            return false;
                    }
                    else
                        return def_type;
                }
            }
            
            namespace ScannerPyPlugin
            {
                
                __attribute__ ((visibility ("default"))) pluginData *&pythonPluginData()
                {
                    static pluginData *data = nullptr;
                    
                    return data;
                }
                
                template <typename T>
                inline T get_inifile_value(const std::string &in)
                {
                    if (!pythonPluginData()->node[in])
                    {
                        scan_err << "Missing iniFile entry \""<< in << "\" needed by a gambit plugin:  \n"
                                << pythonPluginData()->print() << scan_end;
                        return T();
                    }

                    return pythonPluginData()->node[in].as<T>();
                }
                
                template <typename T>
                inline T get_inifile_value(std::string in, T defaults)
                {
                    if (!pythonPluginData()->node[in])
                    {
                        return defaults;
                    }

                    return pythonPluginData()->node[in].as<T>();
                } 
                
                template <typename T>
                inline T &get_input_value(int i)
                {
                    return *static_cast<T*>(pythonPluginData()->inputData[i]);
                }
                
                inline Gambit::Scanner::printer_interface &get_printer()
                {
                    return *pythonPluginData()->printer;
                }

                inline Gambit::Scanner::prior_interface &get_prior()
                {
                    return *pythonPluginData()->prior;
                }
                
                unsigned int &get_dimension() {return get_input_value<unsigned int>(0);}
                
                //inline std::shared_ptr<Gambit::Scanner::Function_Base<double (std::unordered_map<std::string, double> &)>>
                Gambit::Scanner::like_ptr
                get_purpose(const std::string &purpose)
                {
                    //typedef Gambit::Scanner::Function_Base<double (std::unordered_map<std::string, double> &)> s_func;
                    //typedef std::shared_ptr<s_func> s_ptr;
                    
                    void *ptr = (get_input_value<Factory_Base>(1))(purpose);
                    static_cast <Function_Base<void(void)>*>(ptr)->setPurpose(purpose);
                    static_cast <Function_Base<void(void)>*>(ptr)->setPrinter(get_printer().get_stream());
                    static_cast <Function_Base<void(void)>*>(ptr)->setPrior(&get_prior());
                    assign_aux_numbers(purpose, "pointID", "MPIrank");

                    //return  s_ptr(static_cast<s_func *>(ptr));
                    return ptr;
                }
        
            }
            
            namespace ObjPyPlugin
            {
                
                __attribute__ ((visibility ("default"))) pluginData *&pythonPluginData()
                {
                    static pluginData *data = nullptr;
                    
                    return data;
                }
                
                __attribute__ ((visibility ("default"))) double run(py::object func, map_doub_type_ &map)
                {
                    return func(&map).cast<double>();
                }
                
                template <typename T>
                inline T get_inifile_value(const std::string &in)
                {
                    if (!pythonPluginData()->node[in])
                    {
                        scan_err << "Missing iniFile entry \""<< in << "\" needed by a gambit plugin:  \n"
                                << pythonPluginData()->print() << scan_end;
                        return T();
                    }

                    return pythonPluginData()->node[in].as<T>();
                }
                
                template <typename T>
                inline T get_inifile_value(std::string in, T defaults)
                {
                    if (!pythonPluginData()->node[in])
                    {
                        return defaults;
                    }

                    return pythonPluginData()->node[in].as<T>();
                } 
                
                template <typename T>
                inline T &get_input_value(int i)
                {
                    return *static_cast<T*>(pythonPluginData()->inputData[i]);
                }
                
                inline Gambit::Scanner::printer_interface &get_printer()
                {
                    return *pythonPluginData()->printer;
                }

                inline Gambit::Scanner::prior_interface &get_prior()
                {
                    return *pythonPluginData()->prior;
                }
                
                using Gambit::Printers::get_point_id;

                std::vector<std::string> &get_keys() {return get_input_value<std::vector<std::string>>(0);} 
                
                void set_dimension(unsigned int val) {get_input_value<unsigned int>(1) = val;}
                
                void print_parameters(std::unordered_map<std::string, double> &key_map)
                {
                    using Gambit::Printers::get_main_param_id;
                    Gambit::Scanner::printer *printer = get_printer().get_stream();

                    for (auto it = get_keys().begin(), end = get_keys().end(); it != end; ++it)
                    {
                        auto temp = get_main_param_id(*it);
                        printer->print(key_map[*it], *it, temp, printer->getRank(), get_point_id());
                    }
                }
        
            }

        }
        
    }
    
}

PYBIND11_MAKE_OPAQUE(map_doub_type_);
PYBIND11_MAKE_OPAQUE(vec_str_type_);

PYBIND11_EMBEDDED_MODULE(scannerbit, m) 
{
    //typedef std::shared_ptr<Gambit::Scanner::Function_Base<double (std::unordered_map<std::string, double> &)>> s_ptr;
    //typedef Gambit::Scanner::Function_Base<double (std::unordered_map<std::string, double> &)> s_func;
    
    py::bind_map<map_doub_type_, std::shared_ptr<map_doub_type_>>(m, "std_unordered_map_double");
    py::bind_vector<vec_str_type_, std::shared_ptr<vec_str_type_>>(m, "std_vector_string");
    
    py::class_<Gambit::Printers::BaseBasePrinter, std::unique_ptr<Gambit::Printers::BaseBasePrinter, py::nodelete>>(m, "printer")
    .def("print", [](Gambit::Printers::BaseBasePrinter &self, double in, const std::string& label,
                   const int vertexID, const unsigned int rank,
                   const unsigned long pointID)
    {
        self.print(in, label, vertexID, rank, pointID);
    })
    .def("print", [](Gambit::Printers::BaseBasePrinter &self, double in, const std::string& label,
                   const unsigned int rank,
                   const unsigned long pointID)
    {
        self.print(in, label, rank, pointID);
    });
    
    py::class_<Gambit::Priors::BasePrior, std::unique_ptr<Gambit::Priors::BasePrior, py::nodelete>>(m, "prior")
    .def("transform", [](Gambit::Priors::BasePrior &self, Gambit::Scanner::hyper_cube<double> vec, std::unordered_map<std::string, double> &map)
    {
        self.transform(vec, map);
    })
    .def("inverse_transform", [](Gambit::Priors::BasePrior &self, std::unordered_map<std::string, double> &map, Gambit::Scanner::hyper_cube<double> vec)
    {
        self.inverse_transform(map, vec);
    })
    .def("getShownParameters", [](Gambit::Priors::BasePrior &self)
    {
        return self.getShownParameters();
    })
    .def("getParameters", [](Gambit::Priors::BasePrior &self)
    {
        return self.getParameters();
    });
    
    //virtual void new_stream(const std::string&, const Options&) = 0;
    //virtual void new_reader(const std::string&, const Options&) = 0;
    //virtual void create_resume_reader() = 0;
    //virtual BaseBaseReader* get_reader(const std::string&) = 0;
    //virtual bool reader_exists(const std::string&) = 0;
    //virtual void delete_reader(const std::string&) = 0;
    py::class_<Gambit::Scanner::printer_interface, std::unique_ptr<Gambit::Scanner::printer_interface, py::nodelete>>(m, "printer_interface")
    .def("resume_mode", [&](Gambit::Scanner::printer_interface &self)
    {
        return self.resume_mode();
    })
    .def("set_resume_mode", [&](Gambit::Scanner::printer_interface &self, bool rflag){
        return self.set_resume_mode(rflag);
    })
    .def("get_stream", [&](Gambit::Scanner::printer_interface &self)
    {
        return self.get_stream("");
    })
    .def("get_stream", [&](Gambit::Scanner::printer_interface &self, const std::string &in)
    {
        return self.get_stream(in);
    })
    .def("delete_stream", [&](Gambit::Scanner::printer_interface &self)
    {
        self.delete_stream("");
    })
    .def("delete_stream", [&](Gambit::Scanner::printer_interface &self, const std::string &in)
    {
        self.delete_stream(in);
    })
    .def("finalise", [&](Gambit::Scanner::printer_interface &self)
    {
        self.finalise(false);
    })
    .def("finalise", [&](Gambit::Scanner::printer_interface &self, bool abnormal)
    {
        self.finalise(abnormal);
    });
    
    py::class_<Gambit::Scanner::like_ptr>(m, "like_ptr")
    .def("__call__", [](Gambit::Scanner::like_ptr &self, Gambit::Scanner::hyper_cube<double> vec)
    {
        return self(vec);
    })
    .def("getPtID", [&](Gambit::Scanner::like_ptr &self)
    {
        return self->getPtID();
    })
    .def("getPrinter", [&](Gambit::Scanner::like_ptr &self)
    {
        return &self->getPrinter();
    })
    .def("getPrior", [&](Gambit::Scanner::like_ptr &self)
    {
        return &self->getPrior();
    });
}

PYBIND11_EMBEDDED_MODULE(scanner_pyplugin, m) 
{
    using namespace Gambit::Scanner::Plugins::ScannerPyPlugin;
    using namespace Gambit::Scanner::Plugins::Utils;
    
    m.import("scannerbit");
    
    m.def("get_printer", []()
    {
        return &Gambit::Scanner::Plugins::ScannerPyPlugin::get_printer();
    }, "", py::return_value_policy::reference);
    
    m.def("get_prior", []()
    {
        return &Gambit::Scanner::Plugins::ScannerPyPlugin::get_prior();
    }, "", py::return_value_policy::reference);
    
    m.def("get_inifile_value", SCAN_PLUGIN_GET_INIFILE_VALUE);
    m.def("get_dimension", get_dimension);
    m.def("get_purpose", get_purpose);
}

PYBIND11_EMBEDDED_MODULE(objective_pyplugin, m) 
{
    using namespace ::Gambit::Scanner::Plugins::ObjPyPlugin;
    using namespace ::Gambit::Scanner::Plugins::Utils;

    m.import("scannerbit");
    
    m.def("get_inifile_value", SCAN_PLUGIN_GET_INIFILE_VALUE);
    m.def("get_keys", get_keys);
    m.def("set_dimension", set_dimension);
    m.def("print_parameters", print_parameters);
}

#endif
