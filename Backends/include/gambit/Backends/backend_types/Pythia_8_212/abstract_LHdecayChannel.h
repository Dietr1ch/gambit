#ifndef __abstract_LHdecayChannel_Pythia_8_212_h__
#define __abstract_LHdecayChannel_Pythia_8_212_h__

#include "gambit/Backends/abstractbase.hpp"
#include "forward_decls_abstract_classes.h"
#include "forward_decls_wrapper_classes.h"
#include <vector>
#include <cstddef>

#include "identification.hpp"

// Forward declaration needed by the destructor pattern.
void wrapper_deleter(CAT_3(BACKENDNAME,_,SAFE_VERSION)::Pythia8::LHdecayChannel*);


namespace CAT_3(BACKENDNAME,_,SAFE_VERSION)
{
    
    
    namespace Pythia8
    {
        class Abstract_LHdecayChannel : virtual public AbstractBase
        {
            public:
    
                virtual void setChannel(double, int, std::vector<int, std::allocator<int> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> >) =0;
    
                virtual void setChannel__BOSS(double, int, std::vector<int, std::allocator<int> >) =0;
    
                virtual void setBrat(double) =0;
    
                virtual void setIdDa(std::vector<int, std::allocator<int> >) =0;
    
                virtual double getBrat() =0;
    
                virtual int getNDa() =0;
    
                virtual std::vector<int, std::allocator<int> > getIdDa() =0;
    
                virtual std::basic_string<char, std::char_traits<char>, std::allocator<char> > getComment() =0;
    
            public:
                virtual void pointerAssign__BOSS(Abstract_LHdecayChannel*) =0;
                virtual Abstract_LHdecayChannel* pointerCopy__BOSS() =0;
    
            private:
                mutable LHdecayChannel* wptr;
    
            public:
                Abstract_LHdecayChannel()
                {
                }
    
                void wrapper__BOSS(LHdecayChannel* wptr_in)
                {
                    wptr = wptr_in;
                    is_wrapped(true);
                    can_delete_wrapper(true);
                }
    
                LHdecayChannel* wrapper__BOSS()
                {
                    return wptr;
                }
    
                virtual ~Abstract_LHdecayChannel()
                {
                    if (can_delete_wrapper())
                    {
                        can_delete_me(false);
                        wrapper_deleter(wptr);
                    }
                }
        };
    }
    
}


#include "gambit/Backends/backend_undefs.hpp"


#endif /* __abstract_LHdecayChannel_Pythia_8_212_h__ */
