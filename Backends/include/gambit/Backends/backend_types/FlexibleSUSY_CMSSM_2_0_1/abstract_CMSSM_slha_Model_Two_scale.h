#ifndef __abstract_CMSSM_slha_Model_Two_scale_FlexibleSUSY_CMSSM_2_0_1_h__
#define __abstract_CMSSM_slha_Model_Two_scale_FlexibleSUSY_CMSSM_2_0_1_h__

#include "gambit/Backends/abstractbase.hpp"
#include "forward_decls_abstract_classes.h"
#include "forward_decls_wrapper_classes.h"
#include <cstddef>
#include <iostream>

#include "identification.hpp"

namespace CAT_3(BACKENDNAME,_,SAFE_VERSION)
{
    
    
    namespace flexiblesusy
    {
        class Abstract_CMSSM_slha_Model_Two_scale : public virtual AbstractBase
        {
    
            public:
                virtual void pointer_assign__BOSS(Abstract_CMSSM_slha_Model_Two_scale*) =0;
                virtual Abstract_CMSSM_slha_Model_Two_scale* pointer_copy__BOSS() =0;
    
            private:
                CMSSM_slha_Model_Two_scale* wptr;
                bool delete_wrapper;
            public:
                CMSSM_slha_Model_Two_scale* get_wptr() { return wptr; }
                void set_wptr(CMSSM_slha_Model_Two_scale* wptr_in) { wptr = wptr_in; }
                bool get_delete_wrapper() { return delete_wrapper; }
                void set_delete_wrapper(bool del_wrp_in) { delete_wrapper = del_wrp_in; }
    
            public:
                Abstract_CMSSM_slha_Model_Two_scale()
                {
                    wptr = 0;
                    delete_wrapper = false;
                }
    
                Abstract_CMSSM_slha_Model_Two_scale(const Abstract_CMSSM_slha_Model_Two_scale&)
                {
                    wptr = 0;
                    delete_wrapper = false;
                }
    
                Abstract_CMSSM_slha_Model_Two_scale& operator=(const Abstract_CMSSM_slha_Model_Two_scale&) { return *this; }
    
                virtual void init_wrapper() =0;
    
                CMSSM_slha_Model_Two_scale* get_init_wptr()
                {
                    init_wrapper();
                    return wptr;
                }
    
                CMSSM_slha_Model_Two_scale& get_init_wref()
                {
                    init_wrapper();
                    return *wptr;
                }
    
                virtual ~Abstract_CMSSM_slha_Model_Two_scale() =0;
        };
    }
    
}


#include "gambit/Backends/backend_undefs.hpp"


#endif /* __abstract_CMSSM_slha_Model_Two_scale_FlexibleSUSY_CMSSM_2_0_1_h__ */
