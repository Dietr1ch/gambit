//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  ??????
///
///  *********************************************
///
///  Authors:
///
///  \author ????
///  \date ????
///
///  *********************************************

#pragma once

#include "gambit/cmake/cmake_variables.hpp"

#ifdef HAVE_PYBIND11

#include <unordered_map>
#include "gambit/Utils/begin_ignore_warnings_pybind11.hpp"
#include "gambit/Utils/begin_ignore_warnings_eigen.hpp"
#include <pybind11/embed.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include "gambit/Utils/end_ignore_warnings.hpp"

#include "gambit/ScannerBit/factory_defs.hpp"
#include "gambit/ScannerBit/plugin_defs.hpp"
#include "gambit/ScannerBit/scanner_utils.hpp"

#define __SCAN_PLUGIN_GET_INIFILE_VALUE__(GET_INIFILE_VALUE)                                            \
{                                                                                                       \
    if (args.contains("default"))                                                                       \
    {                                                                                                   \
        if (is_type<py::int_>(args, "dtype"))                                                           \
            return py::cast(GET_INIFILE_VALUE<int>(val, args["default"].cast<int>()));                  \
        else if (is_type<py::str>(args, "dtype"))                                                       \
            return py::cast(GET_INIFILE_VALUE<std::string>(val, args["default"].cast<std::string>()));  \
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
                std::vector<int> ret = GET_INIFILE_VALUE<std::vector<int>>(val, defv);                  \
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
                std::vector<std::string> ret = GET_INIFILE_VALUE<std::vector<std::string>>(val, defv);  \
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
                std::vector<double> ret = GET_INIFILE_VALUE<std::vector<double>>(val, defv);            \
                                                                                                        \
                for (auto &&r : ret)                                                                    \
                    list.append(r);                                                                     \
            }                                                                                           \
                                                                                                        \
            return list;                                                                                \
        }                                                                                               \
        else                                                                                            \
            return py::cast(GET_INIFILE_VALUE<double>(val, args["default"].cast<double>()));            \
    }                                                                                                   \
    else                                                                                                \
    {                                                                                                   \
        if (is_type<py::int_>(args, "dtype"))                                                           \
            return py::cast(GET_INIFILE_VALUE<int>(val));                                               \
        else if (is_type<py::bool_>(args, "dtype"))                                                     \
            return py::cast(GET_INIFILE_VALUE<bool>(val));                                              \
        else if (is_type<py::str>(args, "dtype"))                                                       \
            return py::cast(GET_INIFILE_VALUE<std::string>(val));                                       \
        else if (is_type<py::list>(args, "dtype"))                                                      \
        {                                                                                               \
            py::list list;                                                                              \
                                                                                                        \
            if (is_type<py::int_>(args, "etype"))                                                       \
            {                                                                                           \
                std::vector<int> ret = GET_INIFILE_VALUE<std::vector<int>>(val);                        \
                                                                                                        \
                for (auto &&r : ret)                                                                    \
                    list.append(r);                                                                     \
            }                                                                                           \
            else if (is_type<py::bool_>(args, "etype"))                                                 \
            {                                                                                           \
                std::vector<bool> ret = GET_INIFILE_VALUE<std::vector<bool>>(val);                      \
                                                                                                        \
                for (auto &&r : ret)                                                                    \
                    list.append(bool(r));                                                               \
            }                                                                                           \
            else if (is_type<py::str>(args, "etype"))                                                   \
            {                                                                                           \
                std::vector<std::string> ret = GET_INIFILE_VALUE<std::vector<std::string>>(val);        \
                                                                                                        \
                for (auto &&r : ret)                                                                    \
                    list.append(r);                                                                     \
            }                                                                                           \
            else                                                                                        \
            {                                                                                           \
                std::vector<double> ret = GET_INIFILE_VALUE<std::vector<double>>(val);                  \
                                                                                                        \
                for (auto &&r : ret)                                                                    \
                    list.append(r);                                                                     \
            }                                                                                           \
                                                                                                        \
            return list;                                                                                \
        }                                                                                               \
        else                                                                                            \
            return py::cast(GET_INIFILE_VALUE<double>(val));                                            \
    }                                                                                                   \
}                                                                                                       \

#define SCAN_PLUGIN_GET_INIFILE_VALUE_FUNC                  \
[](const std::string &val, py::kwargs args) -> py::object   \
__SCAN_PLUGIN_GET_INIFILE_VALUE__(get_inifile_value)        \

