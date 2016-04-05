//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Routines for direct detection couplings and 
///  likelihoods.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///   
///  \author Christopher Savage
///          (chris@savage.name)
///  \date 2014 Oct
///  \date 2015 Jan, Feb, June
///  
///  \author Jonathan Cornell
///          (jcornell@ucsc.edu)
///  \date 2015 Mar
///
///  *********************************************

#include "gambit/Elements/gambit_module_headers.hpp"
#include "gambit/DarkBit/DarkBit_rollcall.hpp"

namespace Gambit {
  namespace DarkBit {

    //////////////////////////////////////////////////////////////////////////
    //
    //                 Direct detection couplings
    //
    //////////////////////////////////////////////////////////////////////////

    /*! \brief Get direct detection couplings from initialized DarkSUSY.
    */
    void DD_couplings_DarkSUSY(DarkBit::DD_couplings &result)
    {
      using namespace Pipes::DD_couplings_DarkSUSY;

      double fG;

      // Set proton hadronic matrix elements
      (*BEreq::ddcom).ftp(7)  = *Param["fpu"];
      (*BEreq::ddcom).ftp(8)  = *Param["fpd"];
      (*BEreq::ddcom).ftp(10) = *Param["fps"];

      fG = 2./27.*(1. - *Param["fpu"] - *Param["fpd"] - *Param["fps"]);
      (*BEreq::ddcom).ftp(9) = fG;
      (*BEreq::ddcom).ftp(11) = fG;
      (*BEreq::ddcom).ftp(12) = fG;

      logger() << "DarkSUSY proton hadronic matrix elements set to:" << endl;
      logger() << "ftp(7) = fpu = " << (*BEreq::ddcom).ftp(7);
      logger() << "\tftp(8) = fpd = " << (*BEreq::ddcom).ftp(8);
      logger() << "\tftp(10) = fps = " << (*BEreq::ddcom).ftp(10) << endl;
      logger() << "ftp(9) = ftp(11) = ftp(12) = 2/27 fG = " << 
        (*BEreq::ddcom).ftp(9) << endl;

      // Set neutron hadronic matrix elements
      (*BEreq::ddcom).ftn(7)  = *Param["fnu"];
      (*BEreq::ddcom).ftn(8)  = *Param["fnd"];
      (*BEreq::ddcom).ftn(10) = *Param["fns"];

      fG = 2./27.*(1. - *Param["fnu"] - *Param["fnd"] - *Param["fns"]);
      (*BEreq::ddcom).ftn(9) = fG;
      (*BEreq::ddcom).ftn(11) = fG;
      (*BEreq::ddcom).ftn(12) = fG;

      logger() << "DarkSUSY neutron hadronic matrix elements set to:" << endl;
      logger() << "ftn(7) = fnu = " << (*BEreq::ddcom).ftn(7);
      logger() << "\tftn(8) = fnd = " << (*BEreq::ddcom).ftn(8);
      logger() << "\tftn(10) = fns = " << (*BEreq::ddcom).ftn(10) << endl;
      logger() << "ftn(9) = ftn(11) = ftn(12) = 2/27 fG = " << 
        (*BEreq::ddcom).ftn(9) << endl;

      // Set deltaq
      (*BEreq::ddcom).delu = *Param["deltau"];
      (*BEreq::ddcom).deld = *Param["deltad"];
      (*BEreq::ddcom).dels = *Param["deltas"];
      logger() << "DarkSUSY delta q set to:" << endl;
      logger() << "delu = delta u = " << (*BEreq::ddcom).delu;
      logger() << "\tdeld = delta d = " << (*BEreq::ddcom).deld;
      logger() << "\tdels = delta s = " << (*BEreq::ddcom).dels << endl;

      if (*Dep::DarkSUSY_PointInit) {
        // Calling DarkSUSY subroutine dsddgpgn(gps,gns,gpa,gna)
        // to set all four couplings.
        BEreq::dsddgpgn(result.gps, result.gns, result.gpa, result.gna);
        double factor = 
        /// Option rescale_couplings<double>: Rescaling factor for WIMP-nucleon couplings (default 1.)
          runOptions->getValueOrDef<double>(1., "rescale_couplings");
        result.gps *= factor;
        result.gns *= factor;
        result.gpa *= factor;
        result.gna *= factor;
        logger() << "DarkSUSY dsddgpgn gives:" << std::endl;
        logger() << " gps = " << result.gps << std::endl;
        logger() << " gns = " << result.gns << std::endl;
        logger() << " gpa = " << result.gpa << std::endl;
        logger() << " gna = " << result.gna << std::endl;
      } else {
        // Set couplings to zero if DarkSUSY point initialization
        // was not successful
        result.gps = 0.0; result.gns = 0.0;
        result.gpa = 0.0; result.gna = 0.0;
        logger() << "DarkSUSY point initialization failed:" << std::endl;
        logger() << " couplings set to zero." << std::endl;
      }
    }

