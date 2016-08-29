//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  GAMBIT particle database.  Add to this if you
///  need to define a new particle.  
///
///  Single particles can be added with:
///   add_particle("particle_id", (pdg_code, context_int))
///  where context_int is some context-dependent integer
///  that helps identify uni
///
///  Multiple particles with an index can be added with:
///   add_particle_set("X", 
///    (pdg_code_1, context_int_1),
///    (pdg_code_2, context_int_2),
///    (pdg_code_3, context_int_3),
///    ...
///    (pdg_code_n, context_int_n) )
///  This will produce particles with string IDs
///  "X_1", "X_2", "X_3", ..., "X_n".
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///   
///  \author Pat Scott  
///          (p.scott@imperial.ac.uk)
///  \date 2015 Jan
///
///  \author Ankit Beniwal
///          (ankit.beniwal@adelaide.edu.au)
///  \date 2016 Aug
///
///  *********************************************


#include "gambit/Models/partmap.hpp"
#include "gambit/Models/particle_macros.hpp"

namespace Gambit
{

  namespace Models
  {

    void define_particles(partmap* particles)
    {
 
      // ---- GAMBIT generic particle classes (context = 0) ---- 

      add_generic_particle("boson",    (9900081, 0))
      add_generic_particle("fermion",  (9900082, 0))
      add_generic_particle("lepton",   (9900083, 0))
      add_generic_particle("quark",    (9900084, 0))
      add_generic_particle("neutrino", (9900085, 0))
      add_generic_particle("hadron",   (9900086, 0))
      add_generic_particle("meson",    (9900087, 0))
      add_generic_particle("penta",    (9900088, 0))

      // ---- Standard Model gauge/Higgs bosons (context = 0) ----

      add_SM_particle("g",     ( 21, 0) )
      add_SM_particle("gamma", ( 22, 0) )
      add_SM_particle("Z0",    ( 23, 0) )
      add_SM_particle("W+",    ( 24, 0) )
      add_SM_particle("W-",    (-24, 0) )
      add_SM_particle("G",     ( 39, 0) )
      //  SM_particle("h0_1"   ( 25, 0) ) is added in MSSM section below, as it's better not to treat it as SM for most purposes. 

      // ---- Standard Model mass eigenstates (context = 0) ----

      // Mass ordered down and up type quarks
      add_SM_particle_set("d",    (( 1, 0), ( 3, 0), ( 5, 0)) )
      add_SM_particle_set("dbar", ((-1, 0), (-3, 0), (-5, 0)) )
      add_SM_particle_set("u",    (( 2, 0), ( 4, 0), ( 6, 0)) )
      add_SM_particle_set("ubar", ((-2, 0), (-4, 0), (-6, 0)) )
      // Mass ordered charged leptons and neutrinos 
      add_SM_particle_set("e-", (( 11, 0), ( 13, 0), ( 15, 0)) )
      add_SM_particle_set("e+", ((-11, 0), (-13, 0), (-15, 0)) )
      add_SM_particle_set("nu", (( 12, 0), ( 14, 0), ( 16, 0)) )
      add_SM_particle_set("nubar", (( -12, 0), ( -14, 0), ( -16, 0)) )

      // ---- Standard Model flavour eigenstates (context = 1) ----

      // Quarks
      add_SM_particle("d",     (  1, 1) )
      add_SM_particle("u",     (  2, 1) )
      add_SM_particle("s",     (  3, 1) )
      add_SM_particle("c",     (  4, 1) )
      add_SM_particle("b",     (  5, 1) )
      add_SM_particle("t",     (  6, 1) )
      add_SM_particle("dbar",  ( -1, 1) )
      add_SM_particle("ubar",  ( -2, 1) )
      add_SM_particle("sbar",  ( -3, 1) )
      add_SM_particle("cbar",  ( -4, 1) )
      add_SM_particle("bbar",  ( -5, 1) )
      add_SM_particle("tbar",  ( -6, 1) )
      // Leptons
      add_SM_particle("e-",    ( 11, 1) )
      add_SM_particle("mu-",   ( 13, 1) )
      add_SM_particle("tau-",  ( 15, 1) )
      add_SM_particle("e+",    (-11, 1) )
      add_SM_particle("mu+",   (-13, 1) )
      add_SM_particle("tau+",  (-15, 1) )
      // Neutrinos
      add_SM_particle("nu_e"  , (12, 1) )
      add_SM_particle("nu_mu" , (14, 1) )
      add_SM_particle("nu_tau", (16, 1) )
      add_SM_particle("nubar_e"  , (-12, 1) )
      add_SM_particle("nubar_mu" , (-14, 1) )
      add_SM_particle("nubar_tau", (-16, 1) )

      // ---- Standard Model significant mesons (context = 0) ----

      add_SM_particle("pi0",   ( 111, 0) ) 
      add_SM_particle("pi+",   ( 211, 0) ) 
      add_SM_particle("pi-",   (-211, 0) ) 
      add_SM_particle("eta",   ( 221, 0) ) 
      add_SM_particle("rho0",  ( 113, 0) ) 
      add_SM_particle("rho+",  ( 213, 0) ) 
      add_SM_particle("rho-",  (-213, 0) ) 
      add_SM_particle("omega", ( 223, 0) ) 

      // ---- MSSM sparticle mass eigenstates (context = 0) ---- (TODO to be extended to NMSSM)
      // Defined according to SLHA2 (http://arxiv.org/pdf/0801.0045v3.pdf, see eq. 28 - 31)

      // Gluino and gravitino
      add_particle("~g", (1000021,0) )
      add_particle("~G", (1000039,0) )

      // Mass-ordered neutral, pseudoscalar, and charged Higgs bosons
      // PDG 25 is also the SM Higgs boson, but in the MSSM it is not always the most SM-like.
      add_particle_set("h0", ((25, 0), (35, 0)) )
      add_particle("A0", ( 36, 0) )
      add_particle("H+", ( 37, 0) )
      add_particle("H-", (-37, 0) )

      // Mass-ordered down and up-type squarks
      add_particle_set("~d",    (( 1000001, 0), ( 1000003, 0), ( 1000005, 0),
                                 ( 2000001, 0), ( 2000003, 0), ( 2000005, 0)) )
      add_particle_set("~u",    (( 1000002, 0), ( 1000004, 0), ( 1000006, 0),
                                 ( 2000002, 0), ( 2000004, 0), ( 2000006, 0)) )
      add_particle_set("~dbar", ((-1000001, 0), (-1000003, 0), (-1000005, 0),
                                 (-2000001, 0), (-2000003, 0), (-2000005, 0)) )
      add_particle_set("~ubar", ((-1000002, 0), (-1000004, 0), (-1000006, 0),
                                 (-2000002, 0), (-2000004, 0), (-2000006, 0)) )

      // Mass-ordered sleptons and sneutrinos 
      add_particle_set("~e-",    (( 1000011, 0), ( 1000013, 0), ( 1000015, 0),
                                  ( 2000011, 0), ( 2000013, 0), ( 2000015, 0)) )
      add_particle_set("~e+",    ((-1000011, 0), (-1000013, 0), (-1000015, 0),
                                  (-2000011, 0), (-2000013, 0), (-2000015, 0)) )
      add_particle_set("~nu",    (( 1000012, 0), ( 1000014, 0), ( 1000016, 0)) )
      add_particle_set("~nubar", ((-1000012, 0), (-1000014, 0), (-1000016, 0)) )

      // Mass-ordered charginos and neutralinos
      add_particle_set("~chi0", (( 1000022, 0), ( 1000023, 0), (1000025, 0), (1000035, 0)) )
      add_particle_set("~chi+", (( 1000024, 0), ( 1000037, 0)) )
      add_particle_set("~chi-", ((-1000024, 0), (-1000037, 0)) )

      // ---- RPV NMSSM mass eigenstates (context = 10) ---- //TODO not yet totally complete?

      // Example extension of neutrino set to account for mixing with NMSSM neutralinos
      add_particle_set("nu_RPV", ((12, 10), (14, 10), (16, 10), 
           (1000022, 10), (1000023, 10), (1000025, 10), (1000035, 10), (1000045, 10)) )


      // ---- Scalar singlet dark matter ----

      add_particle("S", (9900001, 0))

      // ---- Vector dark matter ----

      add_particle("V", (9900002, 0))

      // ---- Majorana fermion dark matter ----

      add_particle("X", (9900003, 0))

      // ---- Dirac fermion dark matter ----
	
      add_particle("F", (9900004,0)) 
    }

  }

}

