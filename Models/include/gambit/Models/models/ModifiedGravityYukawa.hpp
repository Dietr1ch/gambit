//  GAMBIT: Global and Modular BSM Inference Tool
//  *********************************************
///
///  Modified gravitational inverse-square law by a new Yukawa interaction
///
///  *********************************************
///
///  Authors
///  =======
///
///  (add name and date if you modify)
///
///  \author Iñigo Saez Casares
///  \date 2020 March
///
///  
///
///  *********************************************

#ifndef __ModifiedGravityYukawa_hpp__
#define __ModifiedGravityYukawa_hpp__

#define MODEL ModifiedGravityYukawa
  START_MODEL
  // Physical units : alpha [dimensionless], lambda [cm]
  DEFINEPARS(alpha) // amplitude of the Yukawa interaction
  DEFINEPARS(lambda) // range of the Yukawa interaction
#undef MODEL

#endif
