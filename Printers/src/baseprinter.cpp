//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Definitions for certain baseprinter functions
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Ben Farmer
///          (benjamin.farmer@fysik.su.se)
///  \date 2017 Feb
///
///  *********************************************

// Gambit
#include "gambit/Printers/baseprinter.hpp"
#include "gambit/Utils/util_functions.hpp"

// Boost
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
//#include <boost/preprocessor/punctuation/comma_if.hpp>

// Code!
namespace Gambit
{
  namespace Printers
  {
     /// Specialisations of type ID getters
     /// ID zero reserved for null ID
     #define GETTYPEID(r,data,i,elem) \
     template<> \
     std::size_t getTypeID<elem>(void) {return i+1;}
     BOOST_PP_SEQ_FOR_EACH_I(GETTYPEID, _, PRINTABLE_TYPES)
     #undef GETTYPEID

     /// For debugging; print to stdout all the typeIDs for all types.
     void printAllTypeIDs(void)
     {
        std::cout << "Types known to getTypeID<T>() function:" << std::endl;
        #define PRINTTYPEID(r,data,elem) \
          std::cout << "  Type: " << STRINGIFY(elem) << ", TypeID: " << getTypeID<elem>() << std::endl;
        BOOST_PP_SEQ_FOR_EACH(PRINTTYPEID, _, PRINTABLE_TYPES)
        #undef PRINTTYPEID
     }

     /// Retrieve and directly print data to new output
     bool BaseReader::retrieve_and_print(const std::string& in_label, const std::string& out_label, BaseBasePrinter& printer, const uint rank, const ulong pointID)
     {
        /// First need to get the type data for 'in_label', then call appropriate retrieve and print functions.
        /// I think there is no choice but to do this with a big switch.
        /// Also need to check if the type matches what the printer expects, and decide what to do in case
        /// of mismatch.
        bool valid = false; // Switch to true if value is successfully retrieved for this point
        #define TYPE_CASES(r,data,elem) \
        if( get_type(in_label) == getTypeID<elem>()) \
        { \
            elem buffer; \
            valid = retrieve(buffer, in_label, rank, pointID); \
            if(valid) \
            { \
               printer.print(buffer, out_label, rank, pointID); \
            } \
        } else
        BOOST_PP_SEQ_FOR_EACH(TYPE_CASES, _, PRINTABLE_TYPES)
        #undef TYPE_CASES
        {
          std::ostringstream err;                               
          err << "Did not recognise retrieved type for data label '"<<in_label<<"'! This may indicate a bug in the Reader class you are using, please report it."; 
          printer_error().raise(LOCAL_INFO,err.str());         
        }
        return valid;
     }

     /// Helper function for the ModelParameters '_retrieve' functions
     /// Parses a printer label and checks if it contains a single model parameter.
     /// "out" is a memory location to store the parameter name, if found.
     /// "labelroot" is a memory location to store the rest of the label (i.e. minus the parameter name)
     bool parse_label_for_ModelParameters(const std::string& fulllabel, const std::string& modelname, std::string& out, std::string& labelroot)
     {
        bool result = false;
        std::istringstream iss(fulllabel);
        std::string capability;
        std::string rest; 
        iss >> capability;
        iss >> rest;
        if(!iss)
        {
          // Weren't two elements to the label, so this can't be a match
          result = false; // failed to match
        }
        else
        {
          //capability is "#NormalDist_parameters", for example
          capability.erase(0,1); // cut off the first character (hash, in all potentially matching cases)
          if(Utils::startsWith(capability,modelname))
          {
             // Cut off the modelname which matched
             capability.erase(0,modelname.size());
             if(Utils::startsWith(capability,"_parameters"))
             {
                // Still good so far, check 'rest', should be something like @NormalDist::primary_parameters::mu
                rest.erase(0,1); // cut off the first character (@, in all potentially matching cases)
                std::vector<str> split_rest = Utils::delimiterSplit(rest, "::");
                if(split_rest[0]==modelname and split_rest.size()==3)
                {
                  // Ok! We have a match!
                  out = split_rest[2];
                  result = true;
                  // Get the rest of the full label
                  labelroot = fulllabel;
                  // Erase the "::parameter_name" part
                  labelroot.erase(fulllabel.size() - out.size() - 2, out.size()+2);
                } else { result = false; }
             } else { result = false; }
          } else { result = false; }        
        }
        return result;
     }

  }
}
