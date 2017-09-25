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

// File generated at Sun 24 Sep 2017 15:55:25

/**
 * @file SingletDMZ3_a_muon.hpp
 *
 * This file was generated at Sun 24 Sep 2017 15:55:25 with FlexibleSUSY
 * 2.0.0-dev and SARAH 4.11.0 .
 */

#ifndef SingletDMZ3_A_MUON_H
#define SingletDMZ3_A_MUON_H

namespace flexiblesusy {
class SingletDMZ3_mass_eigenstates;

namespace SingletDMZ3_a_muon {
/**
* @fn calculate_a_muon
* @brief Calculates \f$a_\mu = (g-2)_\mu/2\f$ of the muon.
*/
double calculate_a_muon(const SingletDMZ3_mass_eigenstates& model);

/**
* @fn calculate_a_muon_uncertainty
* @brief Calculates \f$\Delta a_\mu\f$ of the muon.
*/
double calculate_a_muon_uncertainty(const SingletDMZ3_mass_eigenstates& model);
} // namespace SingletDMZ3_a_muon
} // namespace flexiblesusy

#endif
