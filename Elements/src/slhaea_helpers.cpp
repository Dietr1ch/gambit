//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Helper functions for dealing with SLHAea objects
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
#include "gambit/Utils/version.hpp"
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

  /// Add a new block to an SLHAea object, with or without a scale
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

  bool SLHAea_block_exists(SLHAstruct& slha, const str& block)
  {
    // Check if block exists
    bool found = false;
    if(slha.find(block) != slha.end()) found = true;
    return found;
  }

  bool SLHAea_check_block(SLHAstruct& slha, const str& block)
  {
    bool exists;
    if(SLHAea_block_exists(slha,block))
    {
      exists = true;
    }
    else
    {
      slha[block][""] << "BLOCK" << block;
      exists = false; // Didn't exist, but now it does.
    }
    return exists;
  }

  /// Check if a block exists in an SLHAea object, add it if not, and check if it has an entry at a given index
  // TODO: Ben: I just found this, and I can't say I understand the logic related to "overwrite". It also makes
  // overloading for two indices very difficult, so I'm going to delete it.
  bool SLHAea_check_block(SLHAstruct& slha, const str& block, const int index) /*, const bool overwrite)*/
  {
    bool found;
    // Check if block exists and create it if it doesn't
    SLHAea_check_block(slha, block);
    // Check for existing entry
    std::stringstream i;
    i<<index;
    SLHAea::Block::key_type key(1);
    key[0] = i.str();
    //std::cout << "Searching block "<<block<<" for key "<<key[0]<<std::endl;
    if( slha[block].find(key) != slha[block].end()) 
    {
      found = true;
    }
    else
    {
      found = false;
    }
    return found;
  }

  bool SLHAea_check_block(SLHAstruct& slha, const str& block, const int index1, const int index2) /*, const bool overwrite)*/
  {
    bool found;
    // Check if block exists and create it if it doesn't
    SLHAea_check_block(slha, block);
    // Check for existing entry
    std::stringstream i,j;
    i<<index1; j<<index2;
    SLHAea::Block::key_type key(2);
    key[0] = i.str();
    key[1] = j.str();
    //std::cout << "Searching block "<<block<<" for key "<<key[0]<<", "<<key[1]<<std::endl;
    if( slha[block].find(key) != slha[block].end() ) 
    {
      found = true;
    }
    else
    {
      found = false;
    }
    return found;
  }



  /// Check if a line exists in an SLHAea block, then overwrite it if it does.  Otherwise add the line.
  template <class T>
  void SLHAea_overwrite_block(SLHAstruct& slha /*modify*/, const str& block, int index,
   T value, const str& comment)
  {
    if(SLHAea_check_block(slha, block, index))
    {
      //std::cout << "Entry "<<block<<", "<<index<<" already exists, deleting and replacing it." <<std::endl;
      // entry exists already, delete it
      slha.at(block).at(index).at(1);
      auto& line = slha[block][index];
      line.clear();
      line << index << value << comment;
    }
    else
    {
      // Doesn't already exist, add it
      //std::cout << "Adding entry "<<block<<", "<<index<<std::endl;
      slha[block][""] << index << value << comment;
    }
  }

  /// Check if a line exists in an SLHAea block, then overwrite it if it does.  Otherwise add the line.
  template <class T>
  void SLHAea_overwrite_block(SLHAstruct& slha /*modify*/, const str& block, int index1, int index2,
   T value, const str& comment)
  {
    //std::vector<int> indices = initVector<int>(index1, index2);
    if(SLHAea_check_block(slha, block, index1, index2))
    {
      //std::cout << "Entry "<<block<<", "<<index1<<","<<index2<<" already exists, deleting and replacing it." <<std::endl;
      // entry exists already, delete it
      //slha.at(block).at(indices).at(1); // Is this actually a valid way to use SLHAea? I don't see it in their documentation.
      std::stringstream i,j;
      i<<index1; j<<index2;
      SLHAea::Block::key_type key(2);
      key[0] = i.str();
      key[1] = j.str();
      auto& line = slha[block][key];
      line.clear();
      line << index1 << index2 << value << comment;
    }
    else
    {
      //std::cout << "Adding entry "<<block<<","<<index1<<","<<index2<<std::endl;
      // Doesn't exist, add it
      slha[block][""] << index1 << index2 << value << comment;
    }
  }

  /// Delete a block entirely if it exists (TODO: actually only deletes first instance of the block found!)
  void SLHAea_delete_block(SLHAstruct& slha, const std::string& block)
  {
     auto it = slha.find(block);
     if(it!=slha.end()) slha.erase(it);
  }

  void SLHAea_add_GAMBIT_SPINFO(SLHAstruct& slha /*modify*/)
  {
     // For now we don't try to track where the data originally came from, we just label
     // it as GAMBIT-produced.
     std::ostringstream progname;
     if(not SLHAea_check_block(slha, "SPINFO", 1, false))
     {
        SLHAea_add(slha, "SPINFO", 1, "GAMBIT", "Program");
        SLHAea_add(slha, "SPINFO", 2, gambit_version(), "Version number");
     }
  }

  /// Add an entry to an SLHAea object (if overwrite=false, only if it doesn't already exist)
  /// @{
  void SLHAea_add(SLHAstruct& slha /*modify*/, const str& block, const int index,
   const double value, const str& comment, const bool overwrite)
  {
    if (SLHAea_check_block(slha, block, index) and not overwrite) return;
    SLHAea_overwrite_block(slha, block, index, value, (comment == "" ? "" : "# " + comment));
  }

  // string version
  void SLHAea_add(SLHAstruct& slha /*modify*/, const str& block, const int index,
   const str& value, const str& comment, const bool overwrite)
  {
    if (SLHAea_check_block(slha, block, index, overwrite)) return;
    SLHAea_overwrite_block(slha, block, index, value, (comment == "" ? "" : "# " + comment));
  }

  // int version
  void SLHAea_add(SLHAstruct& slha /*modify*/, const str& block, const int index,
   const int value, const str& comment, const bool overwrite)
  {
    if (SLHAea_check_block(slha, block, index, overwrite)) return;
    SLHAea_overwrite_block(slha, block, index, value, (comment == "" ? "" : "# " + comment));
  }

  // two index version
  void SLHAea_add(SLHAstruct& slha /*modify*/, const str& block, const int index1, const int index2,
   const double& value, const str& comment, const bool overwrite)
  {
    if (SLHAea_check_block(slha, block, index1, index2) and not overwrite) return;
    SLHAea_overwrite_block(slha, block, index1, index2, value, (comment == "" ? "" : "# " + comment));
  }

  /// @}

  /// Add an entry from a subspectrum getter to an SLHAea object; SLHA index given by pdg code
  void SLHAea_add_from_subspec(SLHAstruct& slha /*modify*/, const str local_info, const SubSpectrum& subspec,
   const Par::Tags partype, const std::pair<int, int>& pdg_pair, const str& block, const str& comment,
   const bool error_if_missing, const double rescale)
  {
     if(subspec.has(partype,pdg_pair))
     {
       SLHAea_overwrite_block(slha, block, pdg_pair.first, subspec.get(partype,pdg_pair)*rescale, (comment == "" ? "" : "# " + comment));
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
       SLHAea_overwrite_block(slha, block, slha_index, subspec.get(partype,name)*rescale, (comment == "" ? "" : "# " + comment));
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
      SLHAea_overwrite_block(slha, block, slha_index1, slha_index2, subspec.get(partype,name,index1,index2)*rescale, (comment == "" ? "" : "# " + comment));
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

  /// Adds QNUMBERS entry for a particle, SLHA index given by the PDG code
  void SLHAea_add_QNumbers_from_subspec(SLHAstruct& slha, const SubSpectrum& subspec, 
   const std::pair<int,int> pdg_pair)
  {
    if (subspec.has(Par::Pole_Mass,pdg_pair))
    {
      str long_name = Models::ParticleDB().long_name(pdg_pair);
      int spinx2 = Models::ParticleDB().get_spinx2(long_name);
      int chargex3 = Models::ParticleDB().get_chargex3(long_name);
      int color = Models::ParticleDB().get_color(long_name);
      bool is_anti = Models::ParticleDB().has_antiparticle(long_name);

      SLHAea::Block QNblock("QNUMBERS");
      SLHAea::Line line1, line2, line3, line4, line5;
      line1 << "BLOCK" << "QNUMBERS" << pdg_pair.first << "# " + long_name;
      line2 << 1 << chargex3 << "# 3 times electric charge";
      line3 << 2 << spinx2+1 << "# number of spin states (2S+1)";
      line4 << 3 << color    << "# colour rep (1: singlet, 3: triplet, 8: octet)";
      line5 << 4 << is_anti  << "# Particle/Antiparticle distinction (0=own anti)";
      QNblock.push_back(line1);
      QNblock.push_back(line2);
      QNblock.push_back(line3);
      QNblock.push_back(line4);
      QNblock.push_back(line5);
      slha.push_front(QNblock);

    }
  }

  /// Write a SimpleSpectrum to an SLHAea object.
  void add_SimpleSpec_to_SLHAea(const SubSpectrum& subspec, SLHAstruct& slha, const SubSpectrumContents& contents)
  {

    // Pick out the parameters whose SLHA block name is not: SMINPUTS, CKMBLOCK, YUKAWA, or empty.
    std::vector<SpectrumParameter> bsm = contents.all_BSM_parameters();

    // Then assign them to the correct part of the SLHAea object
    for (std::vector<SpectrumParameter>::const_iterator it = bsm.begin(); it != bsm.end(); ++it)
    {
      // The SLHAea comment changes based on the ParType
      std::ostringstream comment;

      // If it's a mass, we always want to write it to the MASS block. Otherwise use what's been specified explicitly.
      str blockname = (it->tag() == Par::Pole_Mass ? "MASS" : it->blockname());

      // Masses
      if (it->tag() == Par::Pole_Mass)
      { 
        comment << it->name() << " mass.";
        std::pair<int, int> pdg_pair = Models::ParticleDB().pdg_pair(it->name());
        SLHAea_add_from_subspec(slha, LOCAL_INFO, subspec, it->tag(), pdg_pair, blockname, comment.str());
        SLHAea_add_QNumbers_from_subspec(slha, subspec, pdg_pair);
      }
      // The rest
      else 
      {
        // Scalar case
        if (it->shape().size()==1 and it->shape()[0]==1) 
        {
          SLHAea_add_from_subspec(slha, LOCAL_INFO, subspec, it->tag(), it->name(), blockname, it->blockindex(), comment.str());
        }
        // Vector (1 index) 
        else if (it->shape().size() == 1 and it->shape()[0] > 1)
        {
          for (int i=1; i<it->shape()[0]+1; ++i)
          { 
            // Increment +1 to each entry for the BLOCKNAME
            SLHAea_add_from_subspec(slha, LOCAL_INFO, subspec, it->tag(), it->name(), blockname, it->blockindex()+i, comment.str());
          }
        }
        // Matrix (2 indices) -- blockindex() should just start from 1.
        else if (it->shape().size() == 2)
        {
          for (int i=1; i<it->shape()[0]+1; ++i)
          { 
            for (int j=1; j<it->shape()[0]+1; ++j)
            {
              SLHAea_add_from_subspec(slha, LOCAL_INFO, subspec, it->tag(), it->name(), i, j, blockname, i, j, comment.str());
            }
          }
        }
      }
    }

  } 

} 
