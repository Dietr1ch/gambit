//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Relic density calculations.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Torsten Bringmann
///          (torsten.bringmann@desy.de)
///  \date 2013 Jun -- 2016 May
///
///  \author Christoph Weniger
///          (c.weniger@uva.nl)
///  \date 2013 Jul - 2015 May
///
///  *********************************************

#include <chrono>

#include "gambit/Elements/gambit_module_headers.hpp"
#include "gambit/DarkBit/DarkBit_rollcall.hpp"
#include "gambit/DarkBit/DarkBit_utils.hpp"
#include "gambit/Utils/util_functions.hpp"


namespace Gambit
{
  namespace DarkBit
  {

//#define DARKBIT_DEBUG
//#define DARKBIT_RD_DEBUG

    //////////////////////////////////////////////////////////////////////////
    //
    //                    DarkSUSY Relic density routines
    //
    //////////////////////////////////////////////////////////////////////////

    /*! \brief Collects spectrum information about coannihilating particles,
     *         resonances and threshold energies -- so far directly from DarkSUSY.
     */
    void RD_spectrum_SUSY(RD_spectrum_type &result)
    {
      using namespace Pipes::RD_spectrum_SUSY;

      std::vector<int> colist; //potential coannihilating particles (indices)
      colist.clear();

      result.coannihilatingParticles.clear();
      result.resonances.clear();
      result.threshold_energy.clear();

      /// Option CoannCharginosNeutralinos<bool>: Specify whether charginos and
      /// neutralinos are included in coannihilations (default: true)
      bool CoannCharginosNeutralinos = runOptions->getValueOrDef<bool>(true,
          "CoannCharginosNeutralinos");

      /// Option CoannSfermions<bool>: Specify whether sfermions are included in
      /// coannihilations (default: true)
      bool CoannSfermions = runOptions->getValueOrDef<bool>(true,
          "CoannSfermions");

      /// Option CoannMaxMass<double>: Maximal sparticle mass to be included in
      /// coannihilations, in units of DM mass (default: 1.6)
      double CoannMaxMass = runOptions->getValueOrDef<double>(1.6,
          "CoannMaxMass");

      // introduce pointers to DS mass spectrum and relevant particle info
      DS_PACODES *DSpart = &(*BEreq::pacodes);
      DS_MSPCTM *mymspctm= &(*BEreq::mspctm);
      DS_INTDOF *myintdof= &(*BEreq::intdof);

      // first add neutralino=WIMP=least massive 'coannihilating particle'
      result.coannihilatingParticles.push_back(
          RD_coannihilating_particle(DSpart->kn(1),
          myintdof->kdof(DSpart->kn(1)),mymspctm->mass(DSpart->kn(1))));

      #ifdef DARKBIT_DEBUG
        std::cout << "WIMP : "<< DSpart->kn(1) << " " <<
            myintdof->kdof(DSpart->kn(1)) << " " << mymspctm->mass(DSpart->kn(1))
            << std::endl;
      #endif

      // FIXME: eventually, this function should not be BE-dependent anymore
      // (i.e. SUSY particle conventions should follow GAMBUT, not DS etc)!
      // The use of any
      // DarkSUSY conventions need thus be moved to RD_annrate_DSprep_func


      // include  neutralino & chargino coannihilation
      if(CoannCharginosNeutralinos)
      {
        for (int i=2; i<=4; i++)
          colist.push_back(DSpart->kn(i));
        colist.push_back(DSpart->kcha(1));
        colist.push_back(DSpart->kcha(2));
      }

      // include sfermion coannihilation
      if(CoannSfermions)
      {
        for (int i=1; i<=6; i++)
          colist.push_back(DSpart->ksl(i));
        for (int i=1; i<=3; i++)
          colist.push_back(DSpart->ksnu(i));
        for (int i=1; i<=6; i++)
          colist.push_back(DSpart->ksqu(i));
        for (int i=1; i<=6; i++)
          colist.push_back(DSpart->ksqd(i));
      }

      // only keep sparticles that are not too heavy
      for (size_t i=0; i<colist.size(); i++)
        if (mymspctm->mass(colist[i])/mymspctm->mass(DSpart->kn(1))
            < CoannMaxMass)
          result.coannihilatingParticles.push_back(
              RD_coannihilating_particle(colist[i], myintdof->kdof(colist[i]),
              mymspctm->mass(colist[i])));


      // determine resonances for LSP annihilation
      int reslist[] = {BEreq::particle_code("Z0"),
                       BEreq::particle_code("h0_2"),
                       BEreq::particle_code("h0_1"),
                       BEreq::particle_code("A0"),
                       BEreq::particle_code("W+"),
                       BEreq::particle_code("H+")};
      int resmax=sizeof(reslist) / sizeof(reslist[0]);
      // the last 2 resonances in the list can only appear for coannihilations
      if (result.coannihilatingParticles.size() == 1)
        resmax -= 2;

//      for (int i=0; i<resmax; i++)
//      {
//        if (mymspctm->mass(reslist[i])/result.coannihilatingParticles[0].mass
//            > 2.)
//        {

/////////////////////////////
/////////////////////////////
// JONATHAN FIXME : please comment out everything and check whether this induces more
//           "dgdap ..." errros. Comment out from here
//          // DS-specific treatment of narrow Higgs width: is no longer needed here.
//          DS_WIDTHS *mywidths= &(*BEreq::widths);
//          if (reslist[i]==BEreq::particle_code("h0_2") && mywidths->width(BEreq::particle_code("h0_2")) < 0.1)
//            // wide res treatment adopted in DS
//            result.resonances.push_back(
//                TH_Resonance(mymspctm->mass(reslist[i]), 0.1));
//          else
//            result.resonances.push_back(
//                TH_Resonance(
//                  mymspctm->mass(reslist[i]), mywidths->width(reslist[i])));
//        }
//      }
// to here
/////////////////////////////
/////////////////////////////


      // determine thresholds; lowest threshold = 2*WIMP rest mass  (unlike DS
      // convention!)
      result.threshold_energy.push_back(
          2*result.coannihilatingParticles[0].mass);
      int thrlist[] = {BEreq::particle_code("W+"),
                       BEreq::particle_code("Z0"),
                       BEreq::particle_code("t")};
      int thrmax=sizeof(thrlist) / sizeof(thrlist[0]);
      for (int i=0; i<thrmax; i++)
        if (mymspctm->mass(thrlist[i])>result.coannihilatingParticles[0].mass)
          result.threshold_energy.push_back(2*mymspctm->mass(thrlist[i]));

    } // function RD_spectrum_SUSY