    /*! \brief Get direct detection couplings from initialized MicrOmegas.
    */
    void DD_couplings_MicrOmegas(DarkBit::DD_couplings &result)
    {
      using namespace Pipes::DD_couplings_MicrOmegas;

      // Set proton hadronic matrix elements.
      (*BEreq::MOcommon).par[2] = *Param["fpd"];
      (*BEreq::MOcommon).par[3] = *Param["fpu"];
      (*BEreq::MOcommon).par[4] = *Param["fps"];

      logger() << "micrOMEGAs proton hadronic matrix elements set to:" << endl;
      logger() << "ScalarFFPd = fpd = " << (*BEreq::MOcommon).par[2];
      logger() << "\tScalarFFPu = fpu = " << (*BEreq::MOcommon).par[3];
      logger() << "\tScalarFFPs = fps = " << (*BEreq::MOcommon).par[4] << endl;

      // Set neutron hadronic matrix elements.
      (*BEreq::MOcommon).par[11] = *Param["fnd"];
      (*BEreq::MOcommon).par[12] = *Param["fnu"];
      (*BEreq::MOcommon).par[13] = *Param["fns"];

      logger() << "micrOMEGAs neutron hadronic matrix elements set to:" << endl;
      logger() << "ScalarFFNd = fnd = " << (*BEreq::MOcommon).par[11];
      logger() << "\tScalarFFNu = fnu = " << (*BEreq::MOcommon).par[12];
      logger() << "\tScalarFFNs = fns = " << (*BEreq::MOcommon).par[13] << endl;

      //Set delta q.
      (*BEreq::MOcommon).par[5] = *Param["deltad"];
      (*BEreq::MOcommon).par[6] = *Param["deltau"];
      (*BEreq::MOcommon).par[7] = *Param["deltas"];

      (*BEreq::MOcommon).par[14] = *Param["deltau"];
      (*BEreq::MOcommon).par[15] = *Param["deltad"];
      (*BEreq::MOcommon).par[16] = *Param["deltas"];

      logger() << "micrOMEGAs delta q set to:" << endl;
      logger() << "pVectorFFPd = pVectorFFNu = delta d = "
        << (*BEreq::MOcommon).par[5] << endl;
      logger() << "pVectorFFPu = pVectorFFPd = delta u = "
        << (*BEreq::MOcommon).par[6] << endl;
      logger() << "pVectorFFPs = pVectorFFNs = delta s = "
        << (*BEreq::MOcommon).par[7] << endl;

      double p1[2], p2[2], p3[2], p4[2];
      int error = BEreq::nucleonAmplitudes(byVal(BEreq::FeScLoop.pointer()), 
          byVal(p1), byVal(p2), byVal(p3), byVal(p4));
      if(error!=0)
        DarkBit_error().raise(LOCAL_INFO, 
            "micrOMEGAs nucleonAmplitudes function failed with "
            "error code " + std::to_string(error) + ".");

      // Rescaling to agree with DarkSUSY convention:
      result.gps = p1[0]*2;
      result.gpa = p2[0]*2;
      result.gns = p3[0]*2;
      result.gna = p4[0]*2;

      logger() << "micrOMEGAs nucleonAmplitudes gives:" << endl;
      logger() << " gps: " << result.gps << endl;
      logger() << " gns: " << result.gns << endl;
      logger() << " gpa: " << result.gpa << endl;
      logger() << " gna: " << result.gna << endl;
    }

