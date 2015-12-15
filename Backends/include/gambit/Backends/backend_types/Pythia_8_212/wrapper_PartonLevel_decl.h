#ifndef __wrapper_PartonLevel_decl_Pythia_8_212_h__
#define __wrapper_PartonLevel_decl_Pythia_8_212_h__

#include <cstddef>
#include "forward_decls_wrapper_classes.h"
#include "gambit/Backends/wrapperbase.hpp"
#include "abstract_PartonLevel.h"
#include "wrapper_Info_decl.h"
#include "wrapper_Settings_decl.h"
#include "wrapper_ParticleData_decl.h"
#include "wrapper_Rndm_decl.h"
#include "wrapper_BeamParticle_decl.h"
#include "wrapper_Couplings_decl.h"
#include "wrapper_SigmaTotal_decl.h"
#include "wrapper_UserHooks_decl.h"
#include "wrapper_Event_decl.h"
#include <vector>
#include "wrapper_ResonanceDecays_decl.h"

#include "identification.hpp"

namespace CAT_3(BACKENDNAME,_,SAFE_VERSION)
{
    
    namespace Pythia8
    {
        
        class PartonLevel : public WrapperBase<Pythia8::Abstract_PartonLevel>
        {
            public:
                typedef WrapperBase<Pythia8::Abstract_PartonLevel> wrapperbase;
                using WrapperBase<Pythia8::Abstract_PartonLevel>::BEptr;
        
                // Member variables: 
            public:
                // -- Static factory pointers: 
                static Pythia8::Abstract_PartonLevel* (*__factory0)();
        
                // -- Other member variables: 
        
                // Member functions: 
            public:
                bool next(WrapperBase< Pythia8::Abstract_Event >& process, WrapperBase< Pythia8::Abstract_Event >& event);
        
                void setupShowerSys(WrapperBase< Pythia8::Abstract_Event >& process, WrapperBase< Pythia8::Abstract_Event >& event);
        
                bool resonanceShowers(WrapperBase< Pythia8::Abstract_Event >& process, WrapperBase< Pythia8::Abstract_Event >& event, bool skipForR);
        
                bool wzDecayShowers(WrapperBase< Pythia8::Abstract_Event >& event);
        
                bool hasVetoed() const;
        
                bool hasVetoedDiff() const;
        
                void accumulate();
        
                void statistics(bool reset);
        
                void statistics();
        
                void resetStatistics();
        
                void resetTrial();
        
                double pTLastInShower();
        
                int typeLastInShower();
        
        
                // Wrappers for original constructors: 
            public:
                PartonLevel();
        
                // Special pointer-based constructor: 
                PartonLevel(Pythia8::Abstract_PartonLevel* in);
                PartonLevel(Pythia8::Abstract_PartonLevel* const & in, bool);
        
                // Copy constructor: 
                PartonLevel(const PartonLevel& in);
        
                // Assignment operator: 
                PartonLevel& operator=(const PartonLevel& in);
        
                // Destructor: 
                ~PartonLevel();
        
            private:
                // Member variable initialiser: 
                void _memberVariablesInit();
        
        };
    }
    
}


#include "gambit/Backends/backend_undefs.hpp"

#endif /* __wrapper_PartonLevel_decl_Pythia_8_212_h__ */