   /*! \brief Collects information about resonances and threshold energies
     *        directly from the ProcessCatalog
     *        [NB: this assumes no coannihilating particles!]
     */
    void RD_spectrum_from_ProcessCatalog(RD_spectrum_type &result)
    {
      using namespace Pipes::RD_spectrum_from_ProcessCatalog;

      // retrieve annihilation processes and DM properties
      std::string DMid= *Dep::DarkMatter_ID;
      TH_Process annihilation =
              (*Dep::TH_ProcessCatalog).getProcess(DMid, DMid);
      TH_ParticleProperty DMproperty =
              (*Dep::TH_ProcessCatalog).getParticleProperty(DMid);

      // get thresholds & resonances from process catalog
      result.resonances = annihilation.resonances_thresholds.resonances;
      result.threshold_energy = annihilation.resonances_thresholds.threshold_energy;

      result.coannihilatingParticles.clear();
      // add WIMP=least massive 'coannihilating particle'
      // NB: particle code (1st entry) is irrelevant (unless Weff is obtained from DS)
      result.coannihilatingParticles.push_back(
          RD_coannihilating_particle(100,1+DMproperty.spin2,DMproperty.mass));
      // FIXME: coannihilation thresholds have to be added once they are included
      // in the process catalog

      #ifdef DARKBIT_DEBUG
        std::cout << "DM dof = " << 1+ DMproperty.spin2 << std::endl;
        // std::cout << "Test : " << BEreq::particle_code("d_3")
        //           << " " << BEreq::particle_code("u_3") << std::endl;
      #endif


    } // function RD_spectrum_from_ProcessCatalog


