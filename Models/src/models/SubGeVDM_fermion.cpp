///  GAMBIT: Global and Modular BSM Inference Tool
///  *********************************************
///
///  Model translation functions for the SubGeVDM_fermion model
///
///  *********************************************
///
///  Authors
///  =======
///
///  (add name and date if you modify)
///
///  \author Felix Kahlhoefer
///          (kahlhoefer@kit.edu)
///  \date 2022 May
///
///  *********************************************


#include "gambit/Models/model_macros.hpp"
#include "gambit/Models/model_helpers.hpp"
#include "gambit/Logs/logger.hpp"

#include "gambit/Models/models/SubGeVDM_fermion.hpp"

#define MODEL SubGeVDM_fermion
  void MODEL_NAMESPACE::SubGeVDM_fermion_to_AnnihilatingDM_general (const ModelParameters &myparams, ModelParameters &friendparams)
  {
    USE_MODEL_PIPE(AnnihilatingDM_general) // get pipe for "interpret as friend" function
    logger()<<"Running interpret_as_friend calculations for SubGeVDM_fermion -> AnnihilatingDM_general ..."<<EOM;

    const double k = (*Dep::wimp_sc) ? 1. : 0.5;
    const double f = *Dep::RD_fraction;

    friendparams.setValue("mass", *Dep::mwimp);
    // In AnnihilatingDM_general the parameter "sigmav" is assumed to already include
    // (RD_fraction)^2 and the factor k
    friendparams.setValue("sigmav", k*f*f*(*Dep::sigmav));
  }
#undef MODEL

#define MODEL Resonant_SubGeVDM_fermion
  void MODEL_NAMESPACE::Resonant_SubGeVDM_fermion_to_SubGeVDM_fermion (const ModelParameters &myparams, ModelParameters &friendparams)
  {
    USE_MODEL_PIPE(SubGeVDM_fermion) // get pipe for "interpret as friend" function
    logger()<<"Running interpret_as_friend calculations for Resonant_SubGeVDM_fermion -> SubGeVDM_fermion ..."<<EOM;

    friendparams.setValue("mDM", myparams["mDM"]);
    friendparams.setValue("gDM", myparams["gDM"]);
    friendparams.setValue("kappa", myparams["kappa"]);
    friendparams.setValue("etaDM", myparams["etaDM"]);
    friendparams.setValue("mAp", 2 * myparams["mDM"] * sqrt(myparams["epsR"] + 1));
  }
#undef MODEL
