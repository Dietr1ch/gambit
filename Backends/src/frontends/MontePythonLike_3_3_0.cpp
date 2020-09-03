//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Frontend source for the MontePython backend.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Janina Renk
///          (janina.renk@fysik.su.se)
///  \date 2019 June, 2020 May
///
///  \author Sanjay Bloor
///          (sanjay.bloor12@imperial.ac.uk)
///  \date 2019 June
///
///  \author Pat Scott
///          (pat.scott@uq.edu.au)
///  \date 2020 Apr
///
///  *********************************************

#include "gambit/Backends/frontend_macros.hpp"
#include "gambit/Backends/frontends/MontePythonLike_3_3_0.hpp"

#ifdef HAVE_PYBIND11

  #include <pybind11/stl.h>
  #include <pybind11/stl_bind.h>
  #include <pybind11/functional.h>

  BE_NAMESPACE
  {
    using namespace pybind11::literals; // to bring in the `_a` literal to initialise python dictionaries with pybind11

    static map_str_dbl chached_likelihoods; // string double map to save likelihood calculation from previous param point

    /// calls the function "get_available_likelihoods" in the patched MontePythonLike.py file of MontePython backend.
    /// This returns a list containing strings with the names of all likelihoods available in the MontePythonLike backend
    std::vector<str> get_MP_available_likelihoods()
    {
      pybind11::list avail_likes = MontePythonLike.attr("get_available_likelihoods")(backendDir);
      return avail_likes.cast<std::vector<str>>();
    }

    /// convenience function to check compatibility of likelihoods and CLASS version in use
    /// add further rules for incompatibilities here.
    void check_likelihood_classy_combi(std::string& likelihood, std::string& classy_backendDir)
    {

      // add all combinations that are not compatible here.
      // example for such an incompatibility: an update in CLASS 2.7 enables the separation 
      // of the matter power spectrum into the baryonic and CDM components separated from 
      // other non-CDM species (e.g. massive neutrinos). Likelihoods that rely on the 
      // baryon+CDM matter power spectrum can't be used with a CLASS version below 2.7
      map_str_str incompatible_combi = {{"bao_correlations", "2.6.3"},{"euclid_pk", "2.6.3"}, 
          {"ska1_IM_band1", "2.6.3"},{"ska1_IM_band2", "2.6.3"},{"ska2_pk", "2.6.3"}};

      // check if incompatible CLASS version exists for given likelihood
      if (incompatible_combi.count(likelihood))
      {
        // if incompatible version in use throw a fatal error
        if (classy_backendDir.find(incompatible_combi[likelihood]) != std::string::npos)
        {
          std::string classy_version = classy_backendDir.substr(classy_backendDir.find("classy/"),classy_backendDir.find("/lib")); 
          classy_version = classy_version.substr(7, classy_version.size() - 11);

          std::ostringstream ss;
            ss << "You requested the MontePython likelihood '"<< likelihood<<"'." << endl
               << "The CLASS version "<< classy_version << " is not compatible with that likelihood"<< endl
               << "You need to install a later version of CLASS. To do so you can execute" << endl
               << "\ncd build; make nuke-classy_"<< classy_version <<"; make classy_<version_name>; cd ..\n" << endl
               << "where <version_name> is the version greater than "<< classy_version <<" of the CLASS backend." << endl
               << "Type \n\n./gambit backends\n\nto see which versions are available to install." << endl;
            backend_error().raise(LOCAL_INFO, ss.str());
        }
      }
    }

    /// convenience function to compute the loglike from a given experiment, given a MontePython likelihood-data container
    /// mplike, using the CLASS Python object cosmo.
    double get_MP_loglike(const MPLike_data_container& mplike, pybind11::object& cosmo, std::string& experiment)
    {

      double result;

      //static const bool first_run 
      // check if likelihood needs to be re-evaluated:
      // => if CLASS re-ran likelihood needs updating in any case
      bool needs_update = cosmo.attr("recomputed").cast<bool>();

      // => if CLASS did not re-compute: check if likelihood has nuisance parameters
      if(not needs_update)
      {
        try
        {
          // get list of nuisance parameter needed for likelihood calculation from MP
          // => if the list is not empty, there are nuisance parameters, hence we need to update MP likelihood
          //   (note: pybind11::bool_(list) = True if list contains entries, False if list is empty)
          pybind11::list nuisance_list  = mplike.likelihoods.at(experiment).attr("use_nuisance");
          needs_update = pybind11::bool_(nuisance_list);
        }
        catch(const std::exception&)
        {
          // if for some reason the MP likelihood object does not have the attribute "use_nuisance" (even though all of them
          //  should, but let's not count on it...) there are no nuisance parameters. So calculation can be skipped. 
          needs_update = false;
        }
      }

      // calculate likelihood if check above concluded that it needs updating
      // (CLASS rerun and/or likelihood has nuisance parameters)
      if(needs_update)
      {

        logger() << LogTags::info << "[MontePythonLike_" << STRINGIFY(VERSION) << "]:  Start evaluation for -> " << experiment << " <-" << EOM;
        // need to use likelihood.at() since it is a const map -> [] can create entry & can't be used on const object
        result = mplike.likelihoods.at(experiment).attr("loglkl")(cosmo, mplike.data).cast<double>();

        logger() << LogTags::info << "[MontePythonLike_" << STRINGIFY(VERSION) << "]:  Finished evaluation for -> " << experiment << " <-";
        logger() << " and got: " << result <<  EOM;

        // save likelihood result
        chached_likelihoods[experiment] = result;
      }
      // use cached likelihood value if the cosmology has not changed w.r.t. previously calculated point
      else
      {
        // get cached result
        result = chached_likelihoods[experiment];
        logger() << LogTags::info << "[MontePythonLike_" << STRINGIFY(VERSION) << "]:  Using cached LogLike value for -> " << experiment << " <-";
        logger() << " which is: " << result <<  EOM;
      }

      return result;
    }

    /// Creates a MontePython 'Data' object.
    /// This is initialised with a list of the relevant experimental limits to import.
    pybind11::object create_MP_data_object(map_str_str& experiments)
    {

      pybind11::dict path_dict = pybind11::dict("MontePython"_a=backendDir,
          "data"_a=backendDir+"/../data/",
          "cosmo"_a=backendDir+"",  // we never want to class CLASS from MP so there is no need to pass anything here
          "root"_a=backendDir+"/../../../");

      // Cast the list of experiments to a tuple, for MP to fire up...
      // note that experiments is a str to str map where the key is the likelihood name.
      // This is the only thing we need here when creating the data object
      // The value of the keys (data files to use) will be needed when initialising the likelihood objects
      // in the function 'create_MP_likelihood_objects'
      pybind11::list MP_experiments;
      for (auto const& it : experiments)
      {
        MP_experiments.attr("append")( it.first.c_str() );
      }

      // Import Data object from MontePython
      // (pass empty string as "command_line" since we do not need this information as sampling is taken care
      // of by GAMBIT)
      pybind11::object data = MontePythonLike.attr("Data")("", path_dict, MP_experiments);

      return data;
    }

    /// Returns a map of string to Python objects. These Python objects from MontePython are the initialised
    /// "Likelihood" objects used internal to MontePython. GAMBIT interfaces to these by requesting the
    /// loglike methods of these Python objects.
    /// Note that this is only executed once before the likelihoods for the first point in parameter space
    /// are calculated. -> time expensive loading up of all modules and data files is just done once.
    map_str_pyobj create_MP_likelihood_objects(pybind11::object& data, map_str_str& experiments)
    {
      // in stand-alone MP the command line contains some information for the sampler (how many points, restart..)
      // since the scanner module of GAMBIT is taking care of these things we do not need to pass anything here
      pybind11::str command_line = "";

      // Root likelihood path.
      std::string like_path = backendDir+"/likelihoods/";

      // Add the Likelihood path to sys so we can import it in Python
      pybind11::module sys = pybind11::module::import("sys");
      sys.attr("path").attr("insert")(0, like_path);

      map_str_pyobj likelihoods;

      // Now go through each experiment one by one, and initialise the Likelihood containers in
      // MontePython, then add them to a dictionary to pass back to CosmoBit.
      for (auto const& it : experiments)
      {

        std::string exp_name = it.first;
        std::string data_file = it.second;

        pybind11::str exp_path;
        // set path to .data file to the default one in MP if the "default" option is choosen
        // if not use the data file that has been chossen in the yaml file
        if(data_file == "default")  {exp_path = like_path + "/" + exp_name + "/" + exp_name + ".data";}
        else                        {exp_path = data_file;}

        pybind11::module  exp_module = pybind11::module::import(exp_name.c_str());
        pybind11::object  EXP_MODULE = exp_module.attr(exp_name.c_str())(exp_path, data, command_line);

        likelihoods[exp_name] = EXP_MODULE;

        // store likelihood objects in data object
        // this is needed to be able to get a list with
        // all nuisance parameters that need to be included
        // in the scan. This is done in the function 'create_MP_objects'
        // by calling data.check_nuisance_params()
        data.attr("add_experiment")(exp_name,EXP_MODULE);
      }
      

      // Remove "like_path" from sys.path (The likelihoods are now loaded)
      sys.attr("path").attr("remove")(like_path);

      return likelihoods;
    }
  }
  END_BE_NAMESPACE

#endif

BE_INI_FUNCTION
{}
END_BE_INI_FUNCTION