    /*! \brief Order RD_spectrum object and derive coannihilation thresholds.
    */
    void RD_spectrum_ordered_func(RD_spectrum_type &result)
    {
      using namespace Pipes::RD_spectrum_ordered_func;

      result = *Dep::RD_spectrum;

      // NB: coannihilatingParticles does not necessarily have to be ordered,
      // but it is assumed that coannihilatingParticles[0] is the DM particle
      RD_coannihilating_particle tmp_co;
      if (result.coannihilatingParticles.size() > 1)
        for (std::size_t i=0; i<result.coannihilatingParticles.size()-1; i++)
        {
          for (std::size_t j=i+1; j<result.coannihilatingParticles.size(); j++)
          {
          if (result.coannihilatingParticles[j].mass<result.coannihilatingParticles[i].mass)
            {
              tmp_co=result.coannihilatingParticles[i];
              result.coannihilatingParticles[i]=result.coannihilatingParticles[j];
              result.coannihilatingParticles[j]=tmp_co;
            }
          }
        }


      // add coannihilation thresholds
      if (result.coannihilatingParticles.size() > 1)
        for (int i=0; i<(int)result.coannihilatingParticles.size(); i++)
          for (int j=std::max(1,i);
              j<(int)result.coannihilatingParticles.size(); j++)
            result.threshold_energy.push_back(
                result.coannihilatingParticles[i].mass
                +result.coannihilatingParticles[j].mass);
      //and order all thresholds
      double tmp;
      for (std::size_t i=0; i<result.threshold_energy.size()-1; i++)
      {
        for (std::size_t j=i+1; j<result.threshold_energy.size(); j++)
        {
          if (result.threshold_energy[j]<result.threshold_energy[i])
          {
            tmp=result.threshold_energy[i];
            result.threshold_energy[i]=result.threshold_energy[j];
            result.threshold_energy[j]=tmp;
          }
        }
      }

      if (!result.resonances.empty()){
        TH_Resonance tmp2;
        for (std::size_t i=0; i<result.resonances.size()-1; i++)
        {
          for (std::size_t j=i+1; j<result.resonances.size(); j++)
          {
            if (result.resonances[j].energy < result.resonances[i].energy)
            {
              tmp2=result.resonances[i];
              result.resonances[i]=result.resonances[j];
              result.resonances[j]=tmp2;
            }
          }
        }
      }
    } // function RD_spectrum_ordered_func


    /*! \brief Some helper function to prepare evaluation of Weff from
     *         DarkSUSY.
     */
    void RD_annrate_DSprep_func(int &result)
    {
      using namespace Pipes::RD_annrate_DSprep_func;

      // Read out location and number of resonances and thresholds from
      // RDspectrum.
      RD_spectrum_type specres = *Dep::RD_spectrum;

      // FIXME: Here goes a translation of GAMBIT particle identifiers
      // -> DS particle codes (once RD_spectrum_SUSY is backend independent)

      //write info about coannihilating particles to DS common blocks
      //[this is essentially the model-dependent part of dsrdstart]
      DS_RDMGEV myrdmgev;
      myrdmgev.nco = specres.coannihilatingParticles.size();
      for (int i=1; i<=myrdmgev.nco; i++) {
        myrdmgev.mco(i)=fabs(specres.coannihilatingParticles[i-1].mass);
        myrdmgev.mdof(i)=specres.coannihilatingParticles[i-1].degreesOfFreedom;
        myrdmgev.kcoann(i)=specres.coannihilatingParticles[i-1].index;
        // NB: only this particle code is DS/SUSY specific!
      }

      double tmp; int itmp;
      for (int i=1; i<=myrdmgev.nco-1; i++) {
        for (int j=i+1; j<=myrdmgev.nco; j++) {
          if (myrdmgev.mco(j)<myrdmgev.mco(i)) {
            tmp=myrdmgev.mco(i);
            myrdmgev.mco(i)=myrdmgev.mco(j);
            myrdmgev.mco(j)=tmp;
            itmp=myrdmgev.mdof(i);
            myrdmgev.mdof(i)=myrdmgev.mdof(j);
            myrdmgev.mdof(j)=itmp;
            itmp=myrdmgev.kcoann(i);
            myrdmgev.kcoann(i)=myrdmgev.kcoann(j);
            myrdmgev.kcoann(j)=itmp;
          }
        }
      #ifdef DARKBIT_RD_DEBUG
        std::cout << "co : "<< myrdmgev.kcoann(i) << " " <<
            myrdmgev.mco(i) << " " << myrdmgev.mdof(i)
            << std::endl;
      #endif
      }
      #ifdef DARKBIT_RD_DEBUG
        std::cout << "co : "<< myrdmgev.kcoann(myrdmgev.nco) << " " <<
            myrdmgev.mco(myrdmgev.nco) << " " << myrdmgev.mdof(myrdmgev.nco)
            << std::endl;
      #endif

      *BEreq::rdmgev = myrdmgev;

      result=1; // everything OK

    } // function RD_eff_annrate_DSprep_func


