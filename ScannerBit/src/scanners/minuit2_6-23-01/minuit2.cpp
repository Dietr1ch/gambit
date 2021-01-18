//  GAMBIT: Global and Modular BSM Inference Tool
//  *********************************************
///  \file
///
///  ScannerBit interface to Minuit2
///
///  Header file
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Andrew Fowlie
///          (andrew.j.fowlie@qq.com)
///  \date 2020 August
///
///  *********************************************

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "gambit/ScannerBit/scanner_plugin.hpp"
#include "gambit/ScannerBit/scanners/minuit2/minuit2.hpp"
#include "gambit/Utils/yaml_options.hpp"
#include "gambit/Utils/util_functions.hpp"

#include "Minuit2/Minuit2Minimizer.h"
#include "Math/Functor.h"


/** @brief Check that a yaml node does not contain unexpected keys */
void check_node_keys(YAML::Node node, std::vector<std::string> keys)
{
  if (node)
  {
    for (const auto &s : node)
    {
      const auto key = s.first.as<std::string>();
      if (std::find(keys.begin(), keys.end(), key) == keys.end())
      {
        throw std::runtime_error("Minuit2: unexpected key = " + key);
      }
    }
  }
}

/** @brief Get a particular key from a node */
double get_node_value(YAML::Node node, std::string key, double default_)
{
  if (node && node[key])
  {
    return node[key].as<double>();
  }
  return default_;
}

