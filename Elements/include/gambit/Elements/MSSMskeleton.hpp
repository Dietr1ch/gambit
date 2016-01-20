//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
//
///  *********************************************
///
///  Authors: 
///  <!-- add name and date if you modify -->
///   
///  \author Ben Farmer
///          (benjamin.farmer@fysik.su.se)
///  \date 2015 Apr 
///
///  *********************************************

#ifndef __MSSMskeleton_hpp__
#define __MSSMskeleton_hpp__

#include "gambit/Elements/subspectrum.hpp"
#include "gambit/Elements/SLHAskeleton.hpp"

namespace Gambit
{

      /// Skeleton "model" class which interacts with an SLHAea object
      /// Some common functions defined in base class
      class MSSMea: public SLHAeaModel
      {
         public:
           /// @{ Constructors
           MSSMea();
           MSSMea(const SLHAstruct& input);
           /// @}

           /// @{ Getters for MSSM information 
           double get_Mu()  const; 
           double get_BMu() const; 
           double get_vd()  const; 
           double get_vu()  const; 

           double get_MassB () const; 
           double get_MassWB() const; 
           double get_MassG () const; 
           double get_mHd2()   const;  
           double get_mHu2()   const;  

           double get_mq2(int i, int j) const;
           double get_ml2(int i, int j) const;
           double get_md2(int i, int j) const;
           double get_mu2(int i, int j) const;
           double get_me2(int i, int j) const;

           double get_TYd(int i, int j) const;
           double get_TYu(int i, int j) const;
           double get_TYe(int i, int j) const;

           double get_Yd(int i, int j) const; 
           double get_Yu(int i, int j) const; 
           double get_Ye(int i, int j) const; 
   
           double get_g1() const;
           double get_g2() const;
           double get_g3() const;
           double get_tanbeta() const;
           double get_sinthW2_DRbar() const;
  
           double get_MGlu_pole() const;

           double get_Mhh_pole_slha(int i) const;
           double get_MAh_pole () const;
           double get_MHpm_pole() const;

           double get_MCha_pole_slha(int i) const;
           double get_MSd_pole_slha(int i) const;
           double get_MSu_pole_slha(int i) const;
           double get_MSe_pole_slha(int i) const;
           double get_MSv_pole_slha(int i) const;
           double get_MChi_pole_slha(int i) const;
           
           // Pole Mixings
           double get_ZD_pole_slha(int i, int j) const;
           double get_ZU_pole_slha(int i, int j) const;
           double get_ZV_pole_slha(int i, int j) const;
           double get_ZE_pole_slha(int i, int j) const;
           double get_ZH_pole_slha(int i, int j) const;
           double get_ZA_pole_slha(int i, int j) const;
           double get_ZP_pole_slha(int i, int j) const;
           double get_ZN_pole_slha(int i, int j) const;
           double get_UM_pole_slha(int i, int j) const;
           double get_UP_pole_slha(int i, int j) const;
           /// @}
      };

      /// MSSM specialisation of SLHAea object wrapper version of SubSpectrum class
      class MSSMskeleton : public SLHAskeleton<MSSMskeleton,SLHAskeletonTraits<MSSMea> > 
      {
         friend class RunparDer<MSSMskeleton,SLHAskeletonTraits<MSSMea> >;
         friend class PhysDer  <MSSMskeleton,SLHAskeletonTraits<MSSMea> >;

         private:
            typedef MapTypes<SLHAskeletonTraits<MSSMea>,MapTag::Get> MTget; 

            typedef MSSMea Model; 

         public:
            // Constructors/destructors
            MSSMskeleton();
            MSSMskeleton(const SLHAstruct&);
            MSSMskeleton(const MSSMskeleton&);
            virtual ~MSSMskeleton() {};

            virtual int get_index_offset() const;
            virtual SLHAstruct getSLHAea() const;
            virtual void add_to_SLHAea(SLHAea::Coll&) const; 
            virtual const std::map<int, int>& PDG_translator() const;
 
            /// Map fillers
            /// Used to initialise maps in the RunparDer and PhysDer classes
            /// (specialisations created and stored automatically by Spec<QedQcdWrapper>)
            typedef std::map<Par::Phys,MapCollection<MTget>> PhysGetterMaps; 
            //typedef std::map<Par::Phys,MapCollection<MTset>> PhysSetterMaps; 
            typedef std::map<Par::Running,MapCollection<MTget>> RunningGetterMaps; 
            //typedef std::map<Par::Running,MapCollection<MTset>> RunningSetterMaps; 

            /// Runnning parameter map fillers (access parameters via spectrum.runningpar)
            static RunningGetterMaps runningpars_fill_getter_maps();
            //static RunningSetterMaps runningpars_fill_setter_maps();
 
            /// Phys parameter map fillers (access parameters via spectrum.phys())
            static PhysGetterMaps    phys_fill_getter_maps();
            //static PhysSetterMaps    phys_fill_setter_maps();
       };

} // end Gambit namespace

#endif
