#ifndef __wrapper_BeamParticle_decl_Pythia_8_212_h__
#define __wrapper_BeamParticle_decl_Pythia_8_212_h__

#include <cstddef>
#include "forward_decls_wrapper_classes.h"
#include "gambit/Backends/wrapperbase.hpp"
#include "abstract_BeamParticle.h"
#include "wrapper_Info_decl.h"
#include "wrapper_ParticleData_decl.h"
#include "wrapper_Rndm_decl.h"
#include "wrapper_Vec4_decl.h"
#include <vector>
#include "wrapper_Settings_decl.h"
#include <ostream>
#include "wrapper_Event_decl.h"

#include "identification.hpp"

namespace CAT_3(BACKENDNAME,_,SAFE_VERSION)
{
    
    namespace Pythia8
    {
        
        class BeamParticle : public WrapperBase<Pythia8::Abstract_BeamParticle>
        {
            public:
                typedef WrapperBase<Pythia8::Abstract_BeamParticle> wrapperbase;
        
                // Member variables: 
            public:
                // -- Static factory pointers: 
                static Pythia8::Abstract_BeamParticle* (*__factory0)();
        
                // -- Other member variables: 
        
                // Member functions: 
            public:
                void newValenceContent();
        
                void newPzE(double pzIn, double eIn);
        
                int id() const;
        
                Pythia8::Vec4 p() const;
        
                double px() const;
        
                double py() const;
        
                double pz() const;
        
                double e() const;
        
                double m() const;
        
                bool isLepton() const;
        
                bool isUnresolved() const;
        
                bool isHadron() const;
        
                bool isMeson() const;
        
                bool isBaryon() const;
        
                double xMax(int iSkip);
        
                double xMax();
        
                double xfHard(int idIn, double x, double Q2);
        
                double xf(int idIn, double x, double Q2);
        
                double xfVal(int idIn, double x, double Q2);
        
                double xfSea(int idIn, double x, double Q2);
        
                double xfMPI(int idIn, double x, double Q2);
        
                double xfISR(int indexMPI, int idIn, double x, double Q2);
        
                bool insideBounds(double x, double Q2);
        
                double alphaS(double Q2);
        
                double mQuarkPDF(int idIn);
        
                int pickValSeaComp();
        
                void initBeamKind();
        
                int size() const;
        
                int sizeInit() const;
        
                void clear();
        
                int append(int iPos, int idIn, double x, int companion);
        
                int append(int iPos, int idIn, double x);
        
                void list(std::basic_ostream<char,std::char_traits<char> >& os) const;
        
                void list() const;
        
                int nValenceKinds() const;
        
                int nValence(int idIn) const;
        
                bool isUnresolvedLepton();
        
                bool remnantFlavours(WrapperBase< Pythia8::Abstract_Event >& event, bool isDIS);
        
                bool remnantFlavours(WrapperBase< Pythia8::Abstract_Event >& event);
        
                bool remnantColours(WrapperBase< Pythia8::Abstract_Event >& event, std::vector<int,std::allocator<int> >& colFrom, std::vector<int,std::allocator<int> >& colTo);
        
                double xRemnant(int i);
        
                bool hasJunction() const;
        
                int junctionCol(int i) const;
        
                void junctionCol(int i, int col);
        
                bool pickGluon(double mDiff);
        
                int pickValence();
        
                int pickRemnant() const;
        
                double zShare(double mDiff, double m1, double m2);
        
                double pxShare() const;
        
                double pyShare() const;
        
                bool remnantFlavoursNew(WrapperBase< Pythia8::Abstract_Event >& event);
        
                void findColSetup(WrapperBase< Pythia8::Abstract_Event >& event);
        
                void setInitialCol(WrapperBase< Pythia8::Abstract_Event >& event);
        
                void updateCol(std::vector<std::pair<int, int>,std::allocator<std::pair<int, int> > > colourChanges);
        
                std::vector<std::pair<int, int>,std::allocator<std::pair<int, int> > > getColUpdates();
        
        
                // Wrappers for original constructors: 
            public:
                BeamParticle();
        
                // Special pointer-based constructor: 
                BeamParticle(Pythia8::Abstract_BeamParticle* in);
                BeamParticle(Pythia8::Abstract_BeamParticle* const & in, bool);
        
                // Copy constructor: 
                BeamParticle(const BeamParticle& in);
        
                // Assignment operator: 
                BeamParticle& operator=(const BeamParticle& in);
        
                // Destructor: 
                ~BeamParticle();
        
            private:
                // Member variable initialiser: 
                void _memberVariablesInit();
        
        };
    }
    
}


#include "gambit/Backends/backend_undefs.hpp"

#endif /* __wrapper_BeamParticle_decl_Pythia_8_212_h__ */
