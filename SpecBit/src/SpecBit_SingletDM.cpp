//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Functions of module SpecBit
///
///  SpecBit module functions related to the
///  scalar singlet DM model.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Ben Farmer
///          (benjamin.farmer@fysik.su.se)
///    \date 2015 May
///
///  *********************************************

#include <string>
#include <sstream>

#include "gambit/Elements/gambit_module_headers.hpp"

#include "gambit/Elements/spectrum.hpp"
#include "gambit/Utils/stream_overloads.hpp" // Just for more convenient output to logger
#include "gambit/Utils/util_macros.hpp"




#include "gambit/SpecBit/SpecBit_rollcall.hpp"
#include "gambit/SpecBit/SpecBit_helpers.hpp"
#include "gambit/SpecBit/QedQcdWrapper.hpp"
#include "gambit/Models/SimpleSpectra/SMHiggsSimpleSpec.hpp"
#include "gambit/Models/SimpleSpectra/ScalarSingletDMSimpleSpec.hpp"
#include "gambit/SpecBit/model_files_and_boxes.hpp"

#include "gambit/SpecBit/SingletDMZ3Spec.hpp"
#include "gambit/SpecBit/SingletDMSpec.hpp"


//#include "gambit/SpecBit/SMskeleton.hpp"


// Flexible SUSY stuff (should not be needed by the rest of gambit)
#include "flexiblesusy/src/ew_input.hpp"
#include "flexiblesusy/src/lowe.h" // From softsusy; used by flexiblesusy
#include "flexiblesusy/src/numerics2.hpp"
#include "flexiblesusy/src/two_loop_corrections.hpp"
// Switch for debug mode
//#define SPECBIT_DEBUG

namespace Gambit
{

  namespace SpecBit
  {
    using namespace LogTags;
    using namespace flexiblesusy;

