//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  "Header" declarations for SingletDMZ3Spec class
///  (definitions in another header file due to
///  this being a template class)
///
///  *********************************************
///
///  Authors: 
///  <!-- add name and date if you modify -->
///   
///  \author Peter Athron  
///          (peter.athron@coepp.org.au)
///  \date 2014, 2015 Jan, Feb, Mar 
///
///  \author Ben Farmer
///          (benjamin.farmer@fysik.su.se)
///  \date 2014, 2015 Jan, Feb, Mar
///
///  \author James McKay
///          j.mckay14@imperial.ac.uk
///  \date 2015 Mar (edited original for use with SingletDM)
///
///  *********************************************

#ifndef SingletDMZ3SPEC_HEAD_H
#define SingletDMZ3SPEC_HEAD_H

#include "gambit/Elements/spec.hpp"
#include "gambit/Utils/util_functions.hpp"
#include "gambit/Models/SpectrumContents/RegisteredSpectra.hpp"

// Flexible SUSY stuff (should not be needed by the rest of gambit)
#include "flexiblesusy/config/config.h"

namespace Gambit 
{
   namespace SpecBit 
   {
      template <class MI>  // "MI" for "Model_interface"
      class SingletDMZ3Spec;
   } 

   // For example of what kind of class MI needs to be, see
   // SpecBit/include/model_files_and_boxes.hpp, 
   // MODELNAME_interface class

   /// Specialisation of "traits" class used to inform Spec<T> class of what
   /// "Model" and "Input" are for this derived class
   template <>
   template <class MI>
   struct SpecTraits<SpecBit::SingletDMZ3Spec<MI>>
   {
      static std::string name() { return "SingletDMZ3Spec"; }
      typedef SpectrumContents::SingletDMZ3 Contents;
      typedef typename MI::Model Model;
      typedef DummyInput Input;
   };

   namespace SpecBit
   {
      template <class MI>
      class SingletDMZ3Spec : public Spec<SingletDMZ3Spec<MI>>
      {
         private:
            str backend_name;
            str backend_version;
            static const int _index_offset;

         public:
            /// These typedefs are inherited, but the name lookup doesn't work so smoothly in
            /// templated wrapper classes, so need to help them along:
            typedef SingletDMZ3Spec<MI> Self;
            typedef typename Self::MTget MTget; 
            typedef typename Self::MTset MTset; 
            typedef typename Self::GetterMaps GetterMaps;
            typedef typename Self::SetterMaps SetterMaps;
            typedef typename SpecTraits<Self>::Model Model;
            typedef typename SpecTraits<Self>::Input Input;
           
            /// Interface function overrides
            static int index_offset() {return _index_offset;}
            virtual double GetScale() const;
            virtual void SetScale(double scale);           
            virtual void RunToScaleOverride(double scale);

            //constructors
            SingletDMZ3Spec();
            SingletDMZ3Spec(MI, str backend_name, str backend_version);

            //Could more constructors to interface with other generators   
             
            // These are public for now so that SpecBit_tests.cpp can access them
            MI model_interface;

            // Dummy placeholder for potential Inputs object
            DummyInput dummyinput;

            //Destructor
            virtual ~SingletDMZ3Spec();

            // Functions to interface Model and Input objects with the base 'Spec' class
            Model& get_Model() { return model_interface.model; }
            Input& get_Input() { return dummyinput; /*unused here, but needs to be defined for the interface*/ }
            const Model& get_Model() const { return model_interface.model; }
            const Input& get_Input() const { return dummyinput; /*unused here, but needs to be defined for the interface*/ }

            //some model independent stuff
                                  //may use something like this to pass error to Gambit
            virtual std::string AccessError(std::string state) const;

 
 
 
            template<class SingletDMZ3like>
            void get_lowe_data_from(SingletDMZ3like &othermodel)
            {

              typename MI::Model& model = model_interface.model;

              model.set_scale( othermodel.get_scale() );
              model.set_Yu( othermodel.get_Yu() );


              return;
            }

            // Overload of this function to just easily copy the othermodel object
            // if it is of type Model
            void get_external_spectrum(typename MI::Model& othermodel)
            {
              model_interface.model = othermodel;
              return;
            }

            /// Map filler overrides
            static GetterMaps fill_getter_maps();
            static SetterMaps fill_setter_maps();

      };


     
   } // end SpecBit namespace
} // end Gambit namespace

#undef MAPS
#undef MAPS1
#endif