    /// Simple calculator of the spin-independent WIMP-proton cross-section 
    void sigma_SI_p_simple(double &result)
    {
      using namespace Pipes::sigma_SI_p_simple;
      double gps = Dep::DD_couplings->gps;
      double reduced_mass = *Dep::mwimp * m_proton / (*Dep::mwimp + m_proton);
      result = gev2cm2/pi*pow(reduced_mass*gps,2.0);
    }

    /// Simple calculator of the spin-independent WIMP-neutron cross-section 
    void sigma_SI_n_simple(double &result)
    {
      using namespace Pipes::sigma_SI_n_simple;
      double gns = Dep::DD_couplings->gns;
      double reduced_mass = *Dep::mwimp * m_neutron / (*Dep::mwimp + m_neutron);
      result = gev2cm2/pi*pow(reduced_mass*gns,2.0);
    }

    /// Simple calculator of the spin-dependent WIMP-proton cross-section 
    void sigma_SD_p_simple(double &result)
    {
      using namespace Pipes::sigma_SD_p_simple;
      double gpa = Dep::DD_couplings->gpa;
      double reduced_mass = *Dep::mwimp * m_proton / (*Dep::mwimp + m_proton);
      result = 3.0*gev2cm2/pi*pow(reduced_mass*gpa,2.0);
    }

    /// Simple calculator of the spin-dependent WIMP-neutron cross-section 
    void sigma_SD_n_simple(double &result)
    {
      using namespace Pipes::sigma_SD_n_simple;
      double gna = Dep::DD_couplings->gna;
      double reduced_mass = *Dep::mwimp * m_neutron / (*Dep::mwimp + m_neutron);
      result = 3.0*gev2cm2/pi*pow(reduced_mass*gna,2.0);
    }


    //////////////////////////////////////////////////////////////////////////
    //
    //          Direct detection DDCalc0 intermediate routines
    //
    //////////////////////////////////////////////////////////////////////////

    /// Point-level initialization of DDCalc backend.
    void SetWIMP_DDCalc0(bool &result) {
      using namespace Pipes::SetWIMP_DDCalc0;
      double M = 
        Dep::TH_ProcessCatalog->getParticleProperty(*Dep::DarkMatter_ID).mass; 
      double GpSI = (*Dep::DD_couplings).gps;
      double GnSI = (*Dep::DD_couplings).gns;
      double GpSD = (*Dep::DD_couplings).gpa;
      double GnSD = (*Dep::DD_couplings).gna;                        
      BEreq::DDCalc0_SetWIMP_mG(&M,&GpSI,&GnSI,&GpSD,&GnSD);
      result = true;
      // Print out WIMP-nucleon cross-sections.
      // This part is optional as WIMP is already set.
      double sigmapSI,sigmanSI,sigmapSD,sigmanSD;
      BEreq::DDCalc0_GetWIMP_msigma(&M,&sigmapSI,&sigmanSI,&sigmapSD,&sigmanSD);
      logger() << "DDCalc0 WIMP-nucleon cross-sections [pb]:" << std::endl;
      logger() << "  sigmapSI = " << sigmapSI << std::endl;
      logger() << "  sigmanSI = " << sigmanSI << std::endl;
      logger() << "  sigmapSD = " << sigmapSD << std::endl;
      logger() << "  sigmanSD = " << sigmanSD << std::endl;
    }

    /// Performs DDCalc0 internal rate calculations for the XENON100 2012 result.
    void CalcRates_XENON100_2012_DDCalc0(bool &result) {
      using namespace Pipes::CalcRates_XENON100_2012_DDCalc0;
      BEreq::DDCalc0_XENON100_2012_CalcRates();
      result = true;
    }

    /// Performs DDCalc0 internal rate calculations for the LUX 2013 result.
    void CalcRates_LUX_2013_DDCalc0(bool &result) {
      using namespace Pipes::CalcRates_LUX_2013_DDCalc0;
      BEreq::DDCalc0_LUX_2013_CalcRates();
      result = true;
    }

