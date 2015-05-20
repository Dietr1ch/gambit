#ifndef __wrapper_CoupSM_decl_Pythia_8_186_h__
#define __wrapper_CoupSM_decl_Pythia_8_186_h__

#include <cstddef>
#include "forward_decls_wrapper_classes.h"
#include "gambit/Backends/wrapperbase.hpp"
#include "abstract_CoupSM.h"
#include "wrapper_Rndm_decl.h"
#include "wrapper_Settings_decl.h"

#include "identification.hpp"

namespace CAT_3(BACKENDNAME,_,SAFE_VERSION)
{
    
    namespace Pythia8
    {
        
        class CoupSM : public WrapperBase<Pythia8::Abstract_CoupSM>
        {
            public:
                typedef WrapperBase<Pythia8::Abstract_CoupSM> wrapperbase;
        
                // Member variables: 
            public:
                // -- Static factory pointers: 
                static Pythia8::Abstract_CoupSM* (*__factory0)();
        
                // -- Other member variables: 
        
                // Member functions: 
            public:
                void init(WrapperBase< Pythia8::Abstract_Settings >& settings, WrapperBase< Pythia8::Abstract_Rndm >* rndmPtrIn);
        
                double alphaS(double scale2);
        
                double alphaS1Ord(double scale2);
        
                double alphaS2OrdCorr(double scale2);
        
                double Lambda3() const;
        
                double Lambda4() const;
        
                double Lambda5() const;
        
                double alphaEM(double scale2);
        
                double sin2thetaW();
        
                double cos2thetaW();
        
                double sin2thetaWbar();
        
                double GF();
        
                double ef(int idAbs);
        
                double vf(int idAbs);
        
                double af(int idAbs);
        
                double t3f(int idAbs);
        
                double lf(int idAbs);
        
                double rf(int idAbs);
        
                double ef2(int idAbs);
        
                double vf2(int idAbs);
        
                double af2(int idAbs);
        
                double efvf(int idAbs);
        
                double vf2af2(int idAbs);
        
                double VCKMgen(int genU, int genD);
        
                double V2CKMgen(int genU, int genD);
        
                double VCKMid(int id1, int id2);
        
                double V2CKMid(int id1, int id2);
        
                double V2CKMsum(int id);
        
                int V2CKMpick(int id);
        
        
                // Wrappers for original constructors: 
            public:
                CoupSM();
        
                // Special pointer-based constructor: 
                CoupSM(Pythia8::Abstract_CoupSM* in);
                CoupSM(Pythia8::Abstract_CoupSM* const & in, bool);
        
                // Copy constructor: 
                CoupSM(const CoupSM& in);
        
                // Assignment operator: 
                CoupSM& operator=(const CoupSM& in);
        
                // Destructor: 
                ~CoupSM();
        
        };
    }
    
}


#include "gambit/Backends/backend_undefs.hpp"

#endif /* __wrapper_CoupSM_decl_Pythia_8_186_h__ */
