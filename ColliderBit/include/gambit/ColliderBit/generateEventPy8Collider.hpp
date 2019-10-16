//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  ColliderBit event loop functions returning
///  collider Monte Carlo events.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Abram Krislock
///          (a.m.b.krislock@fys.uio.no)
///
///  \author Aldo Saavedra
///
///  \author Andy Buckley
///
///  \author Chris Rogan
///          (crogan@cern.ch)
///  \date 2014 Aug
///  \date 2015 May
///
///  \author Pat Scott
///          (p.scott@imperial.ac.uk)
///  \date 2015 Jul
///  \date 2018 Jan
///  \date 2019 Jan
///  \date 2019 May
///
///  \author Anders Kvellestad
///          (anders.kvellestad@fys.uio.no)
///  \date   2017 March
///  \date   2018 Jan
///  \date   2018 May
///
///  \author Tomas Gonzalo
///          (tomas.gonzalo@monash.edu)
///  \date 2019 Sep, Oct
///
///  *********************************************

#include "gambit/ColliderBit/ColliderBit_eventloop.hpp"
#include "gambit/ColliderBit/colliders/Pythia8/Py8EventConversions.hpp"

// #define COLLIDERBIT_DEBUG
#define DEBUG_PREFIX "DEBUG: OMP thread " << omp_get_thread_num() << ":  "

namespace Gambit
{

  namespace ColliderBit
  {

    /// Drop a HepMC file for the event
    #ifndef EXCLUDE_HEPMC
      template<typename PythiaT>
      void dropHepMCEventPy8Collider(const PythiaT* Pythia, const safe_ptr<Options>& runOptions)
      {
        // Write event to HepMC file
        static const bool drop_HepMC2_file = runOptions->getValueOrDef<bool>(false, "drop_HepMC2_file");
        static const bool drop_HepMC3_file = runOptions->getValueOrDef<bool>(false, "drop_HepMC3_file");
        if (drop_HepMC2_file or drop_HepMC3_file)
        {
          thread_local Pythia_default::Pythia8::GAMBIT_hepmc_writer hepmc_writer;
          thread_local bool first = true;

          if (first)
          {
            str filename = "GAMBIT_collider_events.omp_thread_";
            filename += std::to_string(omp_get_thread_num());
            filename += ".hepmc";
            hepmc_writer.init(filename, drop_HepMC2_file, drop_HepMC3_file);
            first = false;
          }

          if(drop_HepMC2_file)
            hepmc_writer.write_event_HepMC2(const_cast<PythiaT*>(Pythia));
          if(drop_HepMC3_file)
            hepmc_writer.write_event_HepMC3(const_cast<PythiaT*>(Pythia));
         
        }
      }
    #endif

    /// Generate a hard scattering event with Pythia
    template<typename PythiaT, typename EventT>
    void generateEventPy8Collider(EventT& event,
                                  const MCLoopInfo& RunMC,
                                  const Py8Collider<PythiaT,EventT>& HardScatteringSim,
                                  const int iteration,
                                  void(*wrapup)(),
                                  const safe_ptr<Options>& runOptions)
    {
      static int nFailedEvents;
      thread_local EventT pythia_event;

      // If the event loop has not been entered yet, reset the counter for the number of failed events
      if (iteration == BASE_INIT)
      {
        // Although BASE_INIT should never be called in parallel, we do this in omp_critical just in case.
        #pragma omp critical (pythia_event_failure)
        {
          nFailedEvents = 0;
        }
        return;
      }

      // If in any other special iteration, do nothing
      if (iteration < BASE_INIT) return;

      // Reset the Pythia and HEPUtils events
      pythia_event.clear();
      event.clear();

      // Attempt (possibly repeatedly) to generate an event
      while(nFailedEvents <= RunMC.current_maxFailedEvents())
      {
        try
        {
          HardScatteringSim.nextEvent(pythia_event);
          break;
        }
        catch (typename Py8Collider<PythiaT,EventT>::EventGenerationError& e)
        {
          #ifdef COLLIDERBIT_DEBUG
          cout << DEBUG_PREFIX << "Py8Collider::EventGenerationError caught in generateEventPy8Collider. Check the ColliderBit log for event details." << endl;
          #endif
          #pragma omp critical (pythia_event_failure)
          {
            // Update global counter
            nFailedEvents += 1;
            // Store Pythia event record in the logs
            std::stringstream ss;
            pythia_event.list(ss, 1);
            logger() << LogTags::debug << "Py8Collider::EventGenerationError error caught in generateEventPy8Collider. Pythia record for event that failed:\n" << ss.str() << EOM;
          }
        }
      }

      // Wrap up event loop if too many events fail.
      if(nFailedEvents > RunMC.current_maxFailedEvents())
      {
        if(RunMC.current_invalidate_failed_points())
        {
          piped_invalid_point.request("exceeded maxFailedEvents");
        }
        else
        {
          piped_warnings.request(LOCAL_INFO,"exceeded maxFailedEvents");
        }
        wrapup();
        return;
      }

      #ifndef EXCLUDE_HEPMC
        dropHepMCEventPy8Collider<PythiaT>(HardScatteringSim.pythia(), runOptions);
      #endif


    }

