//  GAMBIT: Global and Modular BSM Inference Tool
//  *********************************************
///  \file
///
///  Class to hold details of scanner plugins and
///  define simple comparison operations on them. 
///
///  *********************************************
///
///  Authors (add name and date if you modify):
//
///  \author Gregory Martinez
///          (gregory.david.martinez@gmail.com)
///  \date 2013 August
///  \date 2014 Feb
///
///  \author Pat Scott
///          (p.scott@imperial.ac.uk)   
///  \date 2014 Dec
///
///  *********************************************

#include <string>
#include <ostream>
#include <sstream>

#include "gambit/ScannerBit/scanner_utils.hpp"
#include "gambit/ScannerBit/plugin_details.hpp"

namespace Gambit
{

        namespace Scanner
        {

                namespace Plugins
                {
                        inline std::string spacing(int len, int maxlen)
                        {
                                int offset = 0;
                                if (len < maxlen) {offset=maxlen-len;}
                                return std::string(offset+5,' ');
                        }
                        
                        Plugin_Details::Plugin_Details(const std::string &str) : full_string(str)
                        {
                                // 0 status = -1;
                                // 1 status = 0;
                                // 2 status = -2;
                                status= 0;
                                
                                std::string::size_type posMid = str.rfind("__v__");
                                version = str.substr(posMid + 5);
                                
                                std::string::size_type posLast = str.rfind("__t__", posMid - 1);
                                type = str.substr(posLast + 5, posMid - posLast - 5);
                                plugin = str.substr(0, posLast);
                                posLast = version.find("_");
                                major_version = StringToInt(version.substr(0, posLast));
                                posMid = version.find("_", posLast + 1);
                                minor_version = StringToInt(version.substr(posLast + 1, posMid - posLast - 1));
                                posLast = version.find("_", posMid + 1);
                                patch_version = StringToInt(version.substr(posMid + 1, posLast - posMid - 1));
                                release_version = version.substr(posLast + 1);
                                version = IntToString(major_version) + "." + IntToString(minor_version) + "." + IntToString(patch_version);
                                if (release_version != "") 
                                        version += "-" + release_version;
                        }
                        
                        void Plugin_Details::get_status(const YAML::Node &libNode, const YAML::Node &plugNode)
                        {
                                std::vector<std::string> linked_libs_plug, found_incs_plug;
                                
                                if (plugNode.IsMap())
                                {
                                        if (plugNode[type] && plugNode[type][plugin] && plugNode[type][plugin][version] && plugNode[type][plugin][version].IsMap())
                                        {
                                                if (plugNode[type][plugin][version]["reqd_inifile_entries"].IsSequence())
                                                        reqd_inifile_entries = plugNode[type][plugin][version]["reqd_inifile_entries"].as<std::vector<std::string>>();
                                                if (plugNode[type][plugin][version]["reqd_libraries"].IsSequence())
                                                        reqd_not_linked_libs = plugNode[type][plugin][version]["reqd_libraries"].as<std::vector<std::string>>();
                                                if (plugNode[type][plugin][version]["not_linked_libraries"].IsSequence())
                                                        ini_libs_not_found = plugNode[type][plugin][version]["not_linked_libraries"].as<std::vector<std::string>>();
                                                if (plugNode[type][plugin][version]["reqd_include_paths"].IsSequence())
                                                        reqd_incs_not_found = plugNode[type][plugin][version]["reqd_include_paths"].as<std::vector<std::string>>();
                                                if (plugNode[type][plugin][version]["not_found_include_paths"].IsSequence())
                                                        ini_incs_not_found = plugNode[type][plugin][version]["not_found_include_paths"].as<std::vector<std::string>>();
                                                
                                                if (plugNode[type][plugin][version]["linked_libraries"].IsSequence())
                                                        linked_libs_plug = plugNode[type][plugin][version]["linked_libraries"].as<std::vector<std::string>>();
                                                if (plugNode[type][plugin][version]["found_include_paths"].IsSequence())
                                                        found_incs_plug = plugNode[type][plugin][version]["found_include_paths"].as<std::vector<std::string>>();
                                        }
                                }
                                
                                std::string lib = path.substr(path.rfind("/") + 1);
                                if (libNode.IsMap())
                                {
                                        if (libNode[lib].IsMap())
                                        {
                                                std::multimap<std::string, std::string> linked_libs_temp;
                                                if (libNode[lib]["linked_libs"].IsMap())
                                                        for (auto it = libNode[lib]["linked_libs"].begin(), end = libNode[lib]["linked_libs"].end(); it != end; it++)
                                                               linked_libs_temp.insert(std::pair<std::string, std::string>(it->first.as<std::string>(), it->second.as<std::string>()));
                                                        
                                                for (auto it = linked_libs_plug.begin(), end = linked_libs_plug.end(); it != end; it++)
                                                {
                                                        auto range = linked_libs_temp.equal_range(*it);
                                                        if (range.first != range.second)
                                                                linked_libs.insert(range.first, range.second);
                                                }
                                                
                                                std::vector<std::string> linked_temp;
                                                for (auto it = reqd_not_linked_libs.begin(), end = reqd_not_linked_libs.end(); it != end; it++)
                                                {
                                                        auto range = linked_libs_temp.equal_range(*it);
                                                        if (range.first == range.second)
                                                        {
                                                                linked_temp.push_back(*it);
                                                        }
                                                        else
                                                        {
                                                                linked_libs.insert(range.first, range.second);
                                                        }
                                                }
                                                
                                                reqd_not_linked_libs = linked_temp;
                                                
                                                std::multimap<std::string, std::string> found_incs_temp;
                                                if (libNode[lib]["found_incs"].IsMap())
                                                        for (auto it = libNode[lib]["found_incs"].begin(), end = libNode[lib]["found_incs"].end(); it != end; it++)
                                                                found_incs_temp.insert(std::pair<std::string, std::string>(it->first.as<std::string>(), it->second.as<std::string>()));
                                                        
                                                for (auto it = found_incs_plug.begin(), end = found_incs_plug.end(); it != end; it++)
                                                {
                                                        found_incs.insert(std::pair<std::string, std::string>(type + "_locations.yaml", *it));
                                                }
                                                
                                                std::vector<std::string> found_temp;
                                                for (auto it = reqd_incs_not_found.begin(), end = reqd_incs_not_found.end(); it != end; it++)
                                                {
                                                        auto range = found_incs_temp.equal_range(*it);
                                                        if (range.first == range.second)
                                                        {
                                                                found_temp.push_back(*it);
                                                        }
                                                        else
                                                        {
                                                                found_incs.insert(range.first, range.second);
                                                        }
                                                }
                                                
                                                reqd_incs_not_found = found_temp;
                                        }
                                }
                        }
                                
