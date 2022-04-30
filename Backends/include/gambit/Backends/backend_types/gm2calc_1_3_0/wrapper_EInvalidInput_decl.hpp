#ifndef __wrapper_EInvalidInput_decl_gm2calc_1_3_0_hpp__
#define __wrapper_EInvalidInput_decl_gm2calc_1_3_0_hpp__

#include <cstddef>
#include "forward_decls_wrapper_classes.hpp"
#include "gambit/Backends/wrapperbase.hpp"
#include "abstract_EInvalidInput.hpp"
#include "wrapper_Error_decl.hpp"
#include <string>

#include "identification.hpp"

namespace CAT_3(BACKENDNAME,_,SAFE_VERSION)
{
   
   namespace gm2calc
   {
      
      class EInvalidInput : public Error
      {
            // Member variables: 
         public:
            // -- Static factory pointers: 
            static gm2calc::Abstract_EInvalidInput* (*__factory0)(const ::std::basic_string<char>&);
      
            // -- Other member variables: 
      
            // Member functions: 
         public:
            ::std::basic_string<char> what() const;
      
      
            // Wrappers for original constructors: 
         public:
            EInvalidInput(const ::std::basic_string<char>& message_);
      
            // Special pointer-based constructor: 
            EInvalidInput(gm2calc::Abstract_EInvalidInput* in);
      
            // Copy constructor: 
            EInvalidInput(const EInvalidInput& in);
      
            // Assignment operator: 
            EInvalidInput& operator=(const EInvalidInput& in);
      
            // Destructor: 
            ~EInvalidInput();
      
            // Returns correctly casted pointer to Abstract class: 
            gm2calc::Abstract_EInvalidInput* get_BEptr() const;
      
      };
   }
   
}


#include "gambit/Backends/backend_undefs.hpp"

#endif /* __wrapper_EInvalidInput_decl_gm2calc_1_3_0_hpp__ */