    /// Generate a hard scattering event with Pythia and convert it to HEPUtils::Event
    template<typename PythiaT, typename EventT>
    void generateEventPy8Collider(HEPUtils::Event& event,
                                  EventT &pythia_event,
                                  const Py8Collider<PythiaT,EventT>& HardScatteringSim,
                                  void(*wrapup)())
    {

      // Attempt to convert the Pythia event to a HEPUtils event
      try
      {
        if (HardScatteringSim.partonOnly)
          convertPartonEvent(pythia_event, event, HardScatteringSim.antiktR);
        else
          convertParticleEvent(pythia_event, event, HardScatteringSim.antiktR);
      }
      // No good.
      catch (Gambit::exception& e)
      {
        #ifdef COLLIDERBIT_DEBUG
          cout << DEBUG_PREFIX << "Gambit::exception caught during event conversion in generateEventPy8Collider. Check the ColliderBit log for details." << endl;
        #endif

        #pragma omp critical (event_conversion_error)
        {
          // Store Pythia event record in the logs
          std::stringstream ss;
          pythia_event.list(ss, 1);
          logger() << LogTags::debug << "Gambit::exception caught in generateEventPy8Collider. Pythia record for event that failed:\n" << ss.str() << EOM;
        }

        str errmsg = "Bad point: generateEventPy8Collider caught the following runtime error: ";
        errmsg    += e.what();
        piped_invalid_point.request(errmsg);
        wrapup();
        return;
      }


    }

    #ifndef EXCLUDE_HEPMC
      /// Generate a hard scattering event with Pythia and convert it to HepMC event
      template<typename PythiaT, typename EventT>
      void generateEventPy8Collider(HepMC3::GenEvent& event,
                                    EventT &pythia_event,
                                    const Py8Collider<PythiaT,EventT>& HardScatteringSim,
                                    const int iteration,
                                    void(*wrapup)(),
                                    const safe_ptr<Options>& runOptions)
      {
      }
    #endif


    /// Generate a hard scattering event with a specific Pythia,
    #define GET_PYTHIA_EVENT(NAME, PYTHIA_EVENT_TYPE)            \
    void NAME(PYTHIA_EVENT_TYPE &result)                         \
    {                                                            \
      using namespace Pipes::NAME;                               \
      generateEventPy8Collider(result, *Dep::RunMC,              \
       *Dep::HardScatteringSim, *Loop::iteration, Loop::wrapup,  \
       runOptions);                                              \
    }                                                            \
                                                                 \
    void CAT(NAME,_HEPUtils)(HEPUtils::Event& result)            \
    {                                                            \
      using namespace Pipes::CAT(NAME,_HEPUtils);                \
      generateEventPy8Collider(result,                           \
       *Dep::HardScatteringEvent, *Dep::HardScatteringSim,       \
       Loop::wrapup);                                            \
    }                                                            \
                                                                 \
    BOOST_PP_IIF(EXCLUDE_HEPMC,,                                 \
      void CAT(NAME,_HepMC)(HepMC3::GenEvent& result)            \
      {                                                          \
        using namespace Pipes::CAT(NAME,_HepMC);                 \
        generateEventPy8Collider(result,                         \
         *Dep::HardScatteringEvent, *Dep::HardScatteringSim,     \
         *Loop::iteration, Loop::wrapup,                         \
         runOptions);                                            \
      }                                                          \
    )


  }

}
