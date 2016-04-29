#ifndef __abstract_Rndm_Pythia_8_212_EM_h__
#define __abstract_Rndm_Pythia_8_212_EM_h__

#include "gambit/Backends/abstractbase.hpp"
#include "forward_decls_abstract_classes.h"
#include "forward_decls_wrapper_classes.h"
#include <utility>
#include <vector>
#include <string>
#include <cstddef>
#include <iostream>

#include "identification.hpp"

namespace CAT_3(BACKENDNAME,_,SAFE_VERSION)
{
    
    
    namespace Pythia8
    {
        class Abstract_Rndm : public virtual AbstractBase
        {
            public:
    
                virtual void init(int) =0;
    
                virtual void init__BOSS() =0;
    
                virtual double flat() =0;
    
                virtual double exp() =0;
    
                virtual double xexp() =0;
    
                virtual double gauss() =0;
    
                virtual ::std::pair<double, double> gauss2() =0;
    
                virtual int pick(const ::std::vector<double, std::allocator<double> >&) =0;
    
                virtual bool dumpState(::std::basic_string<char, std::char_traits<char>, std::allocator<char> >) =0;
    
                virtual bool readState(::std::basic_string<char, std::char_traits<char>, std::allocator<char> >) =0;
    
            public:
                virtual void pointer_assign__BOSS(Abstract_Rndm*) =0;
                virtual Abstract_Rndm* pointer_copy__BOSS() =0;
    
            private:
                Rndm* wptr;
                bool delete_wrapper;
            public:
                Rndm* get_wptr() { return wptr; }
                void set_wptr(Rndm* wptr_in) { wptr = wptr_in; }
                bool get_delete_wrapper() { return delete_wrapper; }
                void set_delete_wrapper(bool del_wrp_in) { delete_wrapper = del_wrp_in; }
    
            public:
                Abstract_Rndm()
                {
                    wptr = 0;
                    delete_wrapper = false;
                }
    
                Abstract_Rndm(const Abstract_Rndm&)
                {
                    wptr = 0;
                    delete_wrapper = false;
                }
    
                Abstract_Rndm& operator=(const Abstract_Rndm&) { return *this; }
    
                virtual void init_wrapper()
                {
                    std::cerr << "BOSS WARNING: Problem detected with the BOSSed class Pythia8::Rndm from backend Pythia_8_212_EM. The function Abstract_Rndm::init_wrapper() in GAMBIT should never have been called..." << std::endl;
                }
    
                Rndm* get_init_wptr()
                {
                    init_wrapper();
                    return wptr;
                }
    
                Rndm& get_init_wref()
                {
                    init_wrapper();
                    return *wptr;
                }
    
                virtual ~Abstract_Rndm()
                {
                    std::cerr << "BOSS WARNING: Problem detected with the BOSSed class Pythia8::Rndm from backend Pythia_8_212_EM. The function Abstract_Rndm::~Abstract_Rndm in GAMBIT should never have been called..." << std::endl;
                }
        };
    }
    
}


#include "gambit/Backends/backend_undefs.hpp"


#endif /* __abstract_Rndm_Pythia_8_212_EM_h__ */
