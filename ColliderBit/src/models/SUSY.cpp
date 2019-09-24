//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  SUSY-specific sources for ColliderBit.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Pat Scott
///          (p.scott@imperial.ac.uk)
///  \date 2019 Jan
///
///  *********************************************

#include "gambit/ColliderBit/getPy8Collider.hpp"
#include "gambit/ColliderBit/generateEventPy8Collider.hpp"

namespace Gambit
{
  namespace ColliderBit
  {

    // Get Monte Carlo event generator
    GET_SPECIFIC_PYTHIA(getPythia, Pythia_default, MSSM_spectrum, , IS_SUSY)
    // Get Monte Carlo event generator from SLHA file input
    GET_SPECIFIC_PYTHIA_SLHA(getPythia_SLHA, Pythia_default, )

    GET_PYTHIA_AS_BASE_COLLIDER(getPythiaAsBase)

    // Run event generator
    GET_PYTHIA_EVENT(generateEventPythia)


    // Get next SLHA file path and content (for use with model CB_SLHA_file_model)
    void getNextSLHAFileNameAndContent(pair_str_SLHAstruct& result)
    {
      using namespace Pipes::getNextSLHAFileNameAndContent;

      static unsigned int counter = 0;
      static bool first = true;

      if (first)
      {
        if (!runOptions->hasKey("SLHA_filenames")) ColliderBit_error().raise(LOCAL_INFO,"Expected YAML file option 'SLHA_filenames' (a list of SLHA filenames) not found.");
        first = false;
      }

      const static std::vector<str> filenames = runOptions->getValue<std::vector<str> >("SLHA_filenames");

      if (counter >= filenames.size())
      {
        invalid_point().raise("No more SLHA files. My work is done.");
        result = std::make_pair("", SLHAstruct());
      }
      else
      {
        const str& filename = filenames.at(counter);
        result = std::make_pair(filename, read_SLHA(filename));
      }

      counter++;
    }


    // Read a single SLHA file and update some entries for each scan point 
    // (for use with models CB_SLHA_simpmod_scan_model and CB_SLHA_scan_model)
    void getAndReplaceSLHAContent(pair_str_SLHAstruct& result)
    {
      using namespace Pipes::getAndReplaceSLHAContent;

      static unsigned int counter = 0;

      static str filename;
      static SLHAstruct file_content;

      static YAML::Node keysNode;
      static Options keysOptions; 
      static std::map<str,str> SLHAkey_to_parname;
      
      // Do the variable initialization only once
      static bool first = true;
      if (first)
      {
        if (!runOptions->hasKey("SLHA_filename")) ColliderBit_error().raise(LOCAL_INFO,"Expected YAML file option 'SLHA_filename' (a single SLHA filename) not found.");
        if (!runOptions->hasKey("replace_SLHA_keys")) ColliderBit_error().raise(LOCAL_INFO,"Expected YAML file option 'replace_SLHA_keys' (a list of strings in the SLHAea key format, e.g. 'MASS;1000022;1') not found.");

        // Get filename of base SLHA file
        filename = runOptions->getValue<str>("SLHA_filename");

        // Read the original SLHA file once
        file_content = read_SLHA(filename);

        // Get the YAML options under 'replace_SLHA_keys'
        keysNode = runOptions->getValue<YAML::Node>("replace_SLHA_keys");
        keysOptions = Options(keysNode);

        // Construct a map from SLHA keys to scan model parameters
        for (const str& parname : keysOptions.getNames())
        {
          std::vector<str> slhakeys = keysOptions.getValue<std::vector<str> >(parname);
          for (const str& slhakey : slhakeys) 
          {
            SLHAkey_to_parname[slhakey] = parname;
          }
        }

        first = false;
      }

      // Generate new SLHA content by replacing SLHA elements with scan parameters
      SLHAstruct new_content(file_content);
      static int precision = 8;
      for (const auto& key_param_pair : SLHAkey_to_parname)
      {
        new_content.field(key_param_pair.first) = SLHAea::to_string(*Param.at(key_param_pair.second), precision);
      }

      // Construct a dummy name for the SLHA "file" we pass around as a SLHAea object
      std::stringstream filename_mod_ss;
      filename_mod_ss << filename << ".point" << counter;

      // Save result as a pair_str_SLHAstruct
      result = std::make_pair(filename_mod_ss.str(), new_content);

      /// @todo Add option to save the new SLHA content to file 

      counter++;
    }



