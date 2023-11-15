//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Header file for SubGeVDM models, includes
///  - SubGeVDM_fermion
///  - Resonant_SubGeVDM_fermion
///  - Resonant_SubGeVDM_fermion_sigma
///  - SubGeVDM_scalar
///
///  Authors (add name and date if you modify):
///
///  \author The GAMBIT Collaboration
///  \date 12:32PM on October 15, 2019
///
///  \author Felix Kahlhoefer
///         (kahlhoefer@kit.edu)
///  \date 2022 May
///
///  *********************************************

#ifndef __SubGeVDM_fermion_hpp__
#define __SubGeVDM_fermion_hpp__

// Make sure that AnnihilatingDM_general is declared first
#include "gambit/Models/models/CosmoEnergyInjection.hpp"

#define MODEL SubGeVDM_fermion
  START_MODEL

  DEFINEPARS(mDM,mAp,gDM,kappa,etaDM)

  // In order to enable CMB constraints create a friendship relation
  // to the s-wave annihilation "marker" model AnnihilatingDM_general
  INTERPRET_AS_X_FUNCTION(AnnihilatingDM_general,SubGeVDM_fermion_to_AnnihilatingDM_general)
  INTERPRET_AS_X_DEPENDENCY(AnnihilatingDM_general,mwimp,double)
  INTERPRET_AS_X_DEPENDENCY(AnnihilatingDM_general,wimp_sc,bool)
  INTERPRET_AS_X_DEPENDENCY(AnnihilatingDM_general,sigmav,double)
  INTERPRET_AS_X_DEPENDENCY(AnnihilatingDM_general,RD_fraction,double)

#undef MODEL

#define MODEL SubGeVDM_fermion_sigmae
#define PARENT SubGeVDM_fermion
  START_MODEL

  DEFINEPARS(mDM,mAp,sigmae,kappa,etaDM)

  // Translate to "SubGeVDM_fermion" (Mapping is defined in SubGeVDM.cpp)
  INTERPRET_AS_PARENT_FUNCTION(SubGeVDM_fermion_sigmae_to_SubGeVDM_fermion)
#undef PARENT
#undef MODEL


#define MODEL Resonant_SubGeVDM_fermion
  START_MODEL

  DEFINEPARS(mDM,epsR,gDM,kappa,etaDM)

  // Friendship with "SubGeVDM_fermion" (Mapping is defined in SubGeVDM_fermion.cpp)
  INTERPRET_AS_X_FUNCTION(SubGeVDM_fermion,Resonant_SubGeVDM_fermion_to_SubGeVDM_fermion)
#undef MODEL

#define MODEL SubGeVDM_scalar
  START_MODEL

  DEFINEPARS(mDM,mAp,gDM,kappa,etaDM)
#undef MODEL

#endif
