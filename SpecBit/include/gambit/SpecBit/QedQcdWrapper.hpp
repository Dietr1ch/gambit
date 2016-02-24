//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  This class is used to wrap the QedQcd object used by SoftSUSY
///  and FlexibleSUSY in a Gambit SubSpectrum object. This is to enable
///  access to the parameters of the SM defined as a low-energy effective theory 
///  (as opposed to correspending information defined in a UV model). 
///  Parameters defined this way are often used as input to a physics calculator.
///
///  This is one of the simplest wrappers possible, so it is useful as a guide
///  for designing other SubSpectrum wrappers. To assist this, code is documented
///  with the following markings to distinguish pieces that are essential for
///  all wrappers from those which are specific to this wrapper:
///  /***/ - Required by all wrappers.
///  /*O*/ - Optional (e.g. unused maps fillers can be left undeclared)
///  /*P*/ - Required if map fillers are protected, which is sensible.
///
///  *********************************************
///
///  Authors: 
///  <!-- add name and date if you modify -->
///   
///  \author Ben Farmer
///          (benjamin.farmer@fysik.su.se)
///  \date 2015 Mar 
///
///  *********************************************

#ifndef __QedQcdWrapper_hpp__
#define __QedQcdWrapper_hpp__

#include "gambit/Elements/sminputs.hpp"
#include "gambit/Elements/spec.hpp"

#include "lowe.h" ///TODO: wrap using BOSS at some point, i.e. get this from FlexibleSUSY or SoftSUSY

namespace Gambit
{
   namespace SpecBit
   {
      class QedQcdWrapper;       
   }

   /// Specialisation of traits class needed to inform base spectrum class of the Model and Input types
   template <>
   struct SpecTraits<SpecBit::QedQcdWrapper> 
   {
      typedef SpectrumContents::SM Contents;
      typedef softsusy::QedQcd Model;
      typedef SMInputs         Input;
   };
     
   namespace SpecBit
   {   
      class QedQcdWrapper : public Spec<QedQcdWrapper> 
      {
         private:
            // Keep copies of Model and Input objects internally
            Model qedqcd;
            Input sminputs;
   
         public:
            // Constructors/destructors
            QedQcdWrapper();
            QedQcdWrapper(const softsusy::QedQcd&, const SMInputs&);
            virtual ~QedQcdWrapper();        /***/
   
            // Functions to interface Model and Input objects with the base 'Spec' class
            Model& get_Model() { return qedqcd; }
            Input& get_Input() { return sminputs; }
            const Model& get_Model() const { return qedqcd; }
            const Input& get_Input() const { return sminputs; }

            virtual int get_index_offset() const;  /***/   
            virtual int get_numbers_stable_particles() const;  /***/
   
            /// Add QEDQCD information to an SLHAea object
            virtual void add_to_SLHAea(SLHAstruct& slha) const;

            /// RunningPars interface overrides
            virtual double GetScale() const;      /***/
            virtual void SetScale(double scale);  /***/
            virtual void RunToScaleOverride(double);      /***/
   
            /// Limits for running
            /// @{
            double hardup; // Be careful of order in constructor!
            double softup;
            double softlow;
            double hardlow;
            virtual double hard_upper() const {return hardup;} /*O*/
            virtual double soft_upper() const {return softup;} /*O*/
            virtual double soft_lower() const {return softlow;}/*O*/
            virtual double hard_lower() const {return hardlow;}/*O*/
            /// @}
   
            /// Map fillers
            static GetterMaps fill_getter_maps(); /*O*/
            static SetterMaps fill_setter_maps(); /*O*/
    
      };
 
   } // end SpecBit namespace
} // end Gambit namespace

#endif
