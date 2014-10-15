#ifndef __ABSTRACT_VEC4_BOSSedPythia_1_0_H__
#define __ABSTRACT_VEC4_BOSSedPythia_1_0_H__

#include "abstractbase.hpp"
#include "forward_decls_abstract_classes.h"
#include "forward_decls_wrapper_classes.h"

#include "identification.hpp"

// Forward declaration needed by the destructor pattern.
void wrapper_deleter(CAT_3(BACKENDNAME,_,SAFE_VERSION)::Pythia8::Vec4*);


namespace CAT_3(BACKENDNAME,_,SAFE_VERSION)
{
    
    
    namespace Pythia8
    {
        class Abstract_Vec4 : virtual public AbstractBase
        {
            private:
                // IGNORED: Variable  -- Name: TINY  -- XML id: _22813
                // IGNORED: Field  -- Name: xx  -- XML id: _22814
                // IGNORED: Field  -- Name: yy  -- XML id: _22815
                // IGNORED: Field  -- Name: zz  -- XML id: _22816
                // IGNORED: Field  -- Name: tt  -- XML id: _22817
            public:
    
                virtual Pythia8::Abstract_Vec4* operator_equal__BOSS(const Pythia8::Abstract_Vec4&) =0;
    
                virtual Pythia8::Abstract_Vec4* operator_equal__BOSS(double) =0;
    
                virtual void reset() =0;
    
                virtual void p(double, double, double, double) =0;
    
                virtual void p__BOSS(Pythia8::Abstract_Vec4&) =0;
    
                virtual void px(double) =0;
    
                virtual void py(double) =0;
    
                virtual void pz(double) =0;
    
                virtual void e(double) =0;
    
                virtual double px() const =0;
    
                virtual double py() const =0;
    
                virtual double pz() const =0;
    
                virtual double e() const =0;
    
                virtual double mCalc() const =0;
    
                virtual double m2Calc() const =0;
    
                virtual double pT() const =0;
    
                virtual double pT2() const =0;
    
                virtual double pAbs() const =0;
    
                virtual double pAbs2() const =0;
    
                virtual double eT() const =0;
    
                virtual double eT2() const =0;
    
                virtual double theta() const =0;
    
                virtual double phi() const =0;
    
                virtual double thetaXZ() const =0;
    
                virtual double pPos() const =0;
    
                virtual double pNeg() const =0;
    
                virtual double rap() const =0;
    
                virtual double eta() const =0;
    
                virtual void rescale3(double) =0;
    
                virtual void rescale4(double) =0;
    
                virtual void flip3() =0;
    
                virtual void flip4() =0;
    
                virtual void rot(double, double) =0;
    
                virtual void rotaxis(double, double, double, double) =0;
    
                virtual void rotaxis__BOSS(double, const Pythia8::Abstract_Vec4&) =0;
    
                virtual void bst(double, double, double) =0;
    
                virtual void bst(double, double, double, double) =0;
    
                virtual void bst__BOSS(const Pythia8::Abstract_Vec4&) =0;
    
                virtual void bst__BOSS(const Pythia8::Abstract_Vec4&, double) =0;
    
                virtual void bstback__BOSS(const Pythia8::Abstract_Vec4&) =0;
    
                virtual void bstback__BOSS(const Pythia8::Abstract_Vec4&, double) =0;
    
                virtual Pythia8::Abstract_Vec4* operator_minus__BOSS() =0;
    
                virtual Pythia8::Abstract_Vec4* operator_plus_equal__BOSS(const Pythia8::Abstract_Vec4&) =0;
    
                virtual Pythia8::Abstract_Vec4* operator_minus_equal__BOSS(const Pythia8::Abstract_Vec4&) =0;
    
                virtual Pythia8::Abstract_Vec4* operator_asterix_equal__BOSS(double) =0;
    
                virtual Pythia8::Abstract_Vec4* operator_slash_equal__BOSS(double) =0;
    
            public:
                virtual void pointerAssign__BOSS(Abstract_Vec4*) =0;
                virtual Abstract_Vec4* pointerCopy__BOSS() =0;
    
            private:
                Vec4* wptr;
    
            public:
                void wrapper__BOSS(Vec4* wptr_in)
                {
                    wptr = wptr_in;
                    is_wrapped(true);
                }
    
                Vec4* wrapper__BOSS()
                {
                    return wptr;
                }
    
                virtual ~Abstract_Vec4()
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


#include "backend_undefs.hpp"


#endif /* __ABSTRACT_VEC4_BOSSedPythia_1_0_H__ */
