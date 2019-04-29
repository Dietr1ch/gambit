//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Module functions for computing cross-sections
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Pat Scott
///          (p.scott@imperial.ac.uk)
///  \date 2019 Feb
///
///  *********************************************

#include "gambit/ColliderBit/ColliderBit_eventloop.hpp"

//#define COLLIDERBIT_DEBUG

namespace Gambit
{

  namespace ColliderBit
  {

    /// Compute a cross-section from Monte Carlo
    void getMCxsec(xsec& result)
    {
      using namespace Pipes::getMCxsec;

      // Don't bother if there are no analyses that will use this.
      if (Dep::RunMC->analyses.empty()) return;

      // Reset the xsec objects on all threads
      if (*Loop::iteration == START_SUBPROCESS)
      {
        result.reset();
      }

      // If we are in the main event loop, count the event towards cross-section normalisation on this thread
      if (*Loop::iteration > 0)
      {
        result.log_event();
      }

      // Extract the xsecs from the MC on each thread
      if (*Loop::iteration == END_SUBPROCESS && Dep::RunMC->event_generation_began)
      {
        if (not Dep::RunMC->exceeded_maxFailedEvents)
        {
          const double xs_fb = (*Dep::HardScatteringSim)->xsec_pb() * 1000.;
          const double xserr_fb = (*Dep::HardScatteringSim)->xsecErr_pb() * 1000.;
          result.set_xsec(xs_fb, xserr_fb);
          #ifdef COLLIDERBIT_DEBUG
            cout << debug_prefix() << "xs_fb = " << xs_fb << " +/- " << xserr_fb << endl;
          #endif
        }
      }

      // Gather the xsecs from all threads into one
      if (*Loop::iteration == COLLIDER_FINALIZE)
      {
        result.gather_xsecs();
      }

    }

    /// Get a cross-section from NLL-FAST
    void getNLLFastxsec(xsec& result)
    {
      using namespace Pipes::getNLLFastxsec;

      // Don't bother if there are no analyses that will use this.
      if (Dep::RunMC->analyses.empty()) return;

      // Reset the xsec object on the main thread (other threads do not matter)
      if (*Loop::iteration == COLLIDER_INIT)
      {
        result.reset();
      }

      // If we are in the main event loop, count the event towards cross-section normalisation on this thread
      if (*Loop::iteration > 0)
      {
        result.log_event();
      }

      // Set the xsec and its error
      if (*Loop::iteration == COLLIDER_FINALIZE)
      {
        double xs_fb = 0.1;             // replace with xsec from NLL-Fast
        double xserr_fb = 0.1 * xs_fb;  // or whatever
        result.set_xsec(xs_fb, xserr_fb);
      }

    }


    /// Get a cross-section from Prospino
    void getProspinoxsec(xsec& result)
    {
      using namespace Pipes::getProspinoxsec;

      // // Don't bother if there are no analyses that will use this.
      // if (Dep::RunMC->analyses.empty()) return;

      cout << "DEBUG: getProspinoxsec: Loop iteration:" << *Loop::iteration << endl;

      // Reset the xsec object on the main thread (other threads do not matter)
      if (*Loop::iteration == BASE_INIT)
      {
        result.reset();

        // Testing...
        cout << "DEBUG: getProspinoxsec: Requesting dependency BE::prospino_LHC_xsec..." << endl;

        // Rather call a BE convenience function, which itself calls Prospino...
        // @todo pass in a list of processes to SPheno?

        // const Spectrum& spectrum = *Dep::MSSM_spectrum;

        // Get an SLHA1 object for Prospino.
        const SLHAstruct& slha = Dep::MSSM_spectrum->getSLHAea(1);

        // Get the GAMBIT model parameters for Prospino
        const param_map_type& model_params = Param;

        // std::vector<double> xsec_vals = BEreq::prospino_LHC_xsec(*Dep::MSSM_spectrum);
        // std::vector<double> xsec_vals = BEreq::prospino_LHC_xsec(std::move(slha));
        std::vector<double> xsec_vals = BEreq::prospino_LHC_xsec(slha, model_params);

        result.set_xsec(xsec_vals.at(0), xsec_vals.at(1));
      }

      // If we are in the main event loop, count the event towards cross-section normalisation on this thread
      if (*Loop::iteration > 0)
      {
        result.log_event();
      }

    }



  }
}