                        std::string Plugin_Details::printMin() const
                        {
                                std::stringstream out;
                                out << "plugin:  " << plugin << std::endl;
                                out << "version:  " << version << std::endl;
                                out << "type:  " << type << std::endl;
                                
                                return out.str();
                        }
                                
                        std::string Plugin_Details::print() const
                        {
                                std::stringstream out;
                                out << "plugin:  " << plugin << std::endl;
                                out << "\tversion:  " << version << std::endl;
                                out << "\tmajor version:  " << major_version << std::endl;
                                out << "\tminor version:  " << minor_version << std::endl;
                                out << "\tpatch version:  " << patch_version << std::endl;
                                out << "\tplugin path:  " << path << std::endl;
                                out << "\ttype:  " << type << std::endl;
                                
                                return out.str();
                        }
                        
                        std::string Plugin_Details::printFull() const
                        {
                                std::stringstream out;
                                const int maxlen1 = 20;
                                const int maxlen2 = 20;
                                // Default, list-format output header
                                out << type << " plugin" << spacing(type.length() + 7, maxlen1) << "version" << spacing(7, maxlen2) << "status" << std::endl;
                                out << "----------------------------------------------------------------------------" << std::endl;
                                out << plugin << spacing(plugin.length(), maxlen1) << version << spacing(version.length(), maxlen2) << status << std::endl;
                                out << "\nrequired inifile entries:  " << reqd_inifile_entries << std::endl;
                                out << "\nlink status" << std::endl;
                                out << "-----------" << std::endl;
                                out << "missing libraries requested by plugin: " << reqd_not_linked_libs << std::endl;
                                out << "missing libraries specified in inifile: " << ini_libs_not_found << std::endl;
                                out << "linked libraries:";
                                if (linked_libs.size() == 0)
                                {
                                        out << " none" << std::endl;
                                }
                                else
                                {
                                        out << std::endl;
                                        for (auto it = linked_libs.begin(), end = linked_libs.end(); it != end; it++)
                                                out << "    " << it->first << ": " << it->second << std::endl;
                                }
                                
                                out << "\ninclude header status" << std::endl;
                                out << "---------------------" << std::endl;
                                out << "missing headers requested by plugin: " << reqd_incs_not_found << std::endl;
                                out << "missing headers specified in inifile: " << ini_incs_not_found << std::endl;
                                out << "headers found:";
                                if (found_incs.size() == 0)
                                {
                                        out << " none" << std::endl;
                                }
                                else
                                {
                                        out << std::endl;
                                        for (auto it = found_incs.begin(), end = found_incs.end(); it != end; it++)
                                                out << "    " << it->first << ": " << it->second << std::endl;
                                }
                                
                                return out.str();
                        }
                        
                        bool Plugin_Version_Supersedes(const Plugin_Details &plug1, const Plugin_Details &plug2)
                        {
                                if (plug1.major_version > plug2.major_version)
                                {
                                        return true;
                                }
                                else if (plug1.major_version == plug2.major_version)
                                {
                                        if (plug1.minor_version > plug2.minor_version)
                                        {
                                                return true;
                                        }
                                        else if (plug1.minor_version == plug2.minor_version)
                                        {
                                                if (plug1.patch_version > plug2.patch_version)
                                                {
                                                        return true;
                                                }
                                                else if (plug1.patch_version == plug2.patch_version)
                                                {
                                                        if (plug1.release_version == "" && plug2.release_version != "")
                                                        {
                                                                return true;
                                                        }
                                                }
                                        }
                                }
                                
                                return false;
                        }
                                        
                }

        }

}