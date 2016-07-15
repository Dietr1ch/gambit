//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Central module file of DarkBit.  Calculates dark matter
///  related observables.
///
///  Most of the model- or observable-specific code is
///  stored in separate source files.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Torsten Bringmann
///          (torsten.bringmann@desy.de)
///  \date 2013 Jun
///  \date 2014 Mar
///
///  \author Christoph Weniger
///          (c.weniger@uva.nl)
///  \date 2013 Jul - 2015 May
///
///  \author Lars A. Dal
///          (l.a.dal@fys.uio.no)
///  \date 2014 Mar, Jul, Sep, Oct
///
///  \author Christopher Savage
///          (chris@savage.name)
///  \date 2014 Oct
///  \date 2015 Jan, Feb
///
///  \author Pat Scott
///          (pscott@imperial.ac.uk)
///  \date 2014 Mar
///  \date 2015 Mar
///
///  *********************************************

#include "gambit/Elements/gambit_module_headers.hpp"
#include "gambit/DarkBit/DarkBit_rollcall.hpp"
#include "gambit/DarkBit/DarkBit_utils.hpp"

namespace Gambit {
  namespace DarkBit {

    //////////////////////////////////////////////////////////////////////////
    //
    //                 Simple DM property extractors
    //
    //////////////////////////////////////////////////////////////////////////

    /// Retrieve the DM mass in GeV for generic models (GeV)
    void mwimp_generic(double &result) {
      using namespace Pipes::mwimp_generic;
      result = Dep::TH_ProcessCatalog->getParticleProperty(*Dep::DarkMatter_ID).mass;
    }

    /*! \brief Retrieve the total thermally-averaged annihilation cross-section
     * for indirect detection (cm^3 / s).
     */
    void sigmav_late_universe(double &result)
    {
      using namespace Pipes::sigmav_late_universe;
      std::string DMid = *Dep::DarkMatter_ID;
      TH_Process annProc = Dep::TH_ProcessCatalog->getProcess(DMid, DMid);
      result = 0.0;
      // Add all the regular channels
      for (std::vector<TH_Channel>::iterator it = annProc.channelList.begin();
          it != annProc.channelList.end(); ++it)
      {
        if ( it->nFinalStates == 2 )
        {
          // (sv)(v=0) for two-body final state
          result += it->genRate->bind("v")->eval(0.);
        }
      }
      // Add invisible contributions
      result += annProc.genRateMisc->bind("v")->eval(0.);
    }


    //////////////////////////////////////////////////////////////////////////
    //
    //                          DarkBit Unit Test
    //
    //////////////////////////////////////////////////////////////////////////

