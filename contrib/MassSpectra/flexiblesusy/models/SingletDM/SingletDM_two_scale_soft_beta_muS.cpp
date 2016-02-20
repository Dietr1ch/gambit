// ====================================================================
// This file is part of FlexibleSUSY.
//
// FlexibleSUSY is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.
//
// FlexibleSUSY is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with FlexibleSUSY.  If not, see
// <http://www.gnu.org/licenses/>.
// ====================================================================

// File generated at Sat 20 Feb 2016 16:21:18

#include "SingletDM_two_scale_soft_parameters.hpp"
#include "wrappers.hpp"

namespace flexiblesusy {

#define INPUT(parameter) input.parameter
#define TRACE_STRUCT soft_traces

/**
 * Calculates the one-loop beta function of muS.
 *
 * @return one-loop beta function
 */
double SingletDM_soft_parameters::calc_beta_muS_one_loop(const Soft_traces& soft_traces) const
{


   double beta_muS;

   beta_muS = Re((-2*LamSH*muH + 6*LamS*muS)*oneOver16PiSqr);


   return beta_muS;
}

/**
 * Calculates the two-loop beta function of muS.
 *
 * @return two-loop beta function
 */
double SingletDM_soft_parameters::calc_beta_muS_two_loop(const Soft_traces& soft_traces) const
{
   const double traceYdAdjYd = TRACE_STRUCT.traceYdAdjYd;
   const double traceYeAdjYe = TRACE_STRUCT.traceYeAdjYe;
   const double traceYuAdjYu = TRACE_STRUCT.traceYuAdjYu;


   double beta_muS;

   beta_muS = Re(twoLoop*(12*LamSH*muH*traceYdAdjYd + 4*LamSH*muH*
      traceYeAdjYe + 12*LamSH*muH*traceYuAdjYu - 2.4*LamSH*muH*Sqr(g1) - 12*
      LamSH*muH*Sqr(g2) - 30*muS*Sqr(LamS) + 4*muH*Sqr(LamSH) - 2*muS*Sqr(LamSH
      )));


   return beta_muS;
}

/**
 * Calculates the three-loop beta function of muS.
 *
 * @return three-loop beta function
 */
double SingletDM_soft_parameters::calc_beta_muS_three_loop(const Soft_traces& soft_traces) const
{
   DEFINE_PROJECTOR(3,3,3,3)



   double beta_muS;

   beta_muS = 0;


   return beta_muS;
}

} // namespace flexiblesusy