    /// Get a (simple) Spectrum object wrapper for the SingletDM model
    void get_SingletDM_spectrum(Spectrum& result)
    {
      namespace myPipe = Pipes::get_SingletDM_spectrum;
      const SMInputs& sminputs = *myPipe::Dep::SMINPUTS;

      // Initialise an object to carry the Singlet plus Higgs sector information
      Models::SingletDMModel singletmodel;
      
      
      // quantities needed to fill container spectrum, intermediate calculations
      double alpha_em = 1.0 / sminputs.alphainv;
      double mz2 = pow(sminputs.mZ,2);
      double GF = sminputs.GF;
      double sinW2cosW2 = Pi * alpha_em / (pow(2,0.5) * mz2 * GF ) ;
      

      double e = pow( 4*Pi*( alpha_em ),0.5) ;
      
      
      double sin2W = pow(2 * sinW2cosW2, 0.5);
      double tW = 0.5* asin( sin2W );
      double sinW2 = pow( sin (tW) , 2);
      double cosW2 = pow( cos (tW) , 2);
    
      
      // Higgs sector
      double mh   = *myPipe::Param.at("mH");
      singletmodel.HiggsPoleMass   = mh;
      
      
      
      
      
      double vev        = 1. / sqrt(sqrt(2.)*GF);
      singletmodel.HiggsVEV        = vev;
    //  singletmodel.LambdaH   = GF*pow(mh,2)/pow(2,0.5) ;
      
      // Scalar singlet sector
      
      singletmodel.SingletPoleMass = *myPipe::Param.at("mS");
      singletmodel.SingletLambda   = *myPipe::Param.at("lambda_hS");
      singletmodel.SingletLambdaS   = 0;
      
      
      // Standard model
      
      singletmodel.sinW2 = sinW2;
      
      // gauge couplings
      singletmodel.g1 = e / sinW2;
      singletmodel.g2 = e / cosW2;
      singletmodel.g3   = pow( 4*Pi*( sminputs.alphaS ),0.5) ;
      
      double sqrt2v = pow(2.0,0.5)/vev;
      // Yukawas
      
      singletmodel.Yu[0] = sqrt2v * sminputs.mU;
      singletmodel.Yu[1] = sqrt2v * sminputs.mCmC;
      singletmodel.Yu[2] = sqrt2v * sminputs.mT;
      
      singletmodel.Ye[0] = sqrt2v * sminputs.mE;
      singletmodel.Ye[1] = sqrt2v * sminputs.mMu;
      singletmodel.Ye[2] = sqrt2v * sminputs.mTau;
      
      singletmodel.Yd[0] = sqrt2v * sminputs.mD;
      singletmodel.Yd[1] = sqrt2v * sminputs.mS;
      singletmodel.Yd[2] = sqrt2v * sminputs.mBmB;
      
      
      

      // Create a SubSpectrum object to wrap the EW sector information
      Models::ScalarSingletDMSimpleSpec singletspec(singletmodel);

      // We don't supply a LE subspectrum here; an SMSimpleSpec will therefore be automatically created from 'sminputs'
      result = Spectrum(singletspec,sminputs,&myPipe::Param);
    }
    
    
  //  template <class MI,class SI,class SIinfo>
    template<class MI,class SI>
    Spectrum run_FS_spectrum_generator
        ( const typename MI::InputParameters& input
        , const SMInputs& sminputs
        , const Options& runOptions
        , const std::map<str, safe_ptr<double> >& input_Param
        )
    {
      // SoftSUSY object used to set quark and lepton masses and gauge
      // couplings in QEDxQCD effective theory
      // Will be initialised by default using values in lowe.h, which we will
      // override next. 
      softsusy::QedQcd oneset;

      // Fill QedQcd object with SMInputs values
      setup_QedQcd(oneset,sminputs);

      // Run everything to Mz
      oneset.toMz();
 
      // Create spectrum generator object
      typename MI::SpectrumGenerator spectrum_generator;

      // Spectrum generator settings
      // Default options copied from flexiblesusy/src/spectrum_generator_settings.hpp
      //
      // | enum                             | possible values              | default value   |
      // |----------------------------------|------------------------------|-----------------|
      // | precision                        | any positive double          | 1.0e-4          |
      // | max_iterations                   | any positive double          | 0 (= automatic) |
      // | algorithm                        | 0 (two-scale) or 1 (lattice) | 0 (= two-scale) |
      // | calculate_sm_masses              | 0 (no) or 1 (yes)            | 0 (= no)        |
      // | pole_mass_loop_order             | 0, 1, 2                      | 2 (= 2-loop)    |
      // | ewsb_loop_order                  | 0, 1, 2                      | 2 (= 2-loop)    |
      // | beta_loop_order                  | 0, 1, 2                      | 2 (= 2-loop)    |
      // | threshold_corrections_loop_order | 0, 1                         | 1 (= 1-loop)    |
      // | higgs_2loop_correction_at_as     | 0, 1                         | 1 (= enabled)   |
      // | higgs_2loop_correction_ab_as     | 0, 1                         | 1 (= enabled)   |
      // | higgs_2loop_correction_at_at     | 0, 1                         | 1 (= enabled)   |
      // | higgs_2loop_correction_atau_atau | 0, 1                         | 1 (= enabled)   |
     
      #define SPECGEN_SET(NAME,TYPE,DEFAULTVAL) \
         CAT_2(spectrum_generator.set_, NAME) BOOST_PP_LPAREN() runOptions.getValueOrDef<TYPE> \
               BOOST_PP_LPAREN() DEFAULTVAL BOOST_PP_COMMA() STRINGIFY(NAME) \
               BOOST_PP_RPAREN() BOOST_PP_RPAREN()

      SPECGEN_SET(precision_goal,                    double, 1.0e-4);
      SPECGEN_SET(max_iterations,                    double, 0 );
      SPECGEN_SET(calculate_sm_masses,               bool, false );
      SPECGEN_SET(pole_mass_loop_order,              int, 2 );
      SPECGEN_SET(ewsb_loop_order,                   int, 2 );
      SPECGEN_SET(beta_loop_order,                   int, 2 );
      SPECGEN_SET(threshold_corrections_loop_order,  int, 1 );

      #undef SPECGEN_SET

      // Higgs loop corrections are a little different... sort them out now     
      Two_loop_corrections two_loop_settings;

       two_loop_settings.higgs_at_as
         = runOptions.getValueOrDef<bool>(true,"use_higgs_2loop_at_as");
      two_loop_settings.higgs_ab_as
         = runOptions.getValueOrDef<bool>(true,"use_higgs_2loop_ab_as");
      two_loop_settings.higgs_at_at
         = runOptions.getValueOrDef<bool>(true,"use_higgs_2loop_at_at");
      two_loop_settings.higgs_atau_atau
         = runOptions.getValueOrDef<bool>(true,"use_higgs_2loop_atau_atau");

      spectrum_generator.set_two_loop_corrections(two_loop_settings);

      // Generate spectrum
      spectrum_generator.run(oneset, input);
      const typename MI::Problems& problems = spectrum_generator.get_problems();

      MI model_interface(spectrum_generator,oneset,input);

      
      SI singletdmspec(model_interface, "FlexibleSUSY", "1.1.0"); // new templated spectrum class name
      //SingletDMSpec<MI> singletdmspec(model_interface, "FlexibleSUSY", "1.1.0"); // should be 1.2.4?

    
      singletdmspec.set_override(Par::mass1,spectrum_generator.get_high_scale(),"high_scale",true);
      singletdmspec.set_override(Par::mass1,spectrum_generator.get_susy_scale(),"susy_scale",true);
      singletdmspec.set_override(Par::mass1,spectrum_generator.get_low_scale(), "low_scale", true);


      singletdmspec.set_override(Par::Pole_Mass_1srd_high,0.0, "h0_1", true);
      singletdmspec.set_override(Par::Pole_Mass_1srd_low,0.0,"h0_1", true);
      



      // Create a second SubSpectrum object to wrap the qedqcd object used to initialise the spectrum generator
      // Attach the sminputs object as well, so that SM pole masses can be passed on (these aren't easily
      // extracted from the QedQcd object, so use the values that we put into it.)
      QedQcdWrapper qedqcdspec(oneset,sminputs);
      
      
            // Deal with points where spectrum generator encountered a problem
      #ifdef SPECBIT_DEBUG
        std::cout<<"Problem? "<<problems.have_problem()<<std::endl;
      #endif
      if( problems.have_problem() )
      {
         if( runOptions.getValueOrDef<bool>(false,"invalid_point_fatal") )
         {
            ///TODO: Need to tell gambit that the spectrum is not viable somehow. For now
            // just die.
            std::ostringstream errmsg;
            errmsg << "A serious problem was encountered during spectrum generation!; ";
            errmsg << "Message from FlexibleSUSY below:" << std::endl;
            problems.print_problems(errmsg);
            problems.print_warnings(errmsg);
            SpecBit_error().raise(LOCAL_INFO,errmsg.str());
         }
         else
         {
            /// Check what the problem was
            /// see: contrib/MassSpectra/flexiblesusy/src/problems.hpp
            std::ostringstream msg;

            problems.print_problems(msg);
            invalid_point().raise(msg.str()); //TODO: This message isn't ending up in the logs.
         }
      }
      
      return Spectrum(qedqcdspec,singletdmspec,sminputs,&input_Param);
    }



