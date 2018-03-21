//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Basic set of known mathematical and physical
///  constants for GAMBIT.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Pat Scott
///          (p.scott@imperial.ac.uk)
///  \date   2015 Mar
///
///  \author Christoph Weniger
///          (c.weniger@uva.nl)
///  \date   2015 Apr
///  \date   2016 Mar
//
///  *********************************************

#ifndef __numerical_constants_hpp__
#define __numerical_constants_hpp__

#include <cmath>

namespace Gambit
{

  const double pi = 3.141592653589793238462643383279502884197;
  const double root2 = sqrt(2.0);
  const double hbar = 6.582119514e-25;                          // GeV s  (http://pdg.lbl.gov/2017/reviews/rpp2017-rev-phys-constants.pdf)
  const double gev2cm2 = pow(197.327053e-16, 2.0);              // cm^2 per GeV^-2
  const double gev2pb = gev2cm2*1e36;                           // pb per GeV^-2
  const double s2cm = 2.99792458e10;                            // cm per s
  const double atomic_mass_unit=0.931494028;                    // atomic mass unit (GeV/c^2)
  const double m_proton_amu = 1.00727646688;                    // proton mass (amu)
  const double m_neutron_amu = 1.0086649156;                    // neutron mass (amu)
  const double m_proton = m_proton_amu * atomic_mass_unit;      // proton mass (GeV/c^2)
  const double m_neutron = m_neutron_amu * atomic_mass_unit;    // neutron mass (GeV/c^2)

  static const struct Mesons_masses
  {
    static constexpr double pi0 = 0.135;          // neutral pion mass (GeV/c^2)
    static constexpr double pi_plus = 0.1396;     // charged pion mass (GeV/c^2)
    static constexpr double pi_minus = 0.1396;    // charged pion mass (GeV/c^2)
    static constexpr double eta = 0.547;          // eta mass (GeV/c^2)
    static constexpr double rho0 = 0.775;         // neutral rho meson mass (GeV/c^2)
    static constexpr double rho_plus = 0.775;     // charged rho meson mass (GeV/c^2)
    static constexpr double rho_minus = 0.775;    // charged rho meson mass (GeV/c^2)
    static constexpr double omega = 0.7827;       // omega meson mass (GeV/c^2)
    static constexpr double kaon_plus = 0.4937;   // charged kaon meson mass (GeV/c^2)
    static constexpr double kaon_minus = 0.4937;  // charged kaon meson mass (GeV/c^2)
    static constexpr double kaon0 = 0.4976;       // neutral kaon meson mass (GeV/c^2)
    static constexpr double rho1450 = 1.465;      // rho(1450) mass (GeV/c^2)
    static constexpr double D_plus = 1.86962;     // charged D meson mass (GeV/c^2)
    static constexpr double D_s = 1.96847;        // D_s meson mass (GeV/c^2)
    static constexpr double B_plus = 5.27929;     // charged B meson mass (GeV/c^2)
    static constexpr double B_s = 5.36679;        // B_s meson mass (GeV/c^2)
    static constexpr double B_c = 6.2751;         // B_c meson mass (GeV/c^2)
    static constexpr double eta_prime = 0.95778;  // eta prime meson mass (GeV/c^2)
  } meson_masses;

  static const struct Mesons_decay_constants
  {
    static constexpr double pi_plus = 0.13041;    // (GeV)
  } meson_decay_constants;


  /// M_W (Breit-Wigner mass parameter ~ pole) = 80.385 +/- 0.015  GeV (1 sigma), Gaussian.
  /// Reference http://pdg.lbl.gov/2014/listings/rpp2014-list-w-boson.pdf = K.A. Olive et al. (Particle Data Group), Chin. Phys. C38, 090001 (2014)
  /// @{
  const double mw_central_observed = 80.385;
  const double mw_err_observed = 0.015;
  /// @}

}

#endif //#defined __numerical_constants_hpp__
