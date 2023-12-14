///  GAMBIT: Global and Modular BSM Inference Tool
///  *********************************************
///
///  Model translation functions for the SubGeVDM models
///
///  Contains the interpret-as-parent translation
///  functions for:
///
///  SubGeVDM_fermion_sigmae   --> SubGeVDM_fermion
///
///  As well as the interpret-as-friend translation
///
///  SubGeVDM_fermion          --> AnnihilatingDM_general
///  Resonant_SubGeVDM_fermion --> SubGeVDM_fermion
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
///  \author Tomas Gonzalo
///          (tomas.gonzalo@kit.edu)
///  \date 2023 Oct
///
///  *********************************************


#include "gambit/Models/model_macros.hpp"
#include "gambit/Models/model_helpers.hpp"
#include "gambit/Logs/logger.hpp"
#include "gambit/Utils/numerical_constants.hpp"

#include "gambit/Models/models/SubGeVDM.hpp"

#define MODEL SubGeVDM_fermion
  void MODEL_NAMESPACE::SubGeVDM_fermion_to_AnnihilatingDM_general (const ModelParameters &, ModelParameters &friendparams)
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

#define MODEL SubGeVDM_fermion_sigmae
#define PARENT SubGeVDM_fermion
  void MODEL_NAMESPACE::CAT_3(MODEL,_to_,PARENT) (const ModelParameters &myP, ModelParameters &targetP)
  {
    logger()<<"Running interpret_as_parent calculations for " STRINGIFY(MODEL) " --> " STRINGIFY(PARENT) "."<<LogTags::info<<EOM;

    USE_MODEL_PIPE(PARENT) // get pipe for "interpret as PARENT" function

    double mAp = myP["mAp"];
    double mDM = myP["mDM"];
    double kappa = myP["kappa"];
    double sigmae = myP["sigmae"];

    double reduced_mass = mDM * m_electron / (mDM + m_electron);
    double effective_coupling = sqrt(sigmae/gev2cm2*pi*pow(pow(mAp,2)+pow(alpha_EM*m_electron,2),2))/reduced_mass;
    double gDM = effective_coupling/sqrt(4*pi*alpha_EM)/kappa;

    targetP.setValue("mDM", mDM);
    targetP.setValue("mAp", mAp);
    targetP.setValue("gDM", gDM);
    targetP.setValue("kappa", kappa);
    targetP.setValue("etaDM", myP["etaDM"]);
  }
#undef PARENT
#undef MODEL

#define MODEL SubGeVDM_fermion_sigmaN
#define PARENT SubGeVDM_fermion
  void MODEL_NAMESPACE::CAT_3(MODEL,_to_,PARENT) (const ModelParameters &myP, ModelParameters &targetP)
  {
    logger()<<"Running interpret_as_parent calculations for " STRINGIFY(MODEL) " --> " STRINGIFY(PARENT) "."<<LogTags::info<<EOM;

    USE_MODEL_PIPE(PARENT) // get pipe for "interprete as PARENT" function

    double mAp = myP["mAp"];
    double mDM = myP["mDM"];
    double kappa = myP["kappa"];
    double sigmaN = myP["sigmaN"];

    double reduced_mass = mDM * m_proton / (mDM + m_proton);
    double gN = sqrt(sigmaN*pi/gev2cm2)/reduced_mass;
    double gDM = gN*pow(mAp,2)/sqrt(4*pi*alpha_EM)/kappa;

    targetP.setValue("mDM", mDM);
    targetP.setValue("mAp", mAp);
    targetP.setValue("gDM", gDM);
    targetP.setValue("kappa", kappa);
    targetP.setValue("etaDM", myP["etaDM"]);
  }
#undef PARENT
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