    /// Performs DDCalc0 internal rate calculations for the SuperCDMS 2014 result.
    void CalcRates_SuperCDMS_2014_DDCalc0(bool &result) {
      using namespace Pipes::CalcRates_SuperCDMS_2014_DDCalc0;
      BEreq::DDCalc0_SuperCDMS_2014_CalcRates();
      result = true;
    }

    /// Performs DDCalc0 internal rate calculations for the SIMPLE 2014 result.
    void CalcRates_SIMPLE_2014_DDCalc0(bool &result) {
      using namespace Pipes::CalcRates_SIMPLE_2014_DDCalc0;
      BEreq::DDCalc0_SIMPLE_2014_CalcRates();
      result = true;
    }

    /*! \brief Performs DDCalc0 internal rate calculations for the future
     *         argon-based DARWIN experiment.
     *
     * Estimated sensitivity (as of 2015) at the current model point.
     */
    void CalcRates_DARWIN_Ar_2015_DDCalc0(bool &result) {
      using namespace Pipes::CalcRates_DARWIN_Ar_2015_DDCalc0;
      BEreq::DDCalc0_DARWIN_Ar_2015_CalcRates();
      result = true;
    }

    /*! \brief Performs DDCalc0 internal rate calculations for the future
     *         xenon-based DARWIN experiment.  
     *
     * Estimated sensitivity (as of 2015) at the current model point.
     */
    void CalcRates_DARWIN_Xe_2015_DDCalc0(bool &result) {
      using namespace Pipes::CalcRates_DARWIN_Xe_2015_DDCalc0;
      BEreq::DDCalc0_DARWIN_Xe_2015_CalcRates();
      result = true;
    }


    //////////////////////////////////////////////////////////////////////////
    //
    //                Direct detection likelihoods/observables
    //
    //////////////////////////////////////////////////////////////////////////

    // XENON100 2012 -----------------------------------------------------
    // Aprile et al., PRL 109, 181301 (2013) [arxiv:1207.5988]

    /// Log-likelihood - XENON100 2012
    void XENON100_2012_LogLikelihood_DDCalc0(double &result) {
      using namespace Pipes::XENON100_2012_LogLikelihood_DDCalc0;
      result = BEreq::DDCalc0_XENON100_2012_LogLikelihood();
      logger() << "XENON100 2012 log-likelihood: " << result << std::endl;
    }

    /// Observed events (integer) - XENON100 2012
    void XENON100_2012_Events_DDCalc0(int &result) {
      using namespace Pipes::XENON100_2012_Events_DDCalc0;
      result = BEreq::DDCalc0_XENON100_2012_Events();
      logger() << "XENON100 2012 events: " << result << std::endl;
    }

    /// Background expectation - XENON100 2012
    void XENON100_2012_Background_DDCalc0(double &result) {
      using namespace Pipes::XENON100_2012_Background_DDCalc0;
      result = BEreq::DDCalc0_XENON100_2012_Background();
      logger() << "XENON100 2012 background: " << result << std::endl;
    }

    /// Signal expectation - XENON100 2012
    void XENON100_2012_Signal_DDCalc0(double &result) {
      using namespace Pipes::XENON100_2012_Signal_DDCalc0;
      result = BEreq::DDCalc0_XENON100_2012_Signal();
      logger() << "XENON100 2012 signal: " << result << std::endl;
    }

    /// Signal expectation (spin-independent) - XENON100 2012
    void XENON100_2012_SignalSI_DDCalc0(double &result) {
      using namespace Pipes::XENON100_2012_SignalSI_DDCalc0;
      result = BEreq::DDCalc0_XENON100_2012_SignalSI();
      logger() << "XENON100 2012 signal (SI): " << result << std::endl;
    }

    /// Signal expectation (spin-dependent) - XENON100 2012
    void XENON100_2012_SignalSD_DDCalc0(double &result) {
      using namespace Pipes::XENON100_2012_SignalSD_DDCalc0;
      result = BEreq::DDCalc0_XENON100_2012_SignalSD();
      logger() << "XENON100 2012 signal (SD): " << result << std::endl;
    }


    // LUX 2013 ----------------------------------------------------------
    // Akerib et al., PRL 112, 091303 (2014) [arxiv:1310.8214]