    /*! \brief Get Weff directly from initialized DarkSUSY.
    */
    void RD_eff_annrate_SUSY(double(*&result)(double&))
    {
      using namespace Pipes::RD_eff_annrate_SUSY;

      if (BEreq::dsanwx.origin() == "DarkSUSY")
      {
        result=BEreq::dsanwx.pointer();
      }
    } // function RD_eff_annrate_SUSY


    /*! \brief Infer Weff from process catalog.
    */
    // Carries pointer to Weff
    DEF_FUNKTRAIT(RD_EFF_ANNRATE_FROM_PROCESSCATALOG_TRAIT)
      void RD_eff_annrate_from_ProcessCatalog(double(*&result)(double&))
      {
        using namespace Pipes::RD_eff_annrate_from_ProcessCatalog;

        std::string DMid= *Dep::DarkMatter_ID;
        TH_Process annProc = (*Dep::TH_ProcessCatalog).getProcess(DMid, DMid);
        double mDM = (*Dep::TH_ProcessCatalog).getParticleProperty(DMid).mass;
        const double GeV2tocm3s1 = 1.16733e-17;

        auto Weff = daFunk::zero("peff");
        auto peff = daFunk::var("peff");
        auto s = 4*(peff*peff + mDM*mDM);

        for (std::vector<TH_Channel>::iterator it = annProc.channelList.begin();
            it != annProc.channelList.end(); ++it)
        {
          Weff = Weff +
            it->genRate->set("v", 2*peff/sqrt(mDM*mDM+peff*peff))*s/GeV2tocm3s1;
        }
        // Add genRateMisc to Weff
        Weff = Weff + annProc.genRateMisc->set("v", 2*peff/sqrt(mDM*mDM+peff*peff))*s/GeV2tocm3s1;
        if ( Weff->getNArgs() != 1 )
          DarkBit_error().raise(LOCAL_INFO,
              "RD_eff_annrate_from_ProcessCatalog: Wrong number of arguments.\n"
              "The probable cause are three-body final states, which are not supported for this function."
              );
        result = Weff->plain<RD_EFF_ANNRATE_FROM_PROCESSCATALOG_TRAIT>("peff");
      } // function RD_eff_annrate_from_ProcessCatalog