    template <class T>
    void fill_SingletDM_input(T& input, const std::map<str, safe_ptr<double> >& Param,SMInputs sminputs,double scale)
    {
      double mH = *Param.at("mH");
      double mS = *Param.at("mS");
      double lambda_hs = *Param.at("lambda_hS");
      double lambda_s  = *Param.at("lambda_S");
      double QEWSB  = *Param.at("QEWSB");
      input.HiggsIN=-0.5*pow(mH,2);
      double vev=1. / sqrt(sqrt(2.)*sminputs.GF);
      input.muSInput=pow(mS,2)-0.5*lambda_hs*pow(vev,2);
      input.LamSHInput=lambda_hs;
      input.LamSInput=lambda_s;
      input.QEWSB=QEWSB;  // scale where EWSB conditions are applied
      input.Qin=scale;  // highest scale at which model is run to
    }
    
    template <class T>
    void fill_extra_input(T& input, const std::map<str, safe_ptr<double> >& Param )
    {
      input.mu3Input=*Param.at("mu3");
    }

    bool check_perturb(const Spectrum& spec,double scale,int pts)
    {
    using namespace flexiblesusy;
    using namespace Gambit;
    using namespace SpecBit;
    std::unique_ptr<SubSpectrum> SingletDM = spec.clone_HE();
    double step = log10(scale) / pts;
    double runto;
   
    double ul=3.5449077018110318; // sqrt(4*Pi), maximum value for perturbative couplings, same perturbativity bound that FlexibleSUSY uses
    for (int i=0;i<pts;i++)
    {
    runto = pow(10,step*float(i+1.0)); // scale to run spectrum to
    if (runto<100){runto=200.0;}// avoid running to low scales
  
    SingletDM -> RunToScale(runto);
   
    static const SpectrumContents::SingletDM contents;
    static const std::vector<SpectrumParameter> required_parameters = contents.all_parameters_with_tag(Par::dimensionless);
      
    for(std::vector<SpectrumParameter>::const_iterator it = required_parameters.begin();
          it != required_parameters.end(); ++it)
      {
         const Par::Tags        tag   = it->tag();
         const std::string      name  = it->name();
         const std::vector<int> shape = it->shape();
           std::ostringstream label;
           label << name <<" "<< Par::toString.at(tag);
          if(shape.size()==1 and shape[0]==1)
         {
           if (abs(SingletDM->get(tag,name))>ul)
           {
            return false;
           }
         }
        
          else if(shape.size()==1 and shape[0]>1)
         {
           for(int k = 1; k<=shape[0]; ++k) {
           if (abs(SingletDM->get(tag,name,k))>ul)
           {
            return false;
           }
           
           }
          }
          else if(shape.size()==2)
         {
           for(int k = 1; k<=shape[0]; ++k) {
             for(int j = 1; j<=shape[0]; ++j) {
             if (abs(SingletDM->get(tag,name,k,j))>ul)
           {
            return false;
           }
           }
           }
          }
    }
    
    }
    return true;
    }
    