    /// Log-likelihood - LUX 2013
    void LUX_2013_LogLikelihood_DDCalc0(double &result) {
      using namespace Pipes::LUX_2013_LogLikelihood_DDCalc0;
      result = BEreq::DDCalc0_LUX_2013_LogLikelihood();
      logger() << "LUX 2013 log-likelihood: " << result << std::endl;
    }

    /// Observed events (integer) - LUX 2013
    void LUX_2013_Events_DDCalc0(int &result) {
      using namespace Pipes::LUX_2013_Events_DDCalc0;
      result = BEreq::DDCalc0_LUX_2013_Events();
      logger() << "LUX 2013 events: " << result << std::endl;
    }

    /// Background expectation - LUX 2013
    void LUX_2013_Background_DDCalc0(double &result) {
      using namespace Pipes::LUX_2013_Background_DDCalc0;
      result = BEreq::DDCalc0_LUX_2013_Background();
      logger() << "LUX 2013 background: " << result << std::endl;
    }

    /// Signal expectation - LUX 2013
    void LUX_2013_Signal_DDCalc0(double &result) {
      using namespace Pipes::LUX_2013_Signal_DDCalc0;
      result = BEreq::DDCalc0_LUX_2013_Signal();
      logger() << "LUX 2013 signal: " << result << std::endl;
    }

    /// Signal expectation (spin-independent) - LUX 2013
    void LUX_2013_SignalSI_DDCalc0(double &result) {
      using namespace Pipes::LUX_2013_SignalSI_DDCalc0;
      result = BEreq::DDCalc0_LUX_2013_SignalSI();
      logger() << "LUX 2013 signal (SI): " << result << std::endl;
    }

    /// Signal expectation (spin-dependent) - LUX 2013
    void LUX_2013_SignalSD_DDCalc0(double &result) {
      using namespace Pipes::LUX_2013_SignalSD_DDCalc0;
      result = BEreq::DDCalc0_LUX_2013_SignalSD();
      logger() << "LUX 2013 signal (SD): " << result << std::endl;
    }


    // SuperCDMS 2014 ----------------------------------------------------
    // Agnese et al., PRL 112, 241302 (2014) [arxiv:1402.7137]

    /// Log-likelihood - SuperCDMS 2014
    void SuperCDMS_2014_LogLikelihood_DDCalc0(double &result) {
      using namespace Pipes::SuperCDMS_2014_LogLikelihood_DDCalc0;
      result = BEreq::DDCalc0_SuperCDMS_2014_LogLikelihood();
      logger() << "SuperCDMS 2014 log-likelihood: " << result << std::endl;
    }

    /// Observed events (integer) - SuperCDMS 2014
    void SuperCDMS_2014_Events_DDCalc0(int &result) {
      using namespace Pipes::SuperCDMS_2014_Events_DDCalc0;
      result = BEreq::DDCalc0_SuperCDMS_2014_Events();
      logger() << "SuperCDMS 2014 events: " << result << std::endl;
    }

    /// Background expectation - SuperCDMS 2014
    void SuperCDMS_2014_Background_DDCalc0(double &result) {
      using namespace Pipes::SuperCDMS_2014_Background_DDCalc0;
      result = BEreq::DDCalc0_SuperCDMS_2014_Background();
      logger() << "SuperCDMS 2014 background: " << result << std::endl;
    }

    /// Signal expectation - SuperCDMS 2014
    void SuperCDMS_2014_Signal_DDCalc0(double &result) {
      using namespace Pipes::SuperCDMS_2014_Signal_DDCalc0;
      result = BEreq::DDCalc0_SuperCDMS_2014_Signal();
      logger() << "SuperCDMS 2014 signal: " << result << std::endl;
    }

    /// Signal expectation (spin-independent) - SuperCDMS 2014
    void SuperCDMS_2014_SignalSI_DDCalc0(double &result) {
      using namespace Pipes::SuperCDMS_2014_SignalSI_DDCalc0;
      result = BEreq::DDCalc0_SuperCDMS_2014_SignalSI();
      logger() << "SuperCDMS 2014 signal (SI): " << result << std::endl;
    }