    /*! \brief General routine for calculation of relic density, using DarkSUSY
     *         Boltzmann solver
     *
     *  Requires:
     *  - RD_thresholds_resonances
     *  - RD_eff_annrate (Weff)
     */
    void RD_oh2_general(double &result)
    {
      using namespace Pipes::RD_oh2_general;

      // Retrieve ordered list of resonances and thresholds from
      // RD_thresholds_resonances.
      RD_spectrum_type myRDspec = *Dep::RD_spectrum_ordered;
      if (myRDspec.coannihilatingParticles.empty())
      {
        DarkBit_error().raise(LOCAL_INFO, "RD_oh2_general: No DM particle!");
      }
      double mwimp=myRDspec.coannihilatingParticles[0].mass;

      #ifdef DARKBIT_RD_DEBUG
        bool tbtest=false;
      #endif

      // What follows below is the standard accurate calculation of oh2 in DS
      // either in fast = 0 (slower, accuracy <1%)  or fast = 1 (faster, default) mode

      // the following replaces dsrdcom -- which cannot be linked properly!?
      DS_RDPARS myrdpars;

      /// Option fast<int>: Numerical performance of Boltzmann solver in DS
      /// (default: 1) [NB: accurate is fast = 0 !]
      int fast = runOptions->getValueOrDef<int>(1, "fast");
      switch (fast)
      {
        case 0:
          myrdpars.cosmin=0.996195;myrdpars.waccd=0.005;myrdpars.dpminr=1.0e-4;
          myrdpars.dpthr=5.0e-4;myrdpars.wdiffr=0.05;myrdpars.wdifft=0.02;
          break;
        case 1:
          myrdpars.cosmin=0.996195;myrdpars.waccd=0.05;myrdpars.dpminr=5.0e-4;
          myrdpars.dpthr=2.5e-3;myrdpars.wdiffr=0.5;myrdpars.wdifft=0.1;
          break;
        default:
          DarkBit_error().raise(LOCAL_INFO, "Invalid fast flag (should be 0 or 1)");
      }

      myrdpars.hstep=0.01;myrdpars.hmin=1.0e-9;myrdpars.compeps=0.01;
      myrdpars.xinit=2.0;myrdpars.xfinal=200.0;myrdpars.umax=10.0;
      myrdpars.cfr=0.5;
      *BEreq::rdpars = myrdpars;
      DS_RDSWITCH myrdswitch;
      myrdswitch.thavint=1;myrdswitch.rdprt=0;
      *BEreq::rdswitch = myrdswitch;
      DS_RDLUN myrdlun;
      myrdlun.rdlulog=6;myrdlun.rdluerr=6;
      *BEreq::rdlun = myrdlun;
      DS_RDPADD myrdpadd;
      myrdpadd.pdivr=2.0;myrdpadd.dpres=0.5;myrdpadd.nlow=20;
      myrdpadd.nhigh=10;
      myrdpadd.npres=4;myrdpadd.nthup=4;myrdpadd.cthtest=0;myrdpadd.spltest=1;
      *BEreq::rdpadd = myrdpadd;

      DS_RDERRORS myrderrors;
      myrderrors.rderr=0;myrderrors.rdwar=0;myrderrors.rdinit=1234;
      *BEreq::rderrors = myrderrors;


      // write mass and dof of DM & coannihilating particle to DS common blocks
      DS_RDMGEV *myrdmgev = &(*BEreq::rdmgev);

      myrdmgev->nco=myRDspec.coannihilatingParticles.size();
      for (std::size_t i=1; i<=((unsigned int)myrdmgev->nco); i++)
      {
        myrdmgev->mco(i)=myRDspec.coannihilatingParticles[i-1].mass;
        myrdmgev->mdof(i)=myRDspec.coannihilatingParticles[i-1].degreesOfFreedom;
        myrdmgev->kcoann(i)=myRDspec.coannihilatingParticles[i-1].index;
        #ifdef DARKBIT_RD_DEBUG
          std::cout << "kcoann, mco, mdof: " << myrdmgev->kcoann(i) << "  " << myrdmgev->mco(i) << "  " << myrdmgev->mdof(i) << std::endl;
        #endif
      }

      // write information about thresholds and resonances to DS common blocks
      // [this is the model-independent part of dsrdstart]
      myrdmgev->nres=0;
      if (!myRDspec.resonances.empty())
      {
        myrdmgev->nres=myRDspec.resonances.size();
        for (std::size_t i=1; i<=myRDspec.resonances.size(); i++)
        {
          myrdmgev->rgev(i)=myRDspec.resonances[i-1].energy;
          myrdmgev->rwid(i)=myRDspec.resonances[i-1].width;
          #ifdef DARKBIT_RD_DEBUG
            std::cout << "rgev, rwid: " << myrdmgev->rgev(i) << "  " << myrdmgev->rwid(i) << std::endl;
          #endif
        }
      }
      // convert to momenta and write to DS common blocks
      DS_RDPTH myrdpth;
      // NB: DS does not count 2* WIMP rest mass as thr
      myrdpth.nth=myRDspec.threshold_energy.size()-1;
      myrdpth.pth(0)=0; myrdpth.incth(0)=1;
      for (std::size_t i=1; i<myRDspec.threshold_energy.size(); i++)
      {
        myrdpth.pth(i)=sqrt(pow(myRDspec.threshold_energy[i],2)/4-pow(mwimp,2));
        myrdpth.incth(i)=1;
        #ifdef DARKBIT_RD_DEBUG
          std::cout << "pth, incth: " << myrdpth.pth(i) << "  " << myrdpth.incth(i) << std::endl;
        #endif
      }
      *BEreq::rdpth = myrdpth;

      // determine starting point for integration of Boltzmann eq and write
      // to DS common blocks
      DS_RDDOF *myrddof = &(*BEreq::rddof);
      double xstart=std::max(myrdpars.xinit,1.0001*mwimp/myrddof->tgev(1));
      double tstart=mwimp/xstart;
      int k; myrddof->khi=myrddof->nf; myrddof->klo=1;
      while (myrddof->khi > myrddof->klo+1){
        k=(myrddof->khi+myrddof->klo)/2;
        if (myrddof->tgev(k) < tstart){
          myrddof->khi=k;
        }
        else {
          myrddof->klo=k;
        }
      }


      // follow wide res treatment for heavy Higgs adopted in DS
      double widthheavyHiggs=
             (*BEreq::widths).width(BEreq::particle_code("h0_2"));
      if (widthheavyHiggs<0.1)
        (*BEreq::widths).width(BEreq::particle_code("h0_2"))=0.1;

      // always check that invariant rate is OK at least at one point
      double peff = mwimp/100;
      double weff = (*Dep::RD_eff_annrate)(peff);
      if (Utils::isnan(weff))
            DarkBit_error().raise(LOCAL_INFO, "Weff is NaN in RD_Oh2_general. This means that the function\n"
                                            "pointed to by RD_eff_annrate returned NaN for the invariant rate\n"
                                            "entering the relic density calculation.");      

      #ifdef DARKBIT_RD_DEBUG
        // Dump Weff info on screen
        std::cout << "xstart = " << xstart << std::endl;
        for ( peff = mwimp/1000;  peff < mwimp; peff = peff*1.5 )
        {
          weff = (*Dep::RD_eff_annrate)(peff);
          std::cout << "Weff(" << peff << ") = " << weff << std::endl;
          // Check that the invariant rate is OK.
          if (Utils::isnan(weff))
            DarkBit_error().raise(LOCAL_INFO, "RD debug: Weff is NaN in RD_Oh2_general.");
        }
        // Set up timing
        std::chrono::time_point<std::chrono::system_clock> start, end;
        start = std::chrono::system_clock::now();
        logger() << "Tabulating RD_eff_annrate..." << EOM;
        std::cout << "Starting dsrdtab..." << std::endl;
      #endif

            

      // Tabulate the invariant rate
      BEreq::dsrdtab(byVal(*Dep::RD_eff_annrate),xstart);

      #ifdef DARKBIT_RD_DEBUG
        logger() << LogTags::repeat_to_cout << "...done!" << EOM;

        // Get runtime
        end = std::chrono::system_clock::now();
        double runtime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        // Check if runtime too long
        if ( runtime > 30. )
        {
          std::cout << "Duration [ms]: " << runtime << std::endl;
          //SLHAstruct mySLHA = Dep::MSSM_spectrum->getSLHAea(2);
          //std::ofstream ofs("RelicDensity_debug.slha");
          //ofs << mySLHA;
          //ofs.close();
          tbtest=true;
        }
      #endif

      // Check whether piped invalid point was thrown
      piped_invalid_point.check();

      // determine integration limit
      BEreq::dsrdthlim();

      // now solve Boltzmann eqn using tabulated rate
      double xend, yend, xf; int nfcn;
      BEreq::dsrdeqn(byVal(BEreq::dsrdwintp.pointer()),
          xstart,xend,yend,xf,nfcn);
      // using the untabulated rate gives the same result but is usually
      // slower:
      // BEreq::dsrdeqn(byVal(*Dep::RD_eff_annrate),xstart,xend,yend,xf,nfcn);

      // change heavy Higgs width in DS back to standard value
      (*BEreq::widths).width(BEreq::particle_code("h0_2"))
         =widthheavyHiggs;

      //Check for NAN result.
      if ( Utils::isnan(yend) ) DarkBit_error().raise(LOCAL_INFO, "DarkSUSY returned NaN for relic density!");

      result = 0.70365e8*myrddof->fh(myrddof->nf)*mwimp*yend;

      logger() << LogTags::debug << "RD_oh2_general: oh2 =" << result << EOM;

      #ifdef DARKBIT_DEBUG
        std::cout << std::endl << "DM mass = " << mwimp<< std::endl;
        std::cout << "Oh2     = " << result << std::endl << std::endl;
      #endif

      #ifdef DARKBIT_RD_DEBUG
        if (tbtest) exit(1);
      #endif


    } // function RD_oh2_general


