#ifndef __abstract_ResonanceGmZ_Pythia_8_212_h__
#define __abstract_ResonanceGmZ_Pythia_8_212_h__

#include "gambit/Backends/abstractbase.hpp"
#include "forward_decls_abstract_classes.h"
#include "forward_decls_wrapper_classes.h"
#include "wrapper_ResonanceWidths_decl.h"
#include <cstddef>

#include "identification.hpp"

// Forward declaration needed by the destructor pattern.
void wrapper_deleter(CAT_3(BACKENDNAME,_,SAFE_VERSION)::Pythia8::ResonanceGmZ*);


namespace CAT_3(BACKENDNAME,_,SAFE_VERSION)
{
    
    
    namespace Pythia8
    {
        class Abstract_ResonanceGmZ : virtual public AbstractBase, virtual public Pythia8::Abstract_ResonanceWidths
        {
            public:
    
            public:
                using Pythia8::Abstract_ResonanceWidths::pointerAssign__BOSS;
                virtual void pointerAssign__BOSS(Abstract_ResonanceGmZ*) =0;
                virtual Abstract_ResonanceGmZ* pointerCopy__BOSS() =0;
    
            private:
                mutable ResonanceGmZ* wptr;
    
            public:
                Abstract_ResonanceGmZ()
                {
                }
    
                void wrapper__BOSS(ResonanceGmZ* wptr_in)
                {
                    wptr = wptr_in;
                    is_wrapped(true);
                    can_delete_wrapper(true);
                }
    
                ResonanceGmZ* wrapper__BOSS()
                {
                    return wptr;
                }
    
                virtual ~Abstract_ResonanceGmZ()
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


#endif /* __abstract_ResonanceGmZ_Pythia_8_212_h__ */
