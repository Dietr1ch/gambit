//  GAMBIT: Global and Modular BSM Inference Tool
//  *********************************************
///  \file
///
///  test functions implementations.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Gregory Martinez
///          (gregory.david.martinez@gmail.com)
///  \date Feb 2014
///
///  *********************************************

#ifndef __plugin_factory_hpp__
#define __plugin_factory_hpp__

#include <map>
#include <vector>
#include <string>
#include <unordered_map>
#include <sstream>
#include <unordered_map>
#include <typeinfo>

#include "gambit/ScannerBit/scanner_utils.hpp"
#include "gambit/ScannerBit/scan.hpp"
#include "gambit/ScannerBit/plugin_interface.hpp"
#include "gambit/Utils/yaml_options.hpp"
#include "gambit/Utils/type_index.hpp"

#define LOAD_FUNC_TEMPLATE(name, ...) REGISTER_ELEM(__functions__, typeid(__VA_ARGS__), name<__VA_ARGS__>) 
#define LOAD_MULTI_FUNC_TEMPLATE(name, ...) REGISTER_ELEM(__multi_functions__, typeid(__VA_ARGS__), name<__VA_ARGS__>) 

namespace Gambit
{
    namespace Scanner
    {       
        using Gambit::type_index;
        
        registry
        {
            typedef void* func_type(const std::vector<std::string> &, const std::string &);
            typedef void* multi_func_type(const std::map<std::string, std::vector<std::string>> &, const std::vector<std::pair<std::string, std::string>> &);
            std::unordered_map<type_index, func_type *, Gambit::type_hasher, Gambit::type_equal_to> __functions__;
            std::unordered_map<type_index, multi_func_type *> __multi_functions__;
        }
        
        template <typename T>
        class Scanner_Plugin_Function;
        
        template <typename T>
        class Multi_Scanner_Plugin_Function;
        
        LOAD_FUNC_TEMPLATE(Scanner_Plugin_Function, double (std::unordered_map<std::string, double> &));
        LOAD_FUNC_TEMPLATE(Scanner_Plugin_Function, void (const std::vector<double> &, std::unordered_map<std::string, double> &));
        LOAD_FUNC_TEMPLATE(Scanner_Plugin_Function, std::vector<double> (std::unordered_map<std::string, double> &));
        LOAD_MULTI_FUNC_TEMPLATE(Multi_Scanner_Plugin_Function, double(std::unordered_map<std::string, double> &));
        
        inline std::map<std::string, std::vector<std::string>> convert_to_map(const std::vector<std::string> &vec)
        {
            std::map<std::string, std::vector<std::string>> ret;
            
            for (auto it = vec.begin(), end = vec.end(); it != end; it++)
            {
                std::string::size_type pos = it->find("::");
                ret[it->substr(0, pos)].push_back(*it);
            }
            
            return ret;
        }
        
        template <typename ret, typename... args>
        class Scanner_Plugin_Function<ret (args...)> : public Plugins::Plugin_Interface<ret (args...)>, public Function_Base<ret (args...)>
        {
        private:
            //std::vector<double> &params;
                
        public:
            Scanner_Plugin_Function(const std::vector<std::string> &params, const std::string &name) 
                    : Plugins::Plugin_Interface<ret (args...)>("objective", name, params)
            {
            }
            
            ret main(const args&... in)
            {
                return this->Plugins::Plugin_Interface<ret (args...)>::operator()(in...);
            }
        };
        
        template <typename ret, typename... args>
        class Multi_Scanner_Plugin_Function <ret (args...)> : public Function_Base<ret (args...)>
        {
        private:
            std::vector< Scanner_Plugin_Function<ret (args...)> > functions;
                
        public:
            Multi_Scanner_Plugin_Function(const std::map< std::string, std::vector<std::string> > &params, const std::vector<std::pair<std::string, std::string>> &names)
            {
                for (auto it = names.begin(), end = names.end(); it != end; it++)
                {
                    functions.emplace_back(params.at(it->second), it->first);
                }
            }
            
            ret main(const args&... in)
            {
                ret retval = 0.0;
                for (auto it = functions.begin(); it != functions.end(); it++)
                {
                    retval += it->main(in...);
                }
                
                return retval;
            }
        };
        
        class Plugin_Function_Factory : public Factory_Base
        {
        private:
            std::map< std::string, std::vector<std::pair<std::string, std::string> > > names;
            std::map< std::string, std::vector<std::string> > parameters;
            std::unordered_map<std::string, Gambit::type_index> purpose_index;
                
        public:
            Plugin_Function_Factory(const std::vector<std::string> &keys, std::map< std::string, std::vector<std::pair<std::string, std::string>> > &names) 
                    : names(names)
            {
                parameters = convert_to_map(keys);
                purpose_index["Likelihood"] = typeid(double (std::unordered_map<std::string, double> &));
                purpose_index["LogLike"] = typeid(double (std::unordered_map<std::string, double> &));
                purpose_index["Observable"] = typeid(std::vector<double> (std::unordered_map<std::string, double> &));
                purpose_index["Prior"] = typeid(void (const std::vector<double> &, std::unordered_map<std::string, double> &));
            }
            
            void * operator() (const std::string &purpose) const
            {
                auto it = names.find(purpose);
                std::string purpose_i = purpose;
                
                if (purpose_index.find(purpose) == purpose_index.end())
                {
                    purpose_i = "Likelihood";
                }
                
                if (it == names.end())
                {
                    return NULL;
                }
                else if (it->second.size() == 1)
                {
                    return __functions__.at(purpose_index.at(purpose_i))(parameters.at(it->second.at(0).second), it->second.at(0).first);
                }
                else if (it->second.size() > 1)
                {
                    return __multi_functions__.at(purpose_index.at(purpose_i))(parameters, it->second);
                }
                else
                {
                    return NULL;
                }
            }

            ~Plugin_Function_Factory()
            {
            }
        };
    }
}

#endif