#define SCAN_PLUGIN_GET_INIFILE_VALUE_MEM_FUNC                                  \
[](plugin_data &self, const std::string &val, py::kwargs args) -> py::object    \
__SCAN_PLUGIN_GET_INIFILE_VALUE__(self.get_inifile_value)                       \

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
                
                inline py::object yaml_to_dict(const YAML::Node &node)
                {
                    if (node.IsNull())
                    {
                        return py::dict();
                    }
                    else if (node.IsMap())
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
                
                class like_physical : public std::enable_shared_from_this<like_physical>
                {
                private:
                    typedef std::shared_ptr<Gambit::Scanner::Function_Base<double (std::unordered_map<std::string, double> &)>> s_ptr;
                    typedef Gambit::Scanner::Function_Base<double (std::unordered_map<std::string, double> &)> s_func;
                    s_ptr ptr;
                    
                public:
                    like_physical(s_func &s) : ptr(s.shared_from_this()) {}
                    
                    Gambit::Scanner::like_ptr &get(){return static_cast<Gambit::Scanner::like_ptr&>(ptr);}
                };
                
                class like_prior_physical : public std::enable_shared_from_this<like_prior_physical>
                {
                private:
                    typedef std::shared_ptr<Gambit::Scanner::Function_Base<double (std::unordered_map<std::string, double> &)>> s_ptr;
                    typedef Gambit::Scanner::Function_Base<double (std::unordered_map<std::string, double> &)> s_func;
                    s_ptr ptr;
                    
                public:
                    like_prior_physical(s_func &s) : ptr(s.shared_from_this()) {}
                    
                    Gambit::Scanner::like_ptr &get(){return static_cast<Gambit::Scanner::like_ptr&>(ptr);}
                };
                
                class prior_physical : public std::enable_shared_from_this<prior_physical>
                {
                private:
                    typedef std::shared_ptr<Gambit::Scanner::Function_Base<double (std::unordered_map<std::string, double> &)>> s_ptr;
                    typedef Gambit::Scanner::Function_Base<double (std::unordered_map<std::string, double> &)> s_func;
                    s_ptr ptr;
                    
                public:
                    prior_physical(s_func &s) : ptr(s.shared_from_this()) {}
                    
                    Gambit::Scanner::like_ptr &get(){return static_cast<Gambit::Scanner::like_ptr&>(ptr);}
                };
                
            }
            
            namespace ScannerPyPlugin
            {
                
                EXPORT_SYMBOLS pluginData*& pythonPluginData()
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
                inline T get_inifile_value(const std::string &in, const T &defaults)
                {
                    if (!pythonPluginData()->node[in])
                    {
                        return defaults;
                    }

                    return pythonPluginData()->node[in].as<T>();
                }
                
                inline YAML::Node get_inifile_node(const std::string &in)                                                      
                {
                    return pythonPluginData()->node[in];
                }

                inline YAML::Node get_inifile_node()                                                      
                {
                    return pythonPluginData()->node;
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
                
                inline std::shared_ptr<Gambit::Scanner::Function_Base<double (std::unordered_map<std::string, double> &)>>
                get_purpose(const std::string &purpose)
                {
                    //typedef Gambit::Scanner::Function_Base<double (std::unordered_map<std::string, double> &)> s_func;
                    //typedef std::shared_ptr<s_func> s_ptr;
                    
                    void *ptr = (get_input_value<Factory_Base>(1))(purpose);
                    static_cast <Function_Base<void(void)>*>(ptr)->setPurpose(purpose);
                    static_cast <Function_Base<void(void)>*>(ptr)->setPrinter(get_printer().get_stream());
                    static_cast <Function_Base<void(void)>*>(ptr)->setPrior(&get_prior());
                    assign_aux_numbers(purpose, "pointID", "MPIrank");

                    return Gambit::Scanner::like_ptr(ptr);
                }
        
                class scanner_base
                {
                private:
                    typedef std::shared_ptr<Gambit::Scanner::Function_Base<double (std::unordered_map<std::string, double> &)>> s_ptr;
                    typedef Gambit::Scanner::Function_Base<double (std::unordered_map<std::string, double> &)> s_func;
                    typedef Gambit::Scanner::Plugins::Utils::like_physical s_phys_func;
                    typedef Gambit::Scanner::Plugins::Utils::like_prior_physical s_phys_pr_func;
                    typedef Gambit::Scanner::Plugins::Utils::prior_physical s_pr_func;
                    /*
                    py::dict run_opts;
                    std::shared_ptr<s_func> like;
                    std::shared_ptr<s_func> like_hyper_cube;
                    std::shared_ptr<s_phys_func> like_physical;
                    std::shared_ptr<s_phys_pr_func> like_prior_physical;
                    */
                template<typename T>
                py::list to_list(const std::vector<T> &vec)
                {
                    py::list l;
                    for (auto &&elem : vec)
                        l.append(py::cast(elem));
                    
                    return l;
                }
                    
                public:
                    /*scanner_base()
                    {
                        py::dict opts = ::Gambit::Scanner::Plugins::Utils::yaml_to_dict(get_inifile_node());
                        
                        if(opts.contains("run"))
                            run_opts = opts["run"];
                        
                        if(!opts.contains("like"))
                            scan_err << "Inifile missing \"like\" entry." << scan_end;
                        
                        like = get_purpose(opts["like"].cast<std::string>());
                        like_hyper_cube = like;
                        like_physical = std::shared_ptr<s_phys_func>(new s_phys_func(*like.get()));
                        like_prior_physical = std::shared_ptr<s_phys_pr_func>(new s_phys_pr_func(*like.get()));
                    }*/

                    //virtual int run() = 0;
                    int run()
                    {
                        scan_err << "\"run()\" method not defined in python scanner plugin." << scan_end;
                        return 1;
                    }
                    
                    
                    Gambit::Scanner::vector<double> transform_to_vec(Gambit::Scanner::hyper_cube<double> unit)
                    {
                        //auto &map = getLike()->getMap();
                        static std::unordered_map<std::string, double> map;
                        get_prior().transform(unit, map);
                        auto params = get_prior().getShownParameters();
                        Gambit::Scanner::vector<double> vec(params.size());
                        
                        for (size_t i = 0, end = params.size(); i < end; ++i)
                            vec[i] = map[params[i]];
                        
                        return vec;
                    }
                    
                    void transform_inplace(Gambit::Scanner::hyper_cube<double> unit, std::unordered_map<std::string, double> &physical)
                    {
                        get_prior().transform(unit, physical);
                    }
                    
                    void transform_inplace_dict(Gambit::Scanner::hyper_cube<double> unit, py::dict physical)
                    {
                        //auto &map = getLike()->getMap();
                        static std::unordered_map<std::string, double> map;
                        get_prior().transform(unit, map);
                        for (auto &&m : map)
                            physical[py::cast(m.first)] = py::cast(m.second);
                    }
                    
                    py::dict transform(Gambit::Scanner::hyper_cube<double> unit)
                    {
                        py::dict physical;
                        //auto &map = getLike()->getMap();
                        static std::unordered_map<std::string, double> map;
                        get_prior().transform(unit, map);
                        
                        for (auto &&m : map)
                            physical[py::cast(m.first)] = py::cast(m.second);
                        
                        return physical;
                    }
                    
                    void inverse_transform_inplace(std::unordered_map<std::string, double> &physical, Gambit::Scanner::hyper_cube<double> unit)
                    {
                        get_prior().inverse_transform(physical, unit);
                    }
                    
                    void inverse_transform_inplace_dict(py::dict physical, Gambit::Scanner::hyper_cube<double> unit)
                    {
                        std::unordered_map<std::string, double> map;
                        for (auto &&m : physical)
                            map[m.first.cast<std::string>()] = m.second.cast<double>();
                        
                        get_prior().inverse_transform(map, unit);
                    }
                    
                    Gambit::Scanner::vector<double> inverse_transform(std::unordered_map<std::string, double> &physical)
                    {
                        Gambit::Scanner::vector<double> unit(get_prior().size());
                        get_prior().inverse_transform(physical, unit);
                        
                        return unit;
                    }
                    
                    Gambit::Scanner::vector<double> inverse_transform_dict(py::dict physical)
                    {
                        Gambit::Scanner::vector<double> unit(get_prior().size());
                        std::unordered_map<std::string, double> map;
                        for (auto &&m : physical)
                            map[m.first.cast<std::string>()] = m.second.cast<double>();
                        
                        get_prior().inverse_transform(map, unit);
                        
                        return unit;
                    }
                    
                #ifdef WITH_MPI
                    bool with_mpi() {return true;}
                    int rank()
                    {
                        int rank;
                        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
                        
                        return rank;
                    }
                    
                    int numtasks()
                    {
                        int numtasks;
                        MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
                        
                        return numtasks;
                    }
                #else
                    bool with_mpi() {return true;}
                    int rank() {return 0;}
                    int numtasks() {return 1;}
                #endif
                
                    int getRank()
                    {
                        static int my_rank = rank();
                        
                        return my_rank;
                    }
                    
                    int getNumTasks()
                    {
                        static int tasks = numtasks();
                        
                        return tasks;
                    }
                    
                    void print(double val, const std::string &name)
                    {
                        get_printer().get_stream()->print(val, name, getRank(), getLike()->getPtID());
                    }
                    
                    void print_full(double val, const std::string &name, const unsigned int rank, const unsigned long pointID)
                    {
                        get_printer().get_stream()->print(val, name, rank, pointID);
                    }
                    
                    unsigned long point_id() 
                    {
                        return getLike()->getPtID();
                    }
                    
                    py::list &getParameterNames()
                    {
                        static py::list names = to_list<std::string>(get_prior().getShownParameters());
                        
                        return names;
                    }
                    
                    YAML::Node &getNode()
                    {
                        static YAML::Node node = get_inifile_node();
                        
                        return node;
                    }
                    
                    py::dict &getOpts()
                    {
                        static py::dict opts = ::Gambit::Scanner::Plugins::Utils::yaml_to_dict(getNode());
                        
                        return opts;
                    }
                    
                    py::dict &getRunOpts()
                    {
                        //static py::dict run_opts = ::Gambit::Scanner::Plugins::Utils::yaml_to_dict(getNode()["run"] ? getNode()["run"] : getNode());
                        static py::dict run_opts = ::Gambit::Scanner::Plugins::Utils::yaml_to_dict(getNode()["run"] ? getNode()["run"] : YAML::Node());
                        
                        return run_opts;
                    }
                    
                    py::dict &getInitOpts()
                    {
                        //static py::dict init_opts = ::Gambit::Scanner::Plugins::Utils::yaml_to_dict(getNode()["init"] ? getNode()["init"] : getNode());
                        static py::dict init_opts = ::Gambit::Scanner::Plugins::Utils::yaml_to_dict(getNode()["init"] ? getNode()["init"] : YAML::Node());
                        
                        return init_opts;
                    }
                    
                    void set_run_defaults(py::kwargs opts)
                    {
                        auto &runOpts = getRunOpts();
                        for (auto &&o : opts)
                            if (!runOpts.contains(o.first))
                                runOpts[o.first] = o.second;
                    }
                    
                    int &getDim()
                    {
                        static int dim = get_dimension();
                        
                        return dim;
                    }
                    
                    std::shared_ptr<s_func> getLike()
                    {
                        static std::shared_ptr<s_func> like = getNode()["like"] ? get_purpose(getNode()["like"].as<std::string>()) : nullptr;
                        
                        return like;
                    }
                    
                    std::shared_ptr<s_func> getLikeHyperCube()
                    {
                        static std::shared_ptr<s_func> like_hyper_cube = getLike();
                        
                        return like_hyper_cube;
                    }
                    
                    std::shared_ptr<s_phys_func> getLikePhysical()
                    {
                        static std::shared_ptr<s_phys_func> like_physical(getLike() == nullptr ? nullptr : new s_phys_func(*getLike().get()));
                        
                        return like_physical;
                    }
                    
                    std::shared_ptr<s_phys_pr_func> getLikePriorPhysical()
                    {
                        static std::shared_ptr<s_phys_pr_func> like_prior_physical(getLike() == nullptr ? nullptr : new s_phys_pr_func(*getLike().get()));
                        
                        return like_prior_physical;
                    }
                    
                    std::shared_ptr<s_pr_func> getPriorPhysical()
                    {
                        static std::shared_ptr<s_pr_func> prior_physical(getLike() == nullptr ? nullptr : new s_pr_func(*getLike().get()));
                        
                        return prior_physical;
                    }
                };
                
            }
            
            namespace ObjPyPlugin
            {
                
                EXPORT_SYMBOLS pluginData*& pythonPluginData()
                {
                    static pluginData *data = nullptr;
                    
                    return data;
                }
                
                __attribute__ ((visibility ("default"))) double run(py::object &func, map_doub_type_ &map)
                {
                    return func(&map).cast<double>();
                }
                
                __attribute__ ((visibility ("default"))) double run(py::object &func, map_doub_type_ &map, py::kwargs &opts)
                {
                    return func(&map, **opts).cast<double>();
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
                inline T get_inifile_value(const std::string &in, const T &defaults)
                {
                    if (!pythonPluginData()->node[in])
                    {
                        return defaults;
                    }

                    return pythonPluginData()->node[in].as<T>();
                }
                
                inline YAML::Node get_inifile_node(const std::string &in)                                                      
                {
                    return pythonPluginData()->node[in];
                }
                
                inline YAML::Node get_inifile_node()                                                      
                {
                    return pythonPluginData()->node;
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
    py::bind_map<map_doub_type_, std::shared_ptr<map_doub_type_>>(m, "std_unordered_map_double");
    py::bind_vector<vec_str_type_, std::shared_ptr<vec_str_type_>>(m, "std_vector_string");
    
#ifdef WITH_MPI
    m.attr("with_mpi") = py::bool_(true);
    m.def("rank", []()
    {
        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        
        return rank;
    });
    
     m.def("numtasks", []()
    {
        int numtasks;
        MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
        
        return numtasks;
    });
#else
    m.attr("with_mpi") = py::bool_(false);
    m.def("rank", [](){return 0;});
    m.def("numtasks", [](){return 1;});
#endif
    
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
    .def("transform", [](Gambit::Priors::BasePrior &self, Gambit::Scanner::hyper_cube<double> unit, std::unordered_map<std::string, double> &physical)
    {
        self.transform(unit, physical);
    })
    .def("transform", [](Gambit::Priors::BasePrior &self, Gambit::Scanner::hyper_cube<double> unit, py::dict physical)
    {
        static std::unordered_map<std::string, double> map;
        self.transform(unit, map);
        
        for (auto &&m : map)
            physical[py::cast(m.first)] = py::cast(m.second);
    })
    .def("transform", [](Gambit::Priors::BasePrior &self, Gambit::Scanner::hyper_cube<double> unit)
    {
        py::dict physical;
        static std::unordered_map<std::string, double> map;
        
        self.transform(unit, map);
        
        for (auto &&m : map)
            physical[py::cast(m.first)] = py::cast(m.second);
        
        return physical;
    })
    .def("inverse_transform", [](Gambit::Priors::BasePrior &self, std::unordered_map<std::string, double> &physical, Gambit::Scanner::hyper_cube<double> unit)
    {
        self.inverse_transform(physical, unit);
    })
    .def("inverse_transform", [](Gambit::Priors::BasePrior &self, py::dict physical, Gambit::Scanner::hyper_cube<double> unit)
    {
        std::unordered_map<std::string, double> map;
        for (auto &&d : physical)
            map[d.first.cast<std::string>()] = d.second.cast<double>();
        
        self.inverse_transform(map, unit);
    })
    .def("inverse_transform", [](Gambit::Priors::BasePrior &self, std::unordered_map<std::string, double> &physical)
    {
        Gambit::Scanner::vector<double> unit(self.size());
        self.inverse_transform(physical, unit);
        
        return unit;
    })
    .def("inverse_transform", [](Gambit::Priors::BasePrior &self, py::dict physical)
    {
        Gambit::Scanner::vector<double> unit(self.size());
        std::unordered_map<std::string, double> map;
        for (auto &&d : physical)
            map[d.first.cast<std::string>()] = d.second.cast<double>();
        
        self.inverse_transform(map, unit);
        
        return unit;
    })
    .def("getShownParameters", [](Gambit::Priors::BasePrior &self)
    {
        return self.getShownParameters();
    })
    .def("getParameters", [](Gambit::Priors::BasePrior &self)
    {
        return self.getParameters();
    })
    .def("log_prior_density", [](Gambit::Priors::BasePrior &self, std::unordered_map<std::string, double> &physical)
    {
        return self.log_prior_density(physical);
    });
    
    //Mmember functions not exported:
    //  virtual void new_stream(const std::string&, const Options&) = 0;
    //  virtual void new_reader(const std::string&, const Options&) = 0;
    //  virtual void create_resume_reader() = 0;
    //  virtual BaseBaseReader* get_reader(const std::string&) = 0;
    //  virtual bool reader_exists(const std::string&) = 0;
    //  virtual void delete_reader(const std::string&) = 0;
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
    
    typedef std::shared_ptr<Gambit::Scanner::Function_Base<double (std::unordered_map<std::string, double> &)>> s_ptr;
    typedef Gambit::Scanner::Function_Base<double (std::unordered_map<std::string, double> &)> s_func;
    using Gambit::Scanner::like_ptr;
    typedef Gambit::Scanner::Plugins::Utils::like_physical s_phys_func;
    typedef Gambit::Scanner::Plugins::Utils::like_prior_physical s_phys_pr_func;
    typedef Gambit::Scanner::Plugins::Utils::prior_physical s_pr_func;
    
    py::class_<s_phys_func, std::shared_ptr<s_phys_func>>(m, "like_physical_ptr")
    .def("__call__", [](s_phys_func &self, Gambit::Scanner::hyper_cube<double> vec)
    {
        auto &like = self.get();
        auto &map = like->getMap();
        int i = 0;
        for(auto &&name : like->getShownParameters())
            map[name] = vec[i++];
        
        return like(map);
    })
    .def("__call__", [](s_phys_func &self, py::dict params)
    {
        auto &like = self.get();
        auto &map = like->getMap();
        for(auto &&p: params)
            map[p.first.cast<std::string>()] = p.second.cast<double>();
        
        return like(map);
    })
    .def("__call__", [](s_phys_func &self, std::unordered_map<std::string, double> &map)
    {
        return self.get()(map);
    });
    
    py::class_<s_phys_pr_func, std::shared_ptr<s_phys_pr_func>>(m, "like_prior_physical_ptr")
    .def("__call__", [](s_phys_pr_func &self, Gambit::Scanner::hyper_cube<double> vec)
    {
        auto &like = self.get();
        auto &map = like->getMap();
        int i = 0;
        for(auto &&name : like->getShownParameters())
            map[name] = vec[i++];
        
        return like(map, true);
    })
    .def("__call__", [](s_phys_pr_func &self, py::dict params)
    {
        auto &like = self.get();
        auto &map = like->getMap();
        for(auto &&p: params)
            map[p.first.cast<std::string>()] = p.second.cast<double>();
        
        return like(map, true);
    })
    .def("__call__", [](s_phys_pr_func &self, std::unordered_map<std::string, double> &map)
    {
        return self.get()(map, true);
    });
    
    py::class_<s_pr_func, std::shared_ptr<s_pr_func>>(m, "prior_physical_ptr")
    .def("__call__", [](s_pr_func &self, Gambit::Scanner::hyper_cube<double> vec)
    {
        auto &like = self.get();
        auto &map = like->getMap();
        int i = 0;
        for(auto &&name : like->getShownParameters())
            map[name] = vec[++i];
        
        return like->getPrior().log_prior_density(map);
    })
    .def("__call__", [](s_phys_pr_func &self, py::dict params)
    {
        auto &like = self.get();
        auto &map = like->getMap();
        for(auto &&p: params)
            map[p.first.cast<std::string>()] = p.second.cast<double>();
        
        return like->getPrior().log_prior_density(map);
    })
    .def("__call__", [](s_phys_pr_func &self, std::unordered_map<std::string, double> &map)
    {
        return self.get()->getPrior().log_prior_density(map);
    });
    
    py::class_<s_func, s_ptr>(m, "like_ptr")
    .def("__call__", [](s_ptr self, Gambit::Scanner::hyper_cube<double> vec)
    {
        return static_cast<like_ptr &>(self)(vec);
    })
    .def("__call__", [](s_ptr self, std::unordered_map<std::string, double> &map)
    {
        return static_cast<like_ptr &>(self)(map);
    })
    .def("__call__", [](s_ptr self, py::dict params)
    {
        auto &map = self->getMap();
        for(auto &&p: params)
            map[p.first.cast<std::string>()] = p.second.cast<double>();
        
        return static_cast<like_ptr &>(self)(map);
    })
    .def("getPtID", [&](s_ptr self)
    {
        return self->getPtID();
    })
    .def("getPrinter", [&](s_func &self)
    {
        return &self.getPrinter();
    })
    .def("getPrior", [&](s_func &self)
    {
        return &self.getPrior();
    })
    .def("getPhysical", [](s_func &self)
    {
        return std::shared_ptr<s_phys_func>(new s_phys_func(self));
    }, py::return_value_policy::reference_internal)
    .def("getPriorPhysical", [](s_func &self)
    {
        return std::shared_ptr<s_phys_pr_func>(new s_phys_pr_func(self));
    }, py::return_value_policy::reference_internal);
}

PYBIND11_EMBEDDED_MODULE(scanner_plugin, m) 
{
    using namespace Gambit::Scanner::Plugins::ScannerPyPlugin;
    using namespace Gambit::Scanner::Plugins::Utils;
    
    m.attr("scannerbit") = m.import("scannerbit");
    
    m.def("get_printer", []()
    {
        return &get_printer();
    }, "", py::return_value_policy::reference);
    
    m.def("get_prior", []()
    {
        return &get_prior();
    }, "", py::return_value_policy::reference);
    
    m.def("get_inifile_node", [](py::args args) -> py::object
    {
        py::object ret = yaml_to_dict(get_inifile_node());
        
        for (auto &&arg : args)
            ret = py::dict(ret)[py::cast(arg.cast<std::string>())];
        
        return ret;
    });
    
    m.def("get_inifile_value", SCAN_PLUGIN_GET_INIFILE_VALUE_FUNC);
    m.def("get_dimension", get_dimension);
    m.def("get_purpose", get_purpose);
    
    py::class_<scanner_base, std::shared_ptr<scanner_base>>(m, "scanner")
    .def(py::init([](py::kwargs opts)
        {
        #ifdef WITH_MPI
            if (opts.contains("use_mpi") && !opts["use_mpi"].cast<bool>())
            {
                int numtasks;
                MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
                if (numtasks != 1)
                {
                    scan_err << "This scanner does not support MPI." << scan_end;
                }
            }
        #endif
                
            return new scanner_base();
        }))
    .def("run", &scanner_base::run)
    .def("print", &scanner_base::print)
    .def("print", &scanner_base::print_full)
    .def("transform", &scanner_base::transform)
    .def("transform", &scanner_base::transform_inplace)
    .def("transform", &scanner_base::transform_inplace_dict)
    .def("transform_to_vec", &scanner_base::transform_to_vec)
    .def("inverse_transform", &scanner_base::inverse_transform)
    .def("inverse_transform", &scanner_base::inverse_transform_dict)
    .def("inverse_transform", &scanner_base::inverse_transform_inplace)
    .def("inverse_transform", &scanner_base::inverse_transform_inplace_dict)
    .def("set_run_defaults", &scanner_base::set_run_defaults)
    .def_property_readonly("point_id", &scanner_base::point_id)
    .def_property_readonly("loglike_hypercube", &scanner_base::getLikeHyperCube)
    .def_property_readonly("loglike_physical", &scanner_base::getLikePhysical)
    .def_property_readonly("log_target_density", &scanner_base::getLikePriorPhysical)
    .def_property_readonly("log_prior_density", &scanner_base::getPriorPhysical)
    .def_property_readonly("loglike", &scanner_base::getLike)
    .def_property_readonly("args", &scanner_base::getOpts)
    .def_property_readonly("init_args", &scanner_base::getInitOpts)
    .def_property_readonly("run_args", &scanner_base::getRunOpts)
    .def_property_readonly("parameter_names", &scanner_base::getParameterNames)
    .def_property_readonly("mpi_rank", &scanner_base::getRank)
    .def_property_readonly("mpi_size", &scanner_base::getNumTasks)
    .def_property_readonly("dim", &scanner_base::getDim);
}

PYBIND11_EMBEDDED_MODULE(objective_plugin, m) 
{
    using namespace ::Gambit::Scanner::Plugins::ObjPyPlugin;
    using namespace ::Gambit::Scanner::Plugins::Utils;

    m.attr("scannerbit") = m.import("scannerbit");
    
    m.def("get_inifile_node", [](py::args args) -> py::object
    {
        py::object ret = yaml_to_dict(get_inifile_node());
        
        for (auto &&arg : args)
            ret = py::dict(ret)[py::cast(arg.cast<std::string>())];
        
        return ret;
    });
    
    m.def("get_inifile_value", SCAN_PLUGIN_GET_INIFILE_VALUE_FUNC);
    m.def("get_keys", get_keys);
    m.def("set_dimension", set_dimension);
    m.def("print_parameters", print_parameters);
}

#endif  // HAVE_PYBIND11