    //////////////////////////////////////////////////////////////////////////
    //
    //             Simple relic density routines for cross-checks
    //                      (MicrOmegas vs DarkSUSY)
    //
    //////////////////////////////////////////////////////////////////////////

    /*! \brief Relic density directly from a call of initialized MicrOmegas.
    */
    void RD_oh2_MicrOmegas(double &oh2)
    {
      using namespace Pipes::RD_oh2_MicrOmegas;
      // Input
      int fast;     // fast: 1, accurate: 0
      double Beps;  // Beps=1e-5 recommended, Beps=1 switches coannihilation off
      int VZdecay, VWdecay; // 0=no 3 body final states
                            // 1=3 body final states in annihlations
                            // 2=3 body final states in co-annihilations


      // Set options via ini-file (MicrOmegas-specific performance options)
      fast = runOptions->getValueOrDef<int>(0, "fast");
      Beps = runOptions->getValueOrDef<double>(1e-5, "Beps");
      VZdecay = runOptions->getValueOrDef<int>(1, "VZdecay");
      VWdecay = runOptions->getValueOrDef<int>(1, "VWdecay");
      if (VZdecay != 1 || VWdecay != 1)
      {
          *BEreq::VZdecay = VZdecay;
          *BEreq::VWdecay = VWdecay;
          BEreq::cleanDecayTable();
      }
      logger() << LogTags::debug << "Using fast: " << fast << " Beps: " << Beps;
      logger() << " VWdecay: " << VWdecay << " VZdecay: " << VZdecay << EOM;

      // Output
      double Xf;
      oh2 = BEreq::oh2(&Xf, byVal(fast), byVal(Beps));
      logger() << LogTags::debug << "X_f = " << Xf << " Omega h^2 = " << oh2 << EOM;
    }


