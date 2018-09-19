//  GAMBIT: Global and Modular BSM Inference Tool
//  *********************************************
//
//  Majorana DM
//
//  *********************************************
//
//  Authors
//  =======
//
//  (add name and date if you modify)
//
//  Ankit Beniwal
//  2016 August, 2017 June
//
//  Sebastian Wild
//  2018 January
//
//  Sanjay Bloor
//  2018 August
//
//  *********************************************

#ifndef __MajoranaSingletDM_Z2_hpp__
#define __MajoranaSingletDM_Z2_hpp__

#define MODEL MajoranaSingletDM_Z2
  START_MODEL
  DEFINEPARS(mX, lX, xi)
#undef MODEL

#define MODEL MajoranaSingletDM_Z2_sps
#define PARENT MajoranaSingletDM_Z2
  START_MODEL
  DEFINEPARS(mX, lX_s, lX_ps)
  INTERPRET_AS_PARENT_FUNCTION(MajoranaSingletDM_Z2_sps_to_MajoranaSingletDM_Z2)
#undef PARENT
#undef MODEL

#endif