scanner_plugin(minuit2, version(6, 23, 01))
{
  reqd_libraries("Minuit2", "Minuit2Math");
  reqd_headers("Minuit2/Minuit2Minimizer.h", "Math/Functor.h");

  int plugin_main(void)
  {
    // retrieve the dimensionality of the scan
    const int dim = get_dimension();

    // retrieve the model - contains loglike etc
    Gambit::Scanner::like_ptr model = get_purpose(get_inifile_value<std::string>("like"));
    const auto offset = get_inifile_value<double>("likelihood: lnlike_offset", 0.);
    model->setPurposeOffset(offset);
    const auto names = model.get_names();

    // minuit2 algorithm options
    const auto algorithm{get_inifile_value<std::string>("algorithm", "combined")};
    const auto max_loglike_calls{get_inifile_value<int>("max_loglike_calls", 100000)};
    const auto max_iterations{get_inifile_value<int>("max_iterations", 100000)};
    const auto tolerance{get_inifile_value<double>("tolerace", 0.0001)};
    const auto precision{get_inifile_value<double>("precision", 0.0001)};
    const auto print_level{get_inifile_value<int>("print_level", 1)};
    const auto strategy{get_inifile_value<int>("strategy", 2)};

    // get starting point (optional). It can be written in hypercube or physical
    // parameters. Default is center of hypercube for each parameter

    const auto hypercube_start_node = get_inifile_node("unit_hypercube_start");
    const auto physical_start_node = get_inifile_node("start");

    if (hypercube_start_node && physical_start_node)
    {
      throw std::runtime_error("Minuit2: start specified by unit hypercube or physical parameters");
    }

    check_node_keys(physical_start_node ? physical_start_node : hypercube_start_node, names);

    const double default_hypercube_start = 0.5;
    std::vector<double> hypercube_start(dim, default_hypercube_start);
    std::unordered_map<std::string, double> physical_start_map;

    if (physical_start_node)
    {
      physical_start_map = model.transform(hypercube_start);
      for (auto &s : physical_start_map)
      {
        s.second = get_node_value(physical_start_node, s.first, s.second);
      }
      hypercube_start = model.inverse_transform(physical_start_map);
    }
    else
    {
      for (int i = 0; i < dim; i++)
      {
        hypercube_start[i] = get_node_value(hypercube_start_node, names[i], hypercube_start[i]);
      }
      physical_start_map = model.transform(hypercube_start);
    }

    // get hypercube step (optional). It can be written in hypercube or physical
    // parameters. Default is same for each parameter

    const double default_hypercube_step = 0.01;
    const auto hypercube_step_node = get_inifile_node("unit_hypercube_step");
    const auto physical_step_node = get_inifile_node("step");

    if (hypercube_step_node && physical_step_node)
    {
      throw std::runtime_error("Minuit2: step specified by unit hypercube or physical parameters");
    }

    check_node_keys(physical_step_node ? physical_step_node : hypercube_step_node, names);

    std::vector<double> hypercube_step;

    if (physical_step_node)
    {
      const auto center = model.transform(hypercube_start);

      for (int i = 0; i < dim; i++)
      {
        if (!physical_step_node[names[i]])
        {
          hypercube_step.push_back(default_hypercube_step);
        }
        else
        {
          double physical_step = physical_step_node[names[i]].as<double>();
          auto forward = center;
          forward.at(names[i]) += physical_step;
          auto backward = center;
          backward.at(names[i]) -= physical_step;

          const auto hypercube_forward = model.inverse_transform(forward);
          const auto hypercube_backward = model.inverse_transform(backward);
          const double mean_step = 0.5 * (hypercube_forward[i] - hypercube_backward[i]);
          hypercube_step.push_back(mean_step);
        }
      }
    }
    else
    {
      for (const auto& n : names)
      {
         hypercube_step.push_back(get_node_value(hypercube_step_node, n, default_hypercube_step));
      }
    }

    // select algorithm

    ROOT::Minuit2::EMinimizerType kalgorithm;
    if (algorithm == "simplex")
    {
      kalgorithm = ROOT::Minuit2::kSimplex;
    }
    else if (algorithm == "combined")
    {
      kalgorithm = ROOT::Minuit2::kCombined;
    }
    else if (algorithm == "scan")
    {
      kalgorithm = ROOT::Minuit2::kScan;
    }
    else if (algorithm == "fumili")
    {
      kalgorithm = ROOT::Minuit2::kFumili;
    }
    else if (algorithm == "bfgs")
    {
      kalgorithm = ROOT::Minuit2::kMigradBFGS;
    }
    else if (algorithm == "migrad")
    {
      kalgorithm = ROOT::Minuit2::kMigrad;
    }
    else
    {
      throw std::runtime_error("Minuit2: Unknown algorithm: " + algorithm);
    }

    ROOT::Math::Minimizer* min = new ROOT::Minuit2::Minuit2Minimizer(kalgorithm);
    min->SetStrategy(strategy);
    min->SetMaxFunctionCalls(max_loglike_calls);
    min->SetMaxIterations(max_iterations);
    min->SetTolerance(tolerance);
    min->SetPrintLevel(print_level);
    min->SetPrecision(precision);

    auto chi_squared = [&model, dim] (const double* x)
    {
      std::vector<double> v;
      for (int i = 0; i < dim; i++)
      {
        v.push_back(x[i]);
      }
      return -2. * model(v);
    };

    ROOT::Math::Functor f(chi_squared, dim);
    min->SetFunction(f);

    // set the free variables to be minimized

    for (int i = 0; i < dim; i++)
    {
      const bool added = min->SetLimitedVariable(i, names[i], hypercube_start[i], hypercube_step[i], 0., 1.);
      if (added)
      {
        std::cout << names[i] << ". hypercube = " << hypercube_start[i]
                  << " +/- " << hypercube_step[i]
                  << ". physical = " << physical_start_map.at(names[i]) << std::endl;
      }
      else
      {
        throw std::runtime_error("could not add parameter");
      }
    }

    // do the minimization
    min->Minimize();

    std::cout << "minimum chi-squared = " << min->MinValue() << std::endl;

    const double *best_fit_hypercube = min->X();
    for (int i = 0; i < dim; i++)
    {
      std::cout << "best-fit hypercube " << i << " = "
                << best_fit_hypercube[i] << std::endl;
    }

    // convert result to physical parameters
    std::vector<double> v;
    for (int i = 0; i < dim; i++)
    {
      v.push_back(best_fit_hypercube[i]);
    }
    auto best_fit_physical = model.transform(v);
    std::cout << "best-fit physical = " << best_fit_physical << std::endl;

    // whether successful
    const int status = min->Status();
    switch (status) {
      case 0:
        break;
      case 1:
        throw std::runtime_error("Minuit2: Covar was made pos def");
      case 2:
        throw std::runtime_error("Minuit2: Hesse is not valid");
      case 3:
        throw std::runtime_error("Minuit2: Edm is above max");
      case 4:
        throw std::runtime_error("Minuit2: Reached call limit");
      case 5:
        throw std::runtime_error("Minuit2: Covar is not pos def");
      default:
        throw std::runtime_error("Minuit2: Unknown error: " + std::to_string(status));
     }

    // manually delete the pointer
    delete min;

    // success if reached end
    return 0;
  }
}
