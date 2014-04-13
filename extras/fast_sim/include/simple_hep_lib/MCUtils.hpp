#pragma once

// Just convenience includes
#include "simple_hep_lib/MathUtils.hpp"
#include "simple_hep_lib/Vectors.hpp"
#include "simple_hep_lib/Event.hpp"
#include "simple_hep_lib/Particle.hpp"
//#include "FastJetUtils.hpp"
//#include "Py8Utils.hpp"


namespace hep_simple_lib {


    /// @name PID stuff
    /// @todo Move to PIDUtils
    //@{

    template<size_t D> unsigned int digit(unsigned int val)    { return digit<D-1>(val/10); }
    template<>         unsigned int digit<1>(unsigned int val) { return val % 10; }

    inline bool hasQuark(unsigned int qid, int pid) {
      const unsigned int apid = std::abs(pid);
      if (apid == qid) return true;
      if (apid < 100 || std::abs(pid) > 10000) return false;
      return digit<2>(apid) == qid || digit<3>(apid) == qid || digit<4>(apid) == qid;
    }

    inline bool hasCharm(int pid) { return hasQuark(4, pid); }
    inline bool hasBottom(int pid) { return hasQuark(5, pid); }

    //@}


}
