#ifndef __abstract_EReadError_gm2calc_1_2_0_hpp__
#define __abstract_EReadError_gm2calc_1_2_0_hpp__

#include "gambit/Backends/abstractbase.hpp"
#include "forward_decls_abstract_classes.hpp"
#include "forward_decls_wrapper_classes.hpp"
#include <string>
#include "wrapper_Error_decl.hpp"
#include <cstddef>
#include <iostream>

#include "identification.hpp"

namespace CAT_3(BACKENDNAME,_,SAFE_VERSION)
{
   
   
   namespace gm2calc
   {
      class Abstract_EReadError : virtual public gm2calc::Abstract_Error
      {
         public:
   
            virtual ::std::basic_string<char, std::char_traits<char>, std::allocator<char> > what() const =0;
   
         public:
            using gm2calc::Abstract_Error::pointer_assign__BOSS;
            virtual void pointer_assign__BOSS(Abstract_EReadError*) =0;
            virtual Abstract_EReadError* pointer_copy__BOSS() =0;
   
         private:
            EReadError* wptr;
            bool delete_wrapper;
         public:
            EReadError* get_wptr() { return wptr; }
            void set_wptr(EReadError* wptr_in) { wptr = wptr_in; }
            bool get_delete_wrapper() { return delete_wrapper; }
            void set_delete_wrapper(bool del_wrp_in) { delete_wrapper = del_wrp_in; }
   
         public:
            Abstract_EReadError()
            {
               wptr = 0;
               delete_wrapper = false;
            }
   
            Abstract_EReadError(const Abstract_EReadError& in) : 
               gm2calc::Abstract_Error(in)
            {
               wptr = 0;
               delete_wrapper = false;
            }
   
            Abstract_EReadError& operator=(const Abstract_EReadError&) { return *this; }
   
            virtual void init_wrapper()
            {
               std::cerr << "BOSS WARNING: Problem detected with the BOSSed class gm2calc::EReadError from backend gm2calc_1_2_0. The function Abstract_EReadError::init_wrapper() in GAMBIT should never have been called..." << std::endl;
            }
   
            EReadError* get_init_wptr()
            {
               init_wrapper();
               return wptr;
            }
   
            EReadError& get_init_wref()
            {
               init_wrapper();
               return *wptr;
            }
   
            virtual ~Abstract_EReadError()
            {
               std::cerr << "BOSS WARNING: Problem detected with the BOSSed class gm2calc::EReadError from backend gm2calc_1_2_0. The function Abstract_EReadError::~Abstract_EReadError in GAMBIT should never have been called..." << std::endl;
            }
      };
   }
   
}


#include "gambit/Backends/backend_undefs.hpp"


#endif /* __abstract_EReadError_gm2calc_1_2_0_hpp__ */