    /// Signal expectation (spin-dependent) - SuperCDMS 2014
    void SuperCDMS_2014_SignalSD_DDCalc0(double &result) {
      using namespace Pipes::SuperCDMS_2014_SignalSD_DDCalc0;
      result = BEreq::DDCalc0_SuperCDMS_2014_SignalSD();
      logger() << "SuperCDMS 2014 signal (SD): " << result << std::endl;
    }


    // SIMPLE 2014 -------------------------------------------------------
    // Felizardo et al., PRD 89, 072013 (2014) [arxiv:1404.4309]

    /// Log-likelihood - SIMPLE 2014
    void SIMPLE_2014_LogLikelihood_DDCalc0(double &result) {
      using namespace Pipes::SIMPLE_2014_LogLikelihood_DDCalc0;
      result = BEreq::DDCalc0_SIMPLE_2014_LogLikelihood();
      logger() << "SIMPLE 2014 log-likelihood: " << result << std::endl;
    }

    /// Observed events (integer) - SIMPLE 2014
    void SIMPLE_2014_Events_DDCalc0(int &result) {
      using namespace Pipes::SIMPLE_2014_Events_DDCalc0;
      result = BEreq::DDCalc0_SIMPLE_2014_Events();
      logger() << "SIMPLE 2014 events: " << result << std::endl;
    }

    /// Background expectation - SIMPLE 2014
    void SIMPLE_2014_Background_DDCalc0(double &result) {
      using namespace Pipes::SIMPLE_2014_Background_DDCalc0;
      result = BEreq::DDCalc0_SIMPLE_2014_Background();
      logger() << "SIMPLE 2014 background: " << result << std::endl;
    }

    /// Signal expectation - SIMPLE 2014
    void SIMPLE_2014_Signal_DDCalc0(double &result) {
      using namespace Pipes::SIMPLE_2014_Signal_DDCalc0;
      result = BEreq::DDCalc0_SIMPLE_2014_Signal();
      logger() << "SIMPLE 2014 signal: " << result << std::endl;
    }

    /// Signal expectation (spin-independent) - SIMPLE 2014
    void SIMPLE_2014_SignalSI_DDCalc0(double &result) {
      using namespace Pipes::SIMPLE_2014_SignalSI_DDCalc0;
      result = BEreq::DDCalc0_SIMPLE_2014_SignalSI();
      logger() << "SIMPLE 2014 signal (SI): " << result << std::endl;
    }

    /// Signal expectation (spin-dependent) - SIMPLE 2014
    void SIMPLE_2014_SignalSD_DDCalc0(double &result) {
      using namespace Pipes::SIMPLE_2014_SignalSD_DDCalc0;
      result = BEreq::DDCalc0_SIMPLE_2014_SignalSD();
      logger() << "SIMPLE 2014 signal (SD): " << result << std::endl;
    }


    // DARWIN argon-based ------------------------------------------------
    // Estimated argon-based DARWIN sensitivity (as of 2015):
    //   Conrad et al., arxiv:15MM.NNNNN

    /// Log-likelihood - DARWIN
    void DARWIN_Ar_2015_LogLikelihood_DDCalc0(double &result) {
      using namespace Pipes::DARWIN_Ar_2015_LogLikelihood_DDCalc0;
      result = BEreq::DDCalc0_DARWIN_Ar_2015_LogLikelihood();
      logger() << "DARWIN argon-based (2015 estimate) log-likelihood: " 
        << result << std::endl;
    }

    /// Observed events (integer) - DARWIN
    void DARWIN_Ar_2015_Events_DDCalc0(int &result) {
      using namespace Pipes::DARWIN_Ar_2015_Events_DDCalc0;
      result = BEreq::DDCalc0_DARWIN_Ar_2015_Events();
      logger() << "DARWIN argon-based (2015 estimate) events: " 
        << result << std::endl;
    }

    /// Background expectation - DARWIN
    void DARWIN_Ar_2015_Background_DDCalc0(double &result) {
      using namespace Pipes::DARWIN_Ar_2015_Background_DDCalc0;
      result = BEreq::DDCalc0_DARWIN_Ar_2015_Background();
      logger() << "DARWIN argon-based (2015 estimate) background: " 
        << result << std::endl;
    }

