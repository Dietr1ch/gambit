//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Type definition header for module
///  ColliderBit.
///
///  Compile-time registration of type definitions
///  required for the rest of the code to
///  communicate with ColliderBit.
///
///  Add to this if you want to define a new type
///  for the functions in ColliderBit to return,
///  but you don't expect that type to be needed
///  by any other modules.
///
///  *********************************************
///
///  Authors (add name if you modify):
///
///  \author Abram Krislock
///          (a.m.b.krislock@fys.uio.no)
///
///  \author Pat Scott
///          (p.scott@imperial.ac.uk)
///  \date 2018 Jan
///  \date 2019 Jan, May
///
///  \author Tomas Gonzalo
///          (t.e.gonzalo@fys.uio.no)
///  \date 2018 Feb
///
///  *********************************************


#ifndef __ColliderBit_types_hpp__
#define __ColliderBit_types_hpp__

#include <vector>
#include <chrono>

#include "gambit/ColliderBit/MCLoopInfo.hpp"
#include "gambit/ColliderBit/MC_convergence.hpp"
#include "gambit/ColliderBit/xsec.hpp"
#include "gambit/ColliderBit/colliders/Pythia8/Py8Collider.hpp"
#include "gambit/ColliderBit/colliders/Pythia8/Py8Collider_typedefs.hpp"
#include "gambit/ColliderBit/detectors/BuckFast.hpp"
#include "gambit/ColliderBit/analyses/AnalysisContainer.hpp"
#include "gambit/ColliderBit/analyses/AnalysisData.hpp"

#include "gambit/ColliderBit/limits/ALEPHSleptonLimits.hpp"
#include "gambit/ColliderBit/limits/L3GauginoLimits.hpp"
#include "gambit/ColliderBit/limits/L3SleptonLimits.hpp"
#include "gambit/ColliderBit/limits/OPALGauginoLimits.hpp"
#include "gambit/ColliderBit/limits/OPALDegenerateCharginoLimits.hpp"
/// TODO: see if we can use this one:
//#include "gambit/ColliderBit/limits/L3SmallDeltaMGauginoLimits.hpp"

#include "LHEF.h"
#include "HEPUtils/Event.h"

namespace Gambit
{

  namespace ColliderBit
  {

    /// @brief Container for data from multiple analyses and SRs
    typedef std::vector<AnalysisData> AnalysisNumbers;
    typedef std::vector<const AnalysisData*> AnalysisDataPointers;

    /// @brief Container for loglike information for an analysis
    struct AnalysisLogLikes
    {
      std::map<std::string,int> sr_indices;  // Signed indices so that we can use negative values for special cases
      std::map<std::string,double> sr_loglikes;

      std::string combination_sr_label;
      int combination_sr_index;
      double combination_loglike;

      AnalysisLogLikes() :
        combination_sr_label("undefined"),
        combination_sr_index(-2),
        combination_loglike(0.0)
        { }
    };

    /// @brief Typedef for a string-to-AnalysisLogLikes map
    typedef std::map<std::string,AnalysisLogLikes> map_str_AnalysisLogLikes;

    /// @brief Container for multiple analysis containers
    typedef std::vector<AnalysisContainer> AnalysisContainers;

    typedef std::chrono::milliseconds ms;
    typedef std::chrono::steady_clock steady_clock;
    typedef std::chrono::steady_clock::time_point tp;
    typedef std::map<std::string,double> timer_map_type;

    /// @brief Typedef for a str-SLHAstruct pair, to pass around SLHA filenames + content
    typedef std::pair<std::string,SLHAstruct> pair_str_SLHAstruct;

    /// @brief Typedefs related to cross-sections 
    // Note: 
    // The 'iipair' name below refers to the typedef std::pair<int,int> iipair 
    // declared in Utils/include/gambit/Utils/util_types.hpp

    typedef std::vector<std::pair<int,int>> vec_iipair;
    typedef std::multimap<int,std::pair<int,int>> multimap_int_iipair;
    typedef std::map<int,xsec_container> map_int_xsec;
    typedef std::map<int,process_xsec_container> map_int_process_xsec;
    // _Anders: Update this
    typedef std::map<std::pair<int,int>,xsec_container> map_iipair_PID_pair_xsec;
    // typedef std::map<std::pair<int,int>,PID_pair_xsec_container> map_iipair_PID_pair_xsec;

    /// @brief Typedef for a std::function that takes a PID pair (iipair) as input and returns an xsec_container instance
    /// _Anders: Not sure we'll neet this now that we're giving up the PIDPairCrossSectionFunc capability...
    typedef std::function<xsec_container(std::pair<int,int>)> PIDPairCrossSectionFuncType;

    /// @brief Typedef for a std::function that sets the weight for the input HEPUtils::Event
    typedef HEPUtils::Event HEPUtils_Event;  // Extra typedef to avoid macro problem with namespaces
    typedef std::function<void(HEPUtils_Event&, const BaseCollider*)> EventWeighterType_Py8Collider;
  }
}



#endif /* defined __ColliderBit_types_hpp__ */
