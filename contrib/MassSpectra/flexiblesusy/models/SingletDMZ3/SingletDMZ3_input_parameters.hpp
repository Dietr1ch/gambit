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

// File generated at Mon 22 Feb 2016 17:30:33

#ifndef SingletDMZ3_INPUT_PARAMETERS_H
#define SingletDMZ3_INPUT_PARAMETERS_H

#include <complex>
#include <Eigen/Core>

namespace flexiblesusy {

struct SingletDMZ3_input_parameters {
   double HiggsIN;
   double LamSHInput;
   double LamSInput;
   double muSInput;
   double mu3Input;
   double QEWSB;
   double Qin;

   SingletDMZ3_input_parameters()
      : HiggsIN(0), LamSHInput(0), LamSInput(0), muSInput(0), mu3Input(0), QEWSB(0
   ), Qin(0)

   {}
};

std::ostream& operator<<(std::ostream&, const SingletDMZ3_input_parameters&);

} // namespace flexiblesusy

#endif
