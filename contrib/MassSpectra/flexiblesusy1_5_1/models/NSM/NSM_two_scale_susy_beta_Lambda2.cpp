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

// File generated at Sat 27 Aug 2016 12:40:15

#include "NSM_two_scale_susy_parameters.hpp"
#include "wrappers.hpp"

namespace flexiblesusy {

#define INPUT(parameter) input.parameter
#define TRACE_STRUCT susy_traces

/**
 * Calculates the one-loop beta function of Lambda2.
 *
 * @return one-loop beta function
 */
double NSM_susy_parameters::calc_beta_Lambda2_one_loop(const Susy_traces& susy_traces) const
{


   double beta_Lambda2;

   beta_Lambda2 = Re(2*oneOver16PiSqr*(36*Sqr(Lambda2) + Sqr(Lambda3)));


   return beta_Lambda2;
}

/**
 * Calculates the two-loop beta function of Lambda2.
 *
 * @return two-loop beta function
 */
double NSM_susy_parameters::calc_beta_Lambda2_two_loop(const Susy_traces& susy_traces) const
{
   const double traceYdAdjYd = TRACE_STRUCT.traceYdAdjYd;
   const double traceYeAdjYe = TRACE_STRUCT.traceYeAdjYe;
   const double traceYuAdjYu = TRACE_STRUCT.traceYuAdjYu;


   double beta_Lambda2;

   beta_Lambda2 = Re(-4*twoLoop*(816*Power(Lambda2,3) + 4*Power(Lambda3,3
      ) + 3*traceYdAdjYd*Sqr(Lambda3) + traceYeAdjYe*Sqr(Lambda3) + 3*
      traceYuAdjYu*Sqr(Lambda3) + 20*Lambda2*Sqr(Lambda3) - Sqr(g1)*Sqr(Lambda3
      ) - 3*Sqr(g2)*Sqr(Lambda3)));


   return beta_Lambda2;
}

/**
 * Calculates the three-loop beta function of Lambda2.
 *
 * @return three-loop beta function
 */
double NSM_susy_parameters::calc_beta_Lambda2_three_loop(const Susy_traces& susy_traces) const
{
   DEFINE_PROJECTOR(3,3,3,3)



   double beta_Lambda2;

   beta_Lambda2 = 0;


   return beta_Lambda2;
}

} // namespace flexiblesusy
