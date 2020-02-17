//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Frontend source for the exoclassy backend.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Janina Renk
///          (janina.renk@fysik.su.se)
///  \date 2019 June
///
///  \author Sanjay Bloor
///          (sanjay.bloor12@imperial.ac.uk)
///  \date 2019 June
///
///  \author Patrick Stoecker
///          (stoecker@physik.rwth-aachen.de)
///  \date 2019 July
///
///  *********************************************

#include "gambit/Backends/frontend_macros.hpp"
#include "gambit/Backends/frontends/classy_exo_2_7_0.hpp"
#include "gambit/CosmoBit/CosmoBit_utils.hpp"

#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/functional.h>
#include <pybind11/eval.h>


BE_NAMESPACE
{

  pybind11::object static cosmo;
  // save input dictionary from CLASS run from 
  // previously computed point
  pybind11::dict static prev_input_dict;

  // return cosmo object. Need to pass this to MontePython for Likelihoods calculations
  pybind11::object get_classy_cosmo_object()
  {
    return cosmo;
  }

  /// test if two dictionaries contain exactly the same values for all keys
  /// return true if so, false if there is at least one different value
  bool compare_dicts(pybind11::dict classy_input, pybind11::dict prev_input_dict)
  {
    for (auto it : classy_input)
    {
      // test if any pointer is being passed to CLASS -- if so you'd need to compare the contents
      // of the vectors, so just recompute by default in that case (atm the options that pass pointser
      // can be identified by searching for 'array' or 'pointer_to' in the CLASS input dict. If more of 
      // these cases are implemented the checks have to be added here.)
      // (as soon there is a real fast-slow scanner implemented this sould probably be changed)
      if(it.first.cast<std::string>().find("array") != std::string::npos){return false;}
      if(it.first.cast<std::string>().find("pointer_to") != std::string::npos){return false;}
      
      // return false if unequal values are found
      if (classy_input[it.first].cast<std::string>() != prev_input_dict[it.first].cast<std::string>()){return false;}
    }
    // all key values are identical --> no need to run class again, yay!  
    return true;
  }

  /// routine to check class input for consistency. If a case is not treated here class will 
  ///  throw an error saying that one input parameter was not read. Checking for some specific cases
  /// here allows us to give more informative error messages and fix suggestions 
  void class_input_consistency_checks(pybind11::dict classy_input)
  {

    std::ostringstream errMssg;
    // one thing that can go wrong is that the primordial power spectrum is requested but 
    // not output of requiring the perturbations to be solved is asked for => 
    // check if "modes" input is set while "output" is not set
    if(classy_input.contains("modes") and not classy_input.contains("output"))
    {
      errMssg << "You are calling class asking for the following modes to be computed : "<< pybind11::repr(classy_input["modes"]);
      errMssg << "\nHowever, you did not request any output that requires solving the perturbations.\nHence CLASS";
      errMssg << " will not read the input 'modes' and won't run. Add the CLASS input parameter 'output' requesting";
      errMssg << " a spectrum to be computed to the yaml file as run option, e.g. \n  - capability: baseline_classy_input\n";
      errMssg << "    options:\n      classy_dict:\n        output: tCl";
      backend_error().raise(LOCAL_INFO,errMssg.str());
    }
  }

  // Routine to use backward compatible input names for energy injection inputs
  // In the future version of CLASS (v3.0 + ), the inputs will have
  // slightly modified names.
  // Our policy is to use the updated names in the code and to provide translations
  // for the versions prior to 3.0 (which will be outdated eventually)
  //
  // The relevant pairs (OLD -> NEW) are:
  //
  //  "tau_dcdm"                       --> "DM_decay_tau"
  //  "decay_fraction"                 --> "DM_decay_fraction"
  //  "annihilation"                   --> "DM_annihilation_efficiency"
  //  "annihilation_cross_section"     --> "DM_annihilation_cross_section"
  //  "DM_mass"                        --> "DM_annihilation_mass"
  //  "energy_deposition_function"     --> "f_eff_type"
  //  "energy_repartition_coefficient" --> "chi_type"
  //
  // NOTE: Our intention is to revert the inputs (i.e replace NEW with OLD)
  //
  void rename_energy_injection_parameters(pybind11::dict& cosmo_input_dict)
  {
    static bool first = true;
    static std::map<std::string,std::string> new_old_names;
    if (first)
    {
      first = false;
      new_old_names["DM_decay_tau"]                  = "tau_dcdm";
      new_old_names["DM_decay_fraction"]             = "decay_fraction";
      new_old_names["DM_annihilation_efficiency"]    = "annihilation";
      new_old_names["DM_annihilation_cross_section"] = "annihilation_cross_section";
      new_old_names["DM_annihilation_mass"]          = "DM_mass";
      new_old_names["f_eff_type"]                    = "energy_deposition_function";
      new_old_names["chi_type"]                      = "energy_repartition_coefficient";
      new_old_names["chi_file"]                      = "energy repartition coefficient file";
    }

    // In the first batch, we will fix the names of the inputs.
    // We will fix the entries itself, if needed, later.
    for (const auto& item: new_old_names)
    {
      pybind11::str newkey = pybind11::str(item.first);
      pybind11::str oldkey = pybind11::str(item.second);

      if (cosmo_input_dict.attr("has_key")(newkey).cast<bool>())
      {
        cosmo_input_dict[oldkey] = cosmo_input_dict.attr("pop")(newkey);
      }
    }

    // For the input "energy_repartition_coefficient" ("chi_type" in new versions),
    // also the acceptable values have changed.
    // If we can revert the input properly, we do so. If not, throw an error.
    //
    // NOTE: This only affects versions of exoclass <= 2.7.2.
    //
    if (cosmo_input_dict.attr("has_key")("energy_repartition_coefficient").cast<bool>())
    {
      std::string entry = (cosmo_input_dict["energy_repartition_coefficient"]).cast<std::string>();
      if (entry.compare("CK_2004") == 0)
      {
        cosmo_input_dict["energy_repartition_coefficient"] = "SSCK";
      }
      else if (entry.compare("Galli_2013_file") == 0 || entry.compare("Galli_2013_analytic") == 0)
      {
        cosmo_input_dict["energy_repartition_coefficient"] = "GSVI";
      }
      else if (entry.compare("from_x_file") == 0)
      {
        cosmo_input_dict["energy_repartition_coefficient"] = "from_file";
      }
      else
      {
        std::ostringstream errMssg;
        errMssg << "The chosen version of classy [classy_" << STRINGIFY(VERSION) << "] ";
        errMssg << "cannot handle the argument \'" << entry << "\' ";
        errMssg << "of the iput 'chi_type' (aka. 'energy_repartition_coefficient').\n";
        errMssg << "Please adjust your input or use a suitable version.";
        backend_error().raise(LOCAL_INFO,errMssg.str());
      }
    }
  }

  // getter functions to return a bunch of CLASS outputs. This is here in the frontend
  // to make the capabilities inside CosmoBit independent of types that depend on the 
  // Boltzmann solver in use

  // get the lensed Cl.
  std::vector<double> class_get_lensed_cl(std::string spectype)
  {
    // Get dictionary containing all (lensed) Cl spectra
    pybind11::dict cl_dict = cosmo.attr("lensed_cl")();

    // Get only the relevant Cl as np array and steal the pointer to its data.
    pybind11::object cl_array_obj = cl_dict[pybind11::cast<str>(spectype)];
    pybind11::array_t<double> cl_array = pybind11::cast<pybind11::array_t<double>>(cl_array_obj);
    auto cl_ptr = cl_array.data();
    size_t len = cl_array.size();

    // Create the vector to return
    std::vector<double> result(cl_ptr, (cl_ptr+len));

    // cl = 0 for l = 0,1
    result.at(0) = 0.;
    result.at(1) = 0.;

    return result;
  }

  // get the raw (unlensed) Cl.
  std::vector<double> class_get_unlensed_cl(std::string spectype)
  {
    // Get dictionary containing the raw (unlensed) Cl spectra
    pybind11::dict cl_dict = cosmo.attr("raw_cl")();

    // Get only the relevant Cl as np array and steal the pointer to its data.
    pybind11::object cl_array_obj = cl_dict[pybind11::cast<str>(spectype)];
    pybind11::array_t<double> cl_array = pybind11::cast<pybind11::array_t<double>>(cl_array_obj);
    auto cl_ptr = cl_array.data();
    size_t len = cl_array.size();

    // Create the vector to return
    std::vector<double> result(cl_ptr, (cl_ptr+len));

    // cl = 0 for l = 0,1
    result.at(0) = 0.;
    result.at(1) = 0.;

    return result;
  }

  // returns angular diameter distance for given redshift
  double class_get_Da(double z)
  {
    double Da = cosmo.attr("angular_distance")(z).cast<double>();
    // check if units are the same as from class??
    return Da;
  }

  // returns luminosity diameter distance for given redshift
  double class_get_Dl(double z)
  {
    double Dl = cosmo.attr("luminosity_distance")(z).cast<double>();
    return Dl;
  }

  // returns scale_independent_growth_factor for given redshift
  double class_get_scale_independent_growth_factor(double z)
  {
    double growth_fact = cosmo.attr("scale_independent_growth_factor")(z).cast<double>();
    return growth_fact;
  }

  // returns scale_independent_growth_factor for given redshift  TODO: what is different with and without f? think it 
  // is connected to powerspectra with baryons vs. baryons + cdm 
  double class_get_scale_independent_growth_factor_f(double z)
  {
    double growth_fact_f = cosmo.attr("scale_independent_growth_factor_f")(z).cast<double>();
    return growth_fact_f;
  }

  // returns Hubble parameter for given redshift
  double class_get_Hz(double z)
  {
    double H_z = cosmo.attr("Hubble")(z).cast<double>();
    return H_z;
  }

  // returns Omega radiation today
  double class_get_Omega0_r()
  {
    double Omega0_r = cosmo.attr("Omega_r")().cast<double>();
    return Omega0_r;
  }

  // returns Omega of ultra-relativistic species today
  double class_get_Omega0_ur()
  {
    double Omega0_ur = cosmo.attr("Omega_ur")().cast<double>();
    return Omega0_ur;
  }

  // returns Omega matter today
  double class_get_Omega0_m()
  {
    double Omega0_m = cosmo.attr("Omega_m")().cast<double>();
    return Omega0_m;
  }

  // returns Omega ncdm today (contains contributions of all ncdm species)
  double class_get_Omega0_ncdm_tot()
  {
    double Omega0_ncdm = cosmo.attr("Omega_ncdm_tot")().cast<double>();
    return Omega0_ncdm;
  }

/* you *could* also have this function in principle, however since the containts
  the contribution from ALL ncdm components it is not in general true, that Omega_ncdm = Omega_nu
  so I would not reccomend doing it. 
  // returns Omega nu today
  double class_get_Omega0_nu()
  {
    double Omega0_nu = cosmo.attr("Omega_nu")().cast<double>();
    return Omega0_nu;
  }
*/
  // returns Omega_Lambda
  double class_get_Omega0_Lambda()
  {
    double Omega0_Lambda = cosmo.attr("Omega_Lambda")().cast<double>();
    return Omega0_Lambda;
  }


  // returns sound horizon at drag
  double class_get_rs()
  {
    double rs_d = cosmo.attr("rs_drag")().cast<double>();
    return rs_d;
  }

  // returns sigma8 at z = 0
  double class_get_sigma8()
  {
    // in CosmoBit.cpp test if ClassInput contains mPk -> otherwise SegFault when trying to compute sigma8
    double sigma8 = cosmo.attr("sigma8")().cast<double>();
    return sigma8;
  }
  
  // returns Neff
  double class_get_Neff()
  {
    // in CosmoBit.cpp test if ClassInput contains mPk -> otherwise SegFault when trying to compute sigma8
    double Neff = cosmo.attr("Neff")().cast<double>();
    return Neff;
  }

  // print primordial power spectrum for consistency check & debug purposes
  void print_pps()
  {
    std::cout<< "Primordial spectrum from classy: "<< std::string(pybind11::str(cosmo.attr("get_primordial")())) << std::endl;
  }

}
END_BE_NAMESPACE