    // Extract SLHA file elements (for use with model CB_SLHA_file_model)
    void getSLHAFileElements(map_str_dbl& result)
    {
      using namespace Pipes::getSLHAFileElements;

      // Split the required SLHAFileNameAndContent pair
      const str& filename = Dep::SLHAFileNameAndContent->first;
      const SLHAstruct& content = Dep::SLHAFileNameAndContent->second;

      // Should missing elements be replaced by a default value?
      const static bool use_missing_element_value = runOptions->hasKey("value_for_missing_elements");
      static double missing_element_value;

      static bool first = true;
      if (first)
      {
        // Check that the required YAML option "SLHA_keys" is present
        if (!runOptions->hasKey("SLHA_keys")) ColliderBit_error().raise(LOCAL_INFO,"Expected YAML file option 'SLHA_keys' (a list of strings in the SLHAea key format, e.g. 'MASS;1000022;1') not found.");

        // Read default value for missing elements;
        if (use_missing_element_value) missing_element_value = runOptions->getValue<double>("value_for_missing_elements");

        first = false;
      }

      // Read the list of SLHA element keys
      const static std::vector<str> slha_element_keys = runOptions->getValue<std::vector<str> >("SLHA_keys");

      // Loop through the list of SLHA keys and grab the corresponding elements from the SLHA content
      for(str key_str : slha_element_keys)
      {

        // Construct a SLHAea::Key from the key string
        const SLHAea::Key key(key_str);

        // Grab the correct entryand store in the results map
        try
        {
          result[key_str] = SLHAea::to<double>( content.field(key) );
        }
        catch (const std::out_of_range& e)
        {
          std::stringstream errmsg_ss;
          errmsg_ss << "Could not find SLHA element " << key_str << " in file " << filename;

          if (use_missing_element_value)
          {
            logger() << errmsg_ss.str() << EOM;
            result[key_str] = missing_element_value;            
          }
          else
          {
            ColliderBit_error().raise(LOCAL_INFO, errmsg_ss.str());
          }
        }
      }
    }


    // Extract an SLHAstruct with the specturm, either from the MSSM_spectrum 
    // capability (for MSSM models), or simply from the SLHAFileNameAndContent
    // capability (for CB_SLHA_file_model, CB_SLHA_simpmod_scan_model and CB_SLHA_scan_model)

    // @todo Should we perform some kind of SLHA1 vs SLHA2 check when used with the 
    //       CB_SLHA_* models below? For these models we currently just trust the user 
    //       to supply SLHA info in the appropriate format.

    // @todo Should we unify these two functions into a single module function that just
    //       provides a std::function instance that can be called with an 
    //       int argument = 1 or 2 and returns the appropriate SLHA1 or SLHA2 struct?

    // SLHA1
    void getSLHA1Spectrum(SLHAstruct& result)
    {
      using namespace Pipes::getSLHA1Spectrum;

      if(ModelInUse("MSSM63atQ") || ModelInUse("MSSM63atMGUT"))
      {
        result = Dep::MSSM_spectrum->getSLHAea(1);
      }
      else if (ModelInUse("CB_SLHA_file_model") || 
               ModelInUse("CB_SLHA_simpmod_scan_model") || 
               ModelInUse("CB_SLHA_scan_model"))
      {
        result = Dep::SLHAFileNameAndContent->second;
      }
      else
      {
        // This can only happen if the ALLOW_MODELS list in SUSY.hpp has been changed
        // without also changing this function
        std::stringstream errmsg_ss;
        errmsg_ss << "Unknown model! And that makes it a bit hard to return an SLHA1 spectrum... " 
                  << "Please expand the function getSLHA1Spectrum if you want to use it with for new models.!";
        ColliderBit_error().raise(LOCAL_INFO, errmsg_ss.str());
      }
    }

    // SLHA2
    void getSLHA2Spectrum(SLHAstruct& result)
    {
      using namespace Pipes::getSLHA2Spectrum;

      if(ModelInUse("MSSM63atQ") || ModelInUse("MSSM63atMGUT"))
      {
        result = Dep::MSSM_spectrum->getSLHAea(2);
      }
      else if (ModelInUse("CB_SLHA_file_model") || 
               ModelInUse("CB_SLHA_simpmod_scan_model") || 
               ModelInUse("CB_SLHA_scan_model"))
      {
        result = Dep::SLHAFileNameAndContent->second;
      }
      else
      {
        // This can only happen if the ALLOW_MODELS list in SUSY.hpp has been changed
        // without also changing this function
        std::stringstream errmsg_ss;
        errmsg_ss << "Unknown model! And that makes it a bit hard to return an SLHA1 spectrum... " 
                  << "Please expand the function getSLHA2Spectrum if you want to use it with for new models.!";
        ColliderBit_error().raise(LOCAL_INFO, errmsg_ss.str());
      }
    }



  }
}
