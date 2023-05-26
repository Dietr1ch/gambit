//  GAMBIT: Global and Modular BSM Inference Tool
//  *********************************************
///  \file
///
///  declaration for scanner module
///
///  *********************************************
///
///  Authors (add name and date if you modify):
//
///  \author Gregory Martinez
///          (gregory.david.martinez@gmail.com)
///  \date Feb 2014
///
///  *********************************************

#ifndef DUMMY_PRIOR_HPP
#define DUMMY_PRIOR_HPP

#include <algorithm>

#include "gambit/ScannerBit/priors.hpp"


namespace Gambit
{
    namespace Priors
    {
        class Dummy : public BasePrior
        {
        private:

        public:
            // Constructor
            Dummy(const std::vector<std::string>& param, const Options&) : BasePrior(param, param.size())
            {
            }
            
            double log_prior_density(hyper_cube<double>) const override { return 1.; }

            void transform(hyper_cube<double> unitpars, std::unordered_map<std::string, double> &outputMap) const override
            {
                //auto it_vec = unitpars.begin();
                //for (auto it = param_names.begin(), end = param_names.end(); it != end; it++)
                //{
                //    outputMap[*it] = *(it_vec++);
                //}
                for (int i = 0, end = unitpars.size(); i < end; ++i)
                    outputMap[param_names[i]] = unitpars[i];
            }

            void inverse_transform(const std::unordered_map<std::string, double> &physical, hyper_cube<double> unit) const override
            {
                //std::vector<double> u;
                //for (const auto& n : param_names)
                //{
                //    u.push_back(physical.at(n));
                //}
                //return u;
                
                for (int i = 0, end = this->size(); i < end; ++i)
                {
                    unit[i] = physical.at(param_names[i]);
                }
            }

        };

        class None : public BasePrior
        {
        private:

        public:
            None(const std::vector<std::string>& param, const Options&) : BasePrior(param)
            {
            }
            
            double log_prior_density(hyper_cube<double>) const override
            {
                scan_err << "'None' prior has no density" << scan_end;
                return 0.0;
            }

            void transform(hyper_cube<double>, std::unordered_map<std::string, double> &outputMap) const override
            {
                for (auto it = param_names.begin(), end = param_names.end(); it != end; it++)
                {
                    if (outputMap.find(*it) == outputMap.end())
                    {
                        scan_err << "Parameter " << *it << " prior is specified as 'none'"
                                 << " and the scanner has not inputed a value for it."
                                 << scan_end;
                    }
                }
            }

            void inverse_transform(const std::unordered_map<std::string, double> &, hyper_cube<double>) const override
            {
                scan_err << "'None' prior has no inverse transform" << scan_end;
                //return {};
            }


        };

        LOAD_PRIOR(dummy, Dummy)
        LOAD_PRIOR(none, None)
    }
}

#endif
