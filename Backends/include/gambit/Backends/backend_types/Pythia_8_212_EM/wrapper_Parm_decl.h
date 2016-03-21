#ifndef __wrapper_Parm_decl_Pythia_8_212_EM_h__
#define __wrapper_Parm_decl_Pythia_8_212_EM_h__

#include <cstddef>
#include "forward_decls_wrapper_classes.h"
#include "gambit/Backends/wrapperbase.hpp"
#include "abstract_Parm.h"
#include <string>

#include "identification.hpp"

namespace CAT_3(BACKENDNAME,_,SAFE_VERSION)
{
    
    namespace Pythia8
    {
        
        class Parm : public WrapperBase<Pythia8::Abstract_Parm>
        {
            public:
                typedef WrapperBase<Pythia8::Abstract_Parm> wrapperbase;
                using WrapperBase<Pythia8::Abstract_Parm>::BEptr;
        
                // Member variables: 
            public:
                // -- Static factory pointers: 
                static Pythia8::Abstract_Parm* (*__factory0)(::std::basic_string<char, std::char_traits<char>, std::allocator<char> >, double, bool, bool, double, double);
                static Pythia8::Abstract_Parm* (*__factory1)(::std::basic_string<char, std::char_traits<char>, std::allocator<char> >, double, bool, bool, double);
                static Pythia8::Abstract_Parm* (*__factory2)(::std::basic_string<char, std::char_traits<char>, std::allocator<char> >, double, bool, bool);
                static Pythia8::Abstract_Parm* (*__factory3)(::std::basic_string<char, std::char_traits<char>, std::allocator<char> >, double, bool);
                static Pythia8::Abstract_Parm* (*__factory4)(::std::basic_string<char, std::char_traits<char>, std::allocator<char> >, double);
                static Pythia8::Abstract_Parm* (*__factory5)(::std::basic_string<char, std::char_traits<char>, std::allocator<char> >);
                static Pythia8::Abstract_Parm* (*__factory6)();
        
                // -- Other member variables: 
            public:
                std::basic_string<char, std::char_traits<char>, std::allocator<char> >& name;
                double& valNow;
                double& valDefault;
                bool& hasMin;
                bool& hasMax;
                double& valMin;
                double& valMax;
        
                // Member functions: 
        
                // Wrappers for original constructors: 
            public:
                Parm(::std::basic_string<char, std::char_traits<char>, std::allocator<char> > nameIn, double defaultIn, bool hasMinIn, bool hasMaxIn, double minIn, double maxIn);
                Parm(::std::basic_string<char, std::char_traits<char>, std::allocator<char> > nameIn, double defaultIn, bool hasMinIn, bool hasMaxIn, double minIn);
                Parm(::std::basic_string<char, std::char_traits<char>, std::allocator<char> > nameIn, double defaultIn, bool hasMinIn, bool hasMaxIn);
                Parm(::std::basic_string<char, std::char_traits<char>, std::allocator<char> > nameIn, double defaultIn, bool hasMinIn);
                Parm(::std::basic_string<char, std::char_traits<char>, std::allocator<char> > nameIn, double defaultIn);
                Parm(::std::basic_string<char, std::char_traits<char>, std::allocator<char> > nameIn);
                Parm();
        
                // Special pointer-based constructor: 
                Parm(Pythia8::Abstract_Parm* in);
                Parm(Pythia8::Abstract_Parm* const & in, bool);
        
                // Copy constructor: 
                Parm(const Parm& in);
        
                // Assignment operator: 
                Parm& operator=(const Parm& in);
        
                // Destructor: 
                ~Parm();
        
            private:
                // Member variable initialiser: 
                void _memberVariablesInit();
        
        };
    }
    
}


#include "gambit/Backends/backend_undefs.hpp"

#endif /* __wrapper_Parm_decl_Pythia_8_212_EM_h__ */
