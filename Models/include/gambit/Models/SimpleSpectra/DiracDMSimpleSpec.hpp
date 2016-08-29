//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  A simple SubSpectrum wrapper for the DiracDM
///  model. No RGEs included (file format is based 
///  on ScalarSingletDMSimpleSpec.hpp).
///
///  *********************************************
///
///  Authors: 
///  <!-- add name and date if you modify -->
///   
///  \author Ankit Beniwal
///          (ankit.beniwal@adelaide.edu.au)
///  \date 2016 Aug
///
///  *********************************************

#ifndef __DiracDMSimpleSpec_hpp__
#define __DiracDMSimpleSpec_hpp__

#include "gambit/Elements/spec.hpp"
#include "gambit/Models/SpectrumContents/RegisteredSpectra.hpp"

namespace Gambit
{     
   namespace Models
   {
      /// Simple extension of the SMHiggsSimpleSpec "model object"
      /// to include Dirac DM parameters
      /// We could easily just put these in the wrapper itself, but
      /// I am leaving them in a separate struct for the sake of building
      /// up examples towards a more complicated "model" object
      struct DiracDMModel
      {
         double HiggsPoleMass;
         double HiggsVEV;
         double DiracPoleMass;
         double DiracLambda;
         double DiraccosXI;

         double LambdaH;
         double g1, g2, g3, sinW2;
         double Yd[3], Ye[3], Yu[3];
      };
     
  
      /// Forward declare the wrapper class so that we can use it
      /// as the template parameter for the SpecTraits specialisation. 
      class DiracDMSimpleSpec;  
   }

   /// Specialisation of traits class needed to inform base spectrum class of the Model and Input types
   template <>
   struct SpecTraits<Models::DiracDMSimpleSpec> : DefaultTraits
   {
      static std::string name() { return "DiracDMSimpleSpec"; }
      typedef SpectrumContents::DiracDM Contents;
   };

   namespace Models
   { 
      class DiracDMSimpleSpec : public Spec<DiracDMSimpleSpec> 
      {
         private:
            DiracDMModel params;

            typedef DiracDMSimpleSpec Self;

         public:
            /// @{ Constructors/destructors
            DiracDMSimpleSpec(const DiracDMModel& p)
             : params(p)
            {}

            static int index_offset() {return -1;}
        
            /// @}
 
            /// Wrapper-side interface functions to parameter object
            double get_HiggsPoleMass()   const { return params.HiggsPoleMass; } 
            double get_HiggsVEV()        const { return params.HiggsVEV;      } 
            double get_DiracPoleMass() const { return params.DiracPoleMass; } 
            double get_lambda_F()       const { return params.DiracLambda; }
            double get_cos_XI()         const { return params.DiraccosXI; }
            double get_lambda_h()       const { return params.LambdaH; }
            double get_g1()       const { return params.g1; }
            double get_g2()       const { return params.g2; }
            double get_g3()       const { return params.g3; }
            double get_sinW2()       const { return params.sinW2; }
           
            double get_Yd(int i, int j)       const { if (i==j){return params.Yd[i];}else{return 0;} }
            double get_Yu(int i, int j)       const { if (i==j){return params.Yu[i];}else{return 0;} }
            double get_Ye(int i, int j)       const { if (i==j){return params.Ye[i];}else{return 0;} }
           
            void set_HiggsPoleMass(double in)   { params.HiggsPoleMass=in; } 
            void set_HiggsVEV(double in)        { params.HiggsVEV=in;      } 
            void set_DiracPoleMass(double in) { params.DiracPoleMass=in; } 
            void set_lambda_F(double in)       { params.DiracLambda=in; }
            void set_cos_XI(double in)         { params.DiraccosXI=in; }
            void set_lambda_h(double in)       { params.LambdaH=in; }
            void set_g1(double in)        { params.g1=in; }
            void set_g2(double in)        { params.g2=in; }
            void set_g3(double in)       { params.g3=in; }
            void set_sinW2(double in)       { params.sinW2=in; }
           
            void set_Yd(double in, int i, int j)       { if (i==j){params.Yd[i]=in;}}
            void set_Yu(double in, int i, int j)       { if (i==j){params.Yu[i]=in;}}
            void set_Ye(double in, int i, int j)       { if (i==j){params.Ye[i]=in;}}

            /// @{ Map fillers
            static GetterMaps fill_getter_maps()
            {
               GetterMaps getters;
               typedef typename MTget::FInfo2W FInfo2W;
               static const int i012v[] = {0,1,2};
               static const std::set<int> i012(i012v, Utils::endA(i012v));

               using namespace Par;

               getters[mass1]        .map0W["vev"]       = &Self::get_HiggsVEV;
               getters[dimensionless].map0W["lF"] = &Self::get_lambda_F;
               getters[dimensionless].map0W["cosXI"] = &Self::get_cos_XI;

               getters[Pole_Mass].map0W["h0"]      = &Self::get_HiggsPoleMass;
               getters[Pole_Mass].map0W["h0_1"]    = &Self::get_HiggsPoleMass;
 
               getters[Pole_Mass].map0W["F"]       = &Self::get_DiracPoleMass;               
              
               getters[dimensionless].map0W["lambda_h"] = &Self::get_lambda_h;
              
               getters[dimensionless].map0W["g1"] = &Self::get_g1;
               getters[dimensionless].map0W["g2"] = &Self::get_g2;
               getters[dimensionless].map0W["g3"] = &Self::get_g3;
               getters[dimensionless].map0W["sinW2"] = &Self::get_sinW2;
              
               getters[dimensionless].map2W["Yd"]= FInfo2W( &Self::get_Yd, i012, i012);
               getters[dimensionless].map2W["Yu"]= FInfo2W( &Self::get_Yu, i012, i012);
               getters[dimensionless].map2W["Ye"]= FInfo2W( &Self::get_Ye, i012, i012);

               return getters;
            }

            static SetterMaps fill_setter_maps()
            {
               SetterMaps setters;
               typedef typename MTset::FInfo2W FInfo2W;
               static const int i012v[] = {0,1,2};
               static const std::set<int> i012(i012v, Utils::endA(i012v));
              
               using namespace Par;

               setters[mass1].map0W["vev"]       = &Self::set_HiggsVEV;
               setters[dimensionless].map0W["lF"] = &Self::set_lambda_F;
               setters[dimensionless].map0W["cosXI"] = &Self::set_cos_XI;
               setters[dimensionless].map0W["lambda_h"] = &Self::set_lambda_h;

               setters[dimensionless].map0W["g1"] = &Self::set_g1;
               setters[dimensionless].map0W["g2"] = &Self::set_g2;
               setters[dimensionless].map0W["g3"] = &Self::set_g3;
               setters[dimensionless].map0W["sinW2"] = &Self::set_sinW2;

               setters[Pole_Mass].map0W["h0"]    = &Self::set_HiggsPoleMass;
               //setters[Pole_Mass].map0W["h0_1"]  = &Self::set_HiggsPoleMass;
 
               setters[Pole_Mass].map0W["F"]       = &Self::set_DiracPoleMass;                
   
               setters[dimensionless].map2W["Yd"]= FInfo2W( &Self::set_Yd, i012, i012);
               setters[dimensionless].map2W["Yu"]= FInfo2W( &Self::set_Yu, i012, i012);
               setters[dimensionless].map2W["Ye"]= FInfo2W( &Self::set_Ye, i012, i012);

               return setters;
            }
            /// @}

        }; 

   } // end Models namespace
} // end Gambit namespace

#endif
