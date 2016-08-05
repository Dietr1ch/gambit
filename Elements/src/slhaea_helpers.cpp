//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Helper functions for SMInputs struct
///
///  *********************************************
///
///  Authors:
///
///  \author Ben Farmer
///          (benjamin.farmer@fysik.su.se)
///  \date 2015 Mar
///
///  \author Pat Scott
///          (p.scott@imperial.ac.uk)
///  \date 2015 Jul
///
///  *********************************************

#include "gambit/Utils/standalone_error_handlers.hpp"
#include "gambit/Elements/slhaea_helpers.hpp"
#include "gambit/Elements/subspectrum.hpp"

namespace Gambit
{
  /// Read an SLHA file in to an SLHAea object with some error-checking
  SLHAstruct read_SLHA(str slha)
  {
    SLHAstruct slhaea;
    std::ifstream ifs(slha.c_str());
    if (!ifs.good())
    {
     std::ostringstream err;
     err << "ERROR: SLHA file " << slha << " not found.";
     utils_error().raise(LOCAL_INFO,err.str());
    }
    ifs >> slhaea;
    ifs.close();
    return slhaea;
  }

  /// Get an entry from an SLHAea object as a double, with some error checking
  double SLHAea_get(const SLHAstruct& slha, const str& block, const int index)
  {
    double output = 0.0;
    try
    {
      output = SLHAea::to<double>(slha.at(block).at(index).at(1));
    }
    catch (const std::out_of_range& e)
    {
      std::ostringstream errmsg;
      errmsg << "Error accessing data at index " << index << " of block " << block
             << ". Please check that the SLHAea object was properly filled." << std::endl
             << "(Received out_of_range error from SLHAea class with message: " << e.what() << ")";
      utils_error().raise(LOCAL_INFO,errmsg.str());
    }
    return output;
  }

  /// Get an entry from an SLHAea object as a double; raise a warning and use a default value if the entry is missing
  double SLHAea_get(const SLHAstruct& slha, const str& block, const int index, const double defvalue)
  {
    double output;
    try
    {
      output = SLHAea::to<double>(slha.at(block).at(index).at(1));
    }
    catch (const std::out_of_range& e)
    {
      std::ostringstream warn;
      warn << "Warning! No entry found at index "<<index<<" of block "<<block<<". Using default value: "<<defvalue<< std::endl;
      utils_warning().raise(LOCAL_INFO,warn.str());
      output = defvalue;
    }
    return output;
  }

  /// Add a new block to an SLHAea object, with our without a scale
  void SLHAea_add_block(SLHAstruct& slha, const str& name, const double scale)
  {
    if(scale==-1)
    {
      slha[name][""] << "BLOCK" << name;
    }
    else
    {
      slha[name][""] << "BLOCK" << name << "Q=" << scale;
    }
  }

  /// Check if a block exists in an SLHAea object, add it if not, and check if it has an entry at a given index
  bool SLHAea_check_block(SLHAstruct& slha, const str& block, const int index, const bool overwrite)
  {
    // Check if block exists
    try
    {
      slha.at(block);
    }
    catch (const std::out_of_range& e)
    {
      // Nope; add it.
      slha[block][""] << "BLOCK" << block;
    }
    // Check for existing entry
    if(not overwrite)
    {
      try
      {
        slha.at(block).at(index).at(1);
        // Entry exists, no further action required
        return true;
      }
      catch (const std::out_of_range& e)
      {
        // entry doesn't exist; continue with writing
      }
    }
    return false;
  }

  /// Add an entry to an SLHAea object (if overwrite=false, only if it doesn't already exist)
  /// @{
  void SLHAea_add(SLHAstruct& slha /*modify*/, const str& block, const int index,
   const double value, const str& comment, const bool overwrite)
  {
    if (SLHAea_check_block(slha, block, index, overwrite)) return;
    slha[block][""] << index << value << (comment == "" ? "" : "# " + comment);
  }
  void SLHAea_add(SLHAstruct& slha /*modify*/, const str& block, const int index,
   const str& value, const str& comment, const bool overwrite)
  {
    if (SLHAea_check_block(slha, block, index, overwrite)) return;
    slha[block][""] << index << value << (comment == "" ? "" : "# " + comment);
  }
  void SLHAea_add(SLHAstruct& slha /*modify*/, const str& block, const int index,
   const int value, const str& comment, const bool overwrite)
  {
    if (SLHAea_check_block(slha, block, index, overwrite)) return;
    slha[block][""] << index << value << (comment == "" ? "" : "# " + comment);
  }
  /// @}

  /// Add an entry from a subspectrum getter to an SLHAea object; SLHA index given by pdg code
  void SLHAea_add_from_subspec(SLHAstruct& slha /*modify*/, const str local_info, const SubSpectrum& subspec,
   const Par::Tags partype, const std::pair<int, int>& pdg_pair, const str& block, const str& comment,
   const bool error_if_missing, const double rescale)
  {
     if(subspec.has(partype,pdg_pair))
     {
        slha[block][""] << pdg_pair.first << subspec.get(partype,pdg_pair)*rescale << comment;
     }
     else if(error_if_missing)
     {
        std::ostringstream errmsg;
        errmsg << "Error creating SLHAea output from SubSpectrum object! Required entry not found (paramtype="<<Par::toString.at(partype)
               <<", pdg:context="<<pdg_pair.first<<":"<<pdg_pair.second<<")";
        utils_error().raise(local_info,errmsg.str());
     }
     // else skip this entry
     return;
  }

  /// Add an entry from a subspectrum getter to an SLHAea object; 1 SLHA index
  void SLHAea_add_from_subspec(SLHAstruct& slha /*modify*/, const str local_info, const SubSpectrum& subspec,
   const Par::Tags partype, const str& name, const str& block, const int slha_index,
   const str& comment, const bool error_if_missing, const double rescale)
  {
     if(subspec.has(partype,name))
     {
        slha[block][""] << slha_index << subspec.get(partype,name)*rescale << comment;
     }
     else if(error_if_missing)
     {
        std::ostringstream errmsg;
        errmsg << "Error creating SLHAea output from SubSpectrum object! Required entry not found (paramtype="<<Par::toString.at(partype)<<", name="<<name<<")";
        utils_error().raise(local_info,errmsg.str());
     }
     // else skip this entry
     return;
  }

  /// Add an entry from a subspectrum getter to an SLHAea object; two SubSpectrum getter indices, two SLHA indices
  void SLHAea_add_from_subspec(SLHAstruct& slha /*modify*/, const str local_info, const SubSpectrum& subspec,
   const Par::Tags partype, const str& name, const int index1, const int index2, const str& block,
   const int slha_index1, const int slha_index2, const str& comment, const bool error_if_missing, const double rescale)
  {
    if(subspec.has(partype,name,index1,index2))
    {
      slha[block][""] << slha_index1 << slha_index2 << subspec.get(partype,name,index1,index2)*rescale << comment;
    }
    else if(error_if_missing)
    {
      std::ostringstream errmsg;
      errmsg << "Error creating SLHAea output from SubSpectrum object! Required entry not found (paramtype="<<Par::toString.at(partype)<<", name="<<name<<", index1="<<index1<<", index2="<<index2;
      utils_error().raise(local_info,errmsg.str());
    }
    // else skip this entry
    return;
  }

}