BE_INI_FUNCTION
{
  static int error_counter = 0;
  static int max_errors = 100;

  // get input for CLASS run set by CosmoBit 
  CosmoBit::Classy_input input_container= *Dep::classy_final_input;
  pybind11::dict cosmo_input_dict = input_container.get_input_dict();

  static bool first_run = true;
  
  if(first_run)
  {
    max_errors = runOptions->getValueOrDef<int>(100,"max_errors");
    cosmo = classy.attr("Class")();
    // check input for consistency
    class_input_consistency_checks(cosmo_input_dict);

    // create deep copy of cosmo_input_dict
    prev_input_dict = cosmo_input_dict.attr("copy")();
  }

  // test if input arguments for CLASS are exactly the same as in previous run ...
  bool equal = compare_dicts(prev_input_dict, cosmo_input_dict);

  // .. if so there is no need to recompute the results. If not, clean structure, re-fill input & re-compute
  if(not equal or first_run)
  {
    // Clean CLASS (the equivalent of the struct_free() in the `main` of CLASS -- don't want a memory leak, do we
    cosmo.attr("struct_cleanup")();

    // Actually only strictly necessary when cosmology is changed completely between two different runs
    // but just to make sure nothing's going wrong do it anyways..
    cosmo.attr("empty")();

    // set cosmological parameters
    logger() << LogTags::debug << "[classy_"<< STRINGIFY(VERSION) <<"] These are the inputs:\n\n";
    logger() << pybind11::repr(cosmo_input_dict) << EOM;
    cosmo.attr("set")(cosmo_input_dict);

    // Try to run class and catch potential errors
    logger() << LogTags::info << "[classy_"<< STRINGIFY(VERSION) <<"] Start to run \"cosmo.compute\"" << EOM;
    try
    {
      // Try to run classy
      cosmo.attr("compute")();
      // reset counter when no exception is thrown.
      error_counter = 0;
      logger() << LogTags::info << "[classy_"<< STRINGIFY(VERSION) <<"] \"cosmo.compute\" was successful" << EOM;
    }
    catch (std::exception &e)
    {
      std::ostringstream errMssg;
      errMssg << "Could not successfully execute cosmo.compute() in classy_"<< STRINGIFY(VERSION)<<"\n";
      std::string rawErrMessage(e.what());
      // If the error is a CosmoSevereError raise an backend_error ...
      if (rawErrMessage.find("CosmoSevereError") != std::string::npos)
      {
        errMssg << "Caught a \'CosmoSevereError\':\n\n";
        errMssg << rawErrMessage;
        backend_error().raise(LOCAL_INFO,errMssg.str());
      }
      // .. but if it is 'only' a CosmoComputationError, invalidate the parameter point
      // and raise a backend_warning.
      // In case this happens "max_errors" times in a row, raise a backend_error
      // instead, since it probably points to some issue with the inputs
      else if (rawErrMessage.find("CosmoComputationError") != std::string::npos)
      {
        ++error_counter;
        errMssg << "Caught a \'CosmoComputationError\':\n\n";
        errMssg << rawErrMessage;
        if ( max_errors < 0 || error_counter <= max_errors )
        {
          backend_warning().raise(LOCAL_INFO,errMssg.str());
          invalid_point().raise(errMssg.str());
        }
        else
        {
          errMssg << "\nThis happens now for the " << error_counter << "-th time ";
          errMssg << "in a row. There is probably something wrong with your inputs.";
          backend_error().raise(LOCAL_INFO,errMssg.str());
        }
      }
      // any other error (which shouldn't occur) gets also caught as invalid point.
      else
      {
        errMssg << "Caught an unspecified error:\n\n";
        errMssg << rawErrMessage;
        cout << "An unspecified error occurred during compute() in classy_"<< STRINGIFY(VERSION) <<":\n";
        cout << rawErrMessage;
        cout << "\n(This point gets invalidated) " << endl;
        invalid_point().raise(errMssg.str());
      }
    }
    //std::cout << "Trying to print power spectrum..." << std::endl;
    //print_pps();

  }
  // identical CLASS input -- skip compute step & save time! 
  else
  {
    logger() << LogTags::info << "[classy_"<< STRINGIFY(VERSION) <<"] \"cosmo.compute\" was skipped, input was identical to previously computed point" << EOM;
  }

  first_run = false;
  // save input arguments from this run to dictionary prev_input_dict
  // (clear entries before copying, hope there are non memory leaks?)
  prev_input_dict.attr("clear")();
  prev_input_dict = cosmo_input_dict.attr("copy")();

}
END_BE_INI_FUNCTION