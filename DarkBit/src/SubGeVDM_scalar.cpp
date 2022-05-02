//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Implementation of SubGeVDM_scalar
///  DarkBit routines.
///
///  Authors (add name and date if you modify):    
///
///  \author Felix Kahlhoefer
///         (kahlhoefer@kit.edu)
///  \date May 2022
///                                                  
///  ********************************************* 

#include "boost/make_shared.hpp"

#include "gambit/Elements/gambit_module_headers.hpp"
#include "gambit/DarkBit/DarkBit_rollcall.hpp"
#include "gambit/Utils/ascii_table_reader.hpp"
#include "gambit/DarkBit/DarkBit_utils.hpp"

#include "gambit/Elements/gambit_module_headers.hpp"
#include "gambit/Elements/virtual_higgs.hpp"
#include "gambit/DarkBit/DarkBit_rollcall.hpp"
#include "gambit/Utils/ascii_table_reader.hpp"
#include "boost/make_shared.hpp"
#include "gambit/DarkBit/DarkBit_utils.hpp"

namespace Gambit
{

  namespace DarkBit
  {

    class SubGeVDM_scalar
    {
      public:
      /// Initialize SubGeVDM_scalar object (branching ratios etc)
      SubGeVDM_scalar() {};
      ~SubGeVDM_scalar() {};
            
    };

    void DarkMatter_ID_SubGeVDM_scalar(std::string & result) { result = "S"; }
    void DarkMatterConj_ID_SubGeVDM_scalar(std::string & result) { result = "S"; }

    /// Direct detection couplings
    void DD_couplings_SubGeVDM_scalar(DM_nucleon_couplings &result)
    {
      using namespace Pipes::DD_couplings_SubGeVDM_scalar;

      double mAp = *Param["mAp"];
      double effective_coupling = sqrt(4*pi*alpha_EM)*(*Param["gDM"])*(*Param["kappa"]);

      result.gps = effective_coupling/pow(mAp,2);
      result.gns = 0;
      result.gpa = 0;
      result.gna = 0;
    }


  }
}