    /*! \brief Central unit test routine.
     *
     * Dumps various DM related results into yaml files for later inspection.
     */
    void UnitTest_DarkBit(int &result)
    {
      using namespace Pipes::UnitTest_DarkBit;
      using DarkBit_utils::gamma3bdy_limits;
      /* This function depends on all relevant DM observables (indirect and
       * direct) and dumps them into convenient files in YAML format, which
       * afterwards can be checked against the expectations.
       */

      double M_DM =
        Dep::TH_ProcessCatalog->getParticleProperty(*Dep::DarkMatter_ID).mass;
      double Gps = (*Dep::DD_couplings).gps;
      double Gpa = (*Dep::DD_couplings).gpa;
      double Gns = (*Dep::DD_couplings).gns;
      double Gna = (*Dep::DD_couplings).gna;
      double oh2 = *Dep::RD_oh2;

      std::string DMid = *Dep::DarkMatter_ID;
      TH_Process annProc = (*Dep::TH_ProcessCatalog).getProcess(DMid, DMid);
      daFunk::Funk spectrum = (*Dep::GA_AnnYield)->set("v", 0.);

      std::ostringstream filename;
      /*
      static unsigned int counter = 0;
      filename << runOptions->getValueOrDef<std::string>("UnitTest_DarkBit",
          "fileroot");
      filename << "_" << counter << ".yml";
      counter++;
      */
      /// Option filename<std::string>: Output filename (default UnitTest.yaml)
      filename << runOptions->getValueOrDef<std::string>("UnitTest.yaml", "filename");

      std::ofstream os;
      os.open(filename.str());
      if(os)
      {
        // Standard output.
        os << "# Direct detection couplings\n";
        os << "DDcouplings:\n";
        os << "  gps: " << Gps << "\n";
        os << "  gpa: " << Gpa << "\n";
        os << "  gns: " << Gns << "\n";
        os << "  gna: " << Gna << "\n";
        os << "\n";
        os << "# Particle masses [GeV] \n";
        os << "ParticleMasses:\n";
        os << "  Mchi: " << M_DM << "\n";
        os << "\n";
        os << "# Relic density Omega h^2\n";
        os << "RelicDensity:\n";
        os << "  oh2: " << oh2 << "\n";
        os << "\n";

        // Output gamma-ray spectrum (grid be set in YAML file).
        double x_min =
          /// Option GA_AnnYield::Emin<double>: Minimum energy in GeV (default 0.1)
          runOptions->getValueOrDef<double>(0.1, "GA_AnnYield", "Emin");
        double x_max =
          /// Option GA_AnnYield::Emax<double>: Maximum energy in GeV (default 1e4)
          runOptions->getValueOrDef<double>(10000, "GA_AnnYield", "Emax");
          /// Option GA_AnnYield::nbins<int>: Number of energy bins (default 26)
        int n = runOptions->getValueOrDef<double>(26, "GA_AnnYield", "nbins");
        // from 0.1 to 500 GeV
        std::vector<double> x = daFunk::logspace(log10(x_min), log10(x_max), n);
        std::vector<double> y = spectrum->bind("E")->vect(x);
        os << "# Annihilation spectrum dNdE [1/GeV]\n";
        os << "GammaRaySpectrum:\n";
        os << "  E: [";
        for (std::vector<double>::iterator it = x.begin(); it != x.end(); it++)
          os << *it << ", ";
        os  << "]\n";
        os << "  dNdE: [";
        for (std::vector<double>::iterator it = y.begin(); it != y.end(); it++)
          os << *it << ", ";
        os  << "]\n";
        os << std::endl;

        os << "# Annihilation rates\n";
        os << "AnnihilationRates:\n";
        for (std::vector<TH_Channel>::iterator it = annProc.channelList.begin();
            it != annProc.channelList.end(); ++it)
        {
          os << "  ";
          for (std::vector<std::string>::iterator
              jt = it->finalStateIDs.begin(); jt!=it->finalStateIDs.end(); jt++)
          {
            os << *jt << "";
          }
          if (it->finalStateIDs.size() == 2)
            os << ": " << it->genRate->bind("v")->eval(0);
          if (it->finalStateIDs.size() == 3)
          {
            double m1 = (*Dep::TH_ProcessCatalog).getParticleProperty(
                it->finalStateIDs[1]).mass;
            double m2 = (*Dep::TH_ProcessCatalog).getParticleProperty(
                it->finalStateIDs[2]).mass;
            double mass = M_DM;
            daFunk::Funk E1_low =  daFunk::func(gamma3bdy_limits<0>, daFunk::var("E"), mass, m1, m2);
            daFunk::Funk E1_high =  daFunk::func(gamma3bdy_limits<1>, daFunk::var("E"), mass, m1, m2);
            daFunk::Funk dsigmavde = it->genRate->gsl_integration("E1", E1_low, E1_high)->set_epsrel(1e-3)->set("v", 0);
            auto xgrid = daFunk::logspace(-2, 3, 1000);
            auto ygrid = daFunk::logspace(-2, 3, 1000);
            for ( size_t i = 0; i<xgrid.size(); i++ )
            {
              ygrid[i] = dsigmavde->bind("E")->eval(xgrid[i]);
            }
            auto interp = daFunk::interp("E", xgrid, ygrid);
            double svTOT = interp->gsl_integration("E", 10., 20.)->set_epsabs(1e-3)->bind()->eval();
            os << ": " << svTOT;
          }
          os << "\n";
        }
        os << std::endl;
      }
      else
      {
        logger() << "Warning: outputfile not open for writing." << std::endl;
      }
      os.close();
      result = 0;
    }
  }
}