    void get_SingletDM_spectrum_pole(Spectrum& result)
    {
      using namespace softsusy;
      namespace myPipe = Pipes::get_SingletDM_spectrum_pole;
      const SMInputs& sminputs = *myPipe::Dep::SMINPUTS;
      const Options& runOptions=*myPipe::runOptions;
      double scale = runOptions.getValueOrDef<double>(173.34,"FS_high_scale");
      SingletDM_input_parameters input;
      fill_SingletDM_input(input,myPipe::Param,sminputs,scale);
    
      result = run_FS_spectrum_generator<SingletDM_interface<ALGORITHM1>,SingletDMSpec<SingletDM_interface<ALGORITHM1>>>(input,sminputs,*myPipe::runOptions,myPipe::Param);
    
      int check_perturb_pts = runOptions.getValueOrDef<double>(10,"check_perturb_pts");
      double do_check_perturb = runOptions.getValueOrDef<bool>(false,"check_perturb");
      double check_perturb_scale = runOptions.getValueOrDef<double>(1.22e19,"check_high_scale");      
 
      if (do_check_perturb)
      {
      if (!check_perturb(result,check_perturb_scale,check_perturb_pts))
      {
      // invalidate point as spectrum not perturbative up to scale
      std::ostringstream msg;
      msg << "Spectrum not perturbative up to scale = " << check_perturb_scale <<  std::endl;
      #ifdef SPECBIT_DEBUG
      cout << "Spectrum not perturbative up to scale = " << check_perturb_scale <<  endl;
      #endif
      invalid_point().raise(msg.str());
      }
      }
    
    }

