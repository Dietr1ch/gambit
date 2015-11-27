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

// File generated at Wed 28 Oct 2015 11:35:50

#ifndef SSM_INPUT_PARAMETERS_H
#define SSM_INPUT_PARAMETERS_H

#include <complex>
#include <Eigen/Core>

namespace flexiblesusy {

struct SSM_input_parameters {
   double Qin;
   double QEWSB;
   double Lambdainput;
   double LambdaSinput;
   double MSinput;
   double K1input;
   double K2input;

   SSM_input_parameters()
      : Qin(0), QEWSB(0), Lambdainput(0), LambdaSinput(0), MSinput(0), K1input(0),
   K2input(0)

   {}
};

std::ostream& operator<<(std::ostream&, const SSM_input_parameters&);

} // namespace flexiblesusy

#endif