    //////////////////////////////////////////////////////////////////////////
    //
    //   Infer fraction of Dark matter that is made up by scanned DM particles
    //
    //////////////////////////////////////////////////////////////////////////

    void RD_fraction_from_oh2(double &result)
    {
      using namespace Pipes::RD_fraction_from_oh2;
      result = -1;
      double oh2_theory = *Dep::RD_oh2;
      /// Option oh2_obs<double>: Set reference dark matter density (Oh2) for this module function (default 0.1188)
      double oh2_obs = runOptions->getValueOrDef<double>(0.1188, "oh2_obs");
      /// Option mode<str::string>: Set fraction mode (one, leq_one, any ; default is "one")
      std::string mode = runOptions->getValueOrDef<std::string>("one", "mode");
      if (mode ==  "one")
        result = 1;
      if (mode == "leq_one")
        result = std::min(1., oh2_theory/oh2_obs);
      if (mode == "any")
        result = oh2_theory/oh2_obs;
      if (result == -1)
        DarkBit_error().raise(LOCAL_INFO, "ERROR in RD_fraction: Unknown mode (options: one, leq_one, any)");
      logger() << LogTags::debug << "Fraction of dark matter that the scanned model accounts for: " << result << EOM;
    }

    void RD_fraction_fixed(double &result)
    {
      using namespace Pipes::RD_fraction_fixed;
      result = 1;
    }
  }
}
