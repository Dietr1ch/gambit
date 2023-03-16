#ifndef __wrapper_DM_Detector_Ionization_ER_decl_obscura_1_0_2_hpp__
#define __wrapper_DM_Detector_Ionization_ER_decl_obscura_1_0_2_hpp__

#include <cstddef>
#include <string>
#include <vector>
#include "forward_decls_wrapper_classes.hpp"
#include "gambit/Backends/wrapperbase.hpp"
#include "abstract_DM_Detector_Ionization_ER.hpp"
#include "wrapper_DM_Detector_Ionization_decl.hpp"
#include "wrapper_DM_Particle_decl.hpp"
#include "wrapper_DM_Distribution_decl.hpp"

#include "identification.hpp"

namespace CAT_3(BACKENDNAME,_,SAFE_VERSION)
{
    
    namespace obscura
    {
        
        class DM_Detector_Ionization_ER : public DM_Detector_Ionization
        {
                // Member variables: 
            public:
                // -- Static factory pointers: 
                static Abstract_DM_Detector_Ionization_ER* (*__factory0)();
                static Abstract_DM_Detector_Ionization_ER* (*__factory1)(::std::basic_string<char>, double, ::std::basic_string<char>);
                static Abstract_DM_Detector_Ionization_ER* (*__factory2)(::std::basic_string<char>, double, ::std::vector<std::basic_string<char>>, ::std::vector<double>);
                static Abstract_DM_Detector_Ionization_ER* (*__factory3)(::std::basic_string<char>, double, ::std::vector<std::basic_string<char>>);
        
                // -- Other member variables: 
        
                // Member functions: 
        
                // Wrappers for original constructors: 
            public:
                DM_Detector_Ionization_ER();
                DM_Detector_Ionization_ER(::std::basic_string<char> label, double expo, ::std::basic_string<char> atom);
                DM_Detector_Ionization_ER(::std::basic_string<char> label, double expo, ::std::vector<std::basic_string<char>> atoms, ::std::vector<double> mass_fractions);
                DM_Detector_Ionization_ER(::std::basic_string<char> label, double expo, ::std::vector<std::basic_string<char>> atoms);
        
                // Special pointer-based constructor: 
                DM_Detector_Ionization_ER(Abstract_DM_Detector_Ionization_ER* in);
        
                // Copy constructor: 
                DM_Detector_Ionization_ER(const DM_Detector_Ionization_ER& in);
        
                // Assignment operator: 
                DM_Detector_Ionization_ER& operator=(const DM_Detector_Ionization_ER& in);
        
                // Destructor: 
                ~DM_Detector_Ionization_ER();
        
                // Returns correctly casted pointer to Abstract class: 
                Abstract_DM_Detector_Ionization_ER* get_BEptr() const;
        
        };
    }
    
}


#include "gambit/Backends/backend_undefs.hpp"

#endif /* __wrapper_DM_Detector_Ionization_ER_decl_obscura_1_0_2_hpp__ */