    void get_SingletDMZ3_spectrum(Spectrum& result)
    {
      using namespace softsusy;
      namespace myPipe = Pipes::get_SingletDMZ3_spectrum;
      const SMInputs& sminputs = *myPipe::Dep::SMINPUTS;
      const Options& runOptions=*myPipe::runOptions;
      double scale = runOptions.getValueOrDef<double>(173.34,"FS_high_scale");
      SingletDMZ3_input_parameters input;
      fill_SingletDM_input(input,myPipe::Param,sminputs,scale);
      fill_extra_input(input,myPipe::Param);
      result = run_FS_spectrum_generator<SingletDMZ3_interface<ALGORITHM1>,SingletDMZ3Spec<SingletDMZ3_interface<ALGORITHM1>>>(input,sminputs,*myPipe::runOptions,myPipe::Param);

      int check_perturb_pts = runOptions.getValueOrDef<double>(10,"check_perturb_pts");
      double do_check_perturb = runOptions.getValueOrDef<bool>(false,"check_perturb");
      double check_perturb_scale = runOptions.getValueOrDef<double>(1.22e19,"check_high_scale");
      
      if (do_check_perturb)
      {
      if (!check_perturb(result,check_perturb_scale,check_perturb_pts))
      {
      // invalidate point as spectrum not perturbative up to scale
      std::ostringstream msg;
      
      msg << "Spectrum not perturbative up to scale = " << check_perturb_scale <<  std::endl;
      #ifdef SPECBIT_DEBUG
      cout << "Spectrum not perturbative up to scale = " << check_perturb_scale <<  endl;
      #endif
      invalid_point().raise(msg.str());
      }
      }
      
    }

    
    // print spectrum out, stripped down copy from MSSM version with variable names changed
    void fill_map_from_SingletDMspectrum(std::map<std::string,double>&, const Spectrum&);
   
    void get_SingletDM_spectrum_as_map (std::map<std::string,double>& specmap)
    {
      namespace myPipe = Pipes::get_SingletDM_spectrum_as_map;
      const Spectrum& singletdmspec(*myPipe::Dep::SingletDM_spectrum);
      fill_map_from_SingletDMspectrum(specmap, singletdmspec);
    }
    
    void fill_map_from_SingletDMspectrum(std::map<std::string,double>& specmap, const Spectrum& singletdmspec)
    {
      /// Add everything... use spectrum contents routines to automate task
      static const SpectrumContents::SingletDM contents;
      static const std::vector<SpectrumParameter> required_parameters = contents.all_parameters();
      
      for(std::vector<SpectrumParameter>::const_iterator it = required_parameters.begin();
           it != required_parameters.end(); ++it)
      {
         const Par::Tags        tag   = it->tag();
         const std::string      name  = it->name();
         const std::vector<int> shape = it->shape();

         /// Verification routine should have taken care of invalid shapes etc, so won't check for that here.

         // Check scalar case
         if(shape.size()==1 and shape[0]==1)
         {
           std::ostringstream label;
           label << name <<" "<< Par::toString.at(tag);
           specmap[label.str()] = singletdmspec.get_HE().get(tag,name);
         }
         // Check vector case
         else if(shape.size()==1 and shape[0]>1)
         {
           for(int i = 1; i<=shape[0]; ++i) {
             std::ostringstream label;
             label << name <<"_"<<i<<" "<< Par::toString.at(tag);
             specmap[label.str()] = singletdmspec.get_HE().get(tag,name,i);
           }
         }
         // Check matrix case
         else if(shape.size()==2)
         {
           for(int i = 1; i<=shape[0]; ++i) {
             for(int j = 1; j<=shape[0]; ++j) {
               std::ostringstream label;
               label << name <<"_("<<i<<","<<j<<") "<<Par::toString.at(tag);
               specmap[label.str()] = singletdmspec.get_HE().get(tag,name,i,j);
             }  
           }
         }
         // Deal with all other cases
         else
         {
           // ERROR
           std::ostringstream errmsg;           
           errmsg << "Error, invalid parameter received while converting SingletDMspectrum to map of strings! This should no be possible if the spectrum content verification routines were working correctly; they must be buggy, please report this.";
           errmsg << "Problematic parameter was: "<< tag <<", " << name << ", shape="<< shape; 
           utils_error().forced_throw(LOCAL_INFO,errmsg.str());
         }
      }

    }
    
  } // end namespace SpecBit
} // end namespace Gambit

