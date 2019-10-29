#ifndef __wrapper_Spectrum_generator_settings_def_FlexibleSUSY_CMSSM_2_0_1_h__
#define __wrapper_Spectrum_generator_settings_def_FlexibleSUSY_CMSSM_2_0_1_h__

#include <string>
#include <array>

#include "identification.hpp"

namespace CAT_3(BACKENDNAME,_,SAFE_VERSION)
{
    
    namespace flexiblesusy
    {
        
        // Member functions: 
        inline double Spectrum_generator_settings::get(flexiblesusy::Abstract_Spectrum_generator_settings::Settings arg_1) const
        {
            return get_BEptr()->get(arg_1);
        }
        
        inline ::std::basic_string<char, std::char_traits<char>, std::allocator<char> > Spectrum_generator_settings::get_description(flexiblesusy::Abstract_Spectrum_generator_settings::Settings arg_1) const
        {
            return get_BEptr()->get_description(arg_1);
        }
        
        inline void Spectrum_generator_settings::set(flexiblesusy::Abstract_Spectrum_generator_settings::Settings arg_1, double arg_2)
        {
            get_BEptr()->set(arg_1, arg_2);
        }
        
        inline void Spectrum_generator_settings::reset()
        {
            get_BEptr()->reset();
        }
        
        
        // Wrappers for original constructors: 
        inline flexiblesusy::Spectrum_generator_settings::Spectrum_generator_settings() :
            WrapperBase(__factory0())
        {
            get_BEptr()->set_wptr(this);
            get_BEptr()->set_delete_wrapper(false);
        }
        
        // Special pointer-based constructor: 
        inline flexiblesusy::Spectrum_generator_settings::Spectrum_generator_settings(flexiblesusy::Abstract_Spectrum_generator_settings* in) :
            WrapperBase(in)
        {
            get_BEptr()->set_wptr(this);
            get_BEptr()->set_delete_wrapper(false);
        }
        
        // Copy constructor: 
        inline flexiblesusy::Spectrum_generator_settings::Spectrum_generator_settings(const Spectrum_generator_settings& in) :
            WrapperBase(in.get_BEptr()->pointer_copy__BOSS())
        {
            get_BEptr()->set_wptr(this);
            get_BEptr()->set_delete_wrapper(false);
        }
        
        // Assignment operator: 
        inline flexiblesusy::Spectrum_generator_settings& Spectrum_generator_settings::operator=(const Spectrum_generator_settings& in)
        {
            if (this != &in)
            {
                get_BEptr()->pointer_assign__BOSS(in.get_BEptr());
            }
            return *this;
        }
        
        
        // Destructor: 
        inline flexiblesusy::Spectrum_generator_settings::~Spectrum_generator_settings()
        {
            if (get_BEptr() != 0)
            {
                get_BEptr()->set_delete_wrapper(false);
                if (can_delete_BEptr())
                {
                    delete BEptr;
                    BEptr = 0;
                }
            }
            set_delete_BEptr(false);
        }
        
        // Returns correctly casted pointer to Abstract class: 
        inline flexiblesusy::Abstract_Spectrum_generator_settings* flexiblesusy::Spectrum_generator_settings::get_BEptr() const
        {
            return dynamic_cast<flexiblesusy::Abstract_Spectrum_generator_settings*>(BEptr);
        }
    }
    
}


#include "gambit/Backends/backend_undefs.hpp"

#endif /* __wrapper_Spectrum_generator_settings_def_FlexibleSUSY_CMSSM_2_0_1_h__ */