    /// Signal expectation - DARWIN
    void DARWIN_Ar_2015_Signal_DDCalc0(double &result) {
      using namespace Pipes::DARWIN_Ar_2015_Signal_DDCalc0;
      result = BEreq::DDCalc0_DARWIN_Ar_2015_Signal();
      logger() << "DARWIN argon-based (2015 estimate) signal: " 
        << result << std::endl;
    }

    /// Signal expectation (spin-independent) - DARWIN
    void DARWIN_Ar_2015_SignalSI_DDCalc0(double &result) {
      using namespace Pipes::DARWIN_Ar_2015_SignalSI_DDCalc0;
      result = BEreq::DDCalc0_DARWIN_Ar_2015_SignalSI();
      logger() << "DARWIN argon-based (2015 estimate) signal (SI): " 
        << result << std::endl;
    }

    /// Signal expectation (spin-dependent) - DARWIN
    void DARWIN_Ar_2015_SignalSD_DDCalc0(double &result) {
      using namespace Pipes::DARWIN_Ar_2015_SignalSD_DDCalc0;
      result = BEreq::DDCalc0_DARWIN_Ar_2015_SignalSD();
      logger() << "DARWIN argon-based (2015 estimate) signal (SD): " 
        << result << std::endl;
    }


    // DARWIN xenon-based ------------------------------------------------
    // Estimated xenon-based DARWIN sensitivity (as of 2015):
    //   Conrad et al., arxiv:15MM.NNNNN

    /// Log-likelihood - DARWIN xenon-based
    void DARWIN_Xe_2015_LogLikelihood_DDCalc0(double &result) {
      using namespace Pipes::DARWIN_Xe_2015_LogLikelihood_DDCalc0;
      result = BEreq::DDCalc0_DARWIN_Xe_2015_LogLikelihood();
      logger() << "DARWIN xenon-based (2015 estimate) log-likelihood: " 
        << result << std::endl;
    }

    /// Observed events (integer) - DARWIN xenon-based
    void DARWIN_Xe_2015_Events_DDCalc0(int &result) {
      using namespace Pipes::DARWIN_Xe_2015_Events_DDCalc0;
      result = BEreq::DDCalc0_DARWIN_Xe_2015_Events();
      logger() << "DARWIN xenon-based (2015 estimate) events: " 
        << result << std::endl;
    }

    /// Background expectation - DARWIN xenon-based
    void DARWIN_Xe_2015_Background_DDCalc0(double &result) {
      using namespace Pipes::DARWIN_Xe_2015_Background_DDCalc0;
      result = BEreq::DDCalc0_DARWIN_Xe_2015_Background();
      logger() << "DARWIN xenon-based (2015 estimate) background: " 
        << result << std::endl;
    }

    /// Signal expectation - DARWIN xenon-based
    void DARWIN_Xe_2015_Signal_DDCalc0(double &result) {
      using namespace Pipes::DARWIN_Xe_2015_Signal_DDCalc0;
      result = BEreq::DDCalc0_DARWIN_Xe_2015_Signal();
      logger() << "DARWIN xenon-based (2015 estimate) signal: " 
        << result << std::endl;
    }

    /// Signal expectation (spin-independent) - DARWIN xenon-based
    void DARWIN_Xe_2015_SignalSI_DDCalc0(double &result) {
      using namespace Pipes::DARWIN_Xe_2015_SignalSI_DDCalc0;
      result = BEreq::DDCalc0_DARWIN_Xe_2015_SignalSI();
      logger() << "DARWIN xenon-based (2015 estimate) signal (SI): " 
        << result << std::endl;
    }

    /// Signal expectation (spin-dependent) - DARWIN xenon-based
    void DARWIN_Xe_2015_SignalSD_DDCalc0(double &result) {
      using namespace Pipes::DARWIN_Xe_2015_SignalSD_DDCalc0;
      result = BEreq::DDCalc0_DARWIN_Xe_2015_SignalSD();
      logger() << "DARWIN xenon-based (2015 estimate) signal (SD): " 
        << result << std::endl;
    }
  }
}
