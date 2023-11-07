//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Likelihood for isotropic, velocity-dependent 
///  cross-section based on evaporation of sub-cluster
///  in Bullet cluster collision. 
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Sowmiya Balan
///          (sowmiya.balan@kit.edu)
///  \date 2023 September
///
///
///  *********************************************

#include <iostream>
#include <fstream>

#include <gsl/gsl_math.h>
#include <gsl/gsl_spline.h>
#include <gsl/gsl_interp2d.h>
#include <gsl/gsl_spline2d.h>

#include "gambit/Elements/gambit_module_headers.hpp"
#include "gambit/Utils/util_functions.hpp"
#include "gambit/Utils/ascii_table_reader.hpp"
#include "gambit/DarkBit/DarkBit_rollcall.hpp"
#include "gambit/DarkBit/DarkBit_utils.hpp"


namespace Gambit
{
  namespace DarkBit
  {
    void self_int_xsection (double& result) 
    {
      result = 1; // in units of cm^2/g
    }

    class bullet_cluster_1dInterp_lnL 
    {
      private:
        // The gsl objects for the interpolating functions.
        gsl_interp_accel *acc;
        gsl_spline *spline;
        // Upper and lower boundaries available for the interpolating function.
        double lo;
        double up;

      public:
        // Constructor
        bullet_cluster_1dInterp_lnL(std::string file, std::string interp_type)
        {
          // Check if file exists.
          if (not(Utils::file_exists(file)))
          {
            DarkBit_error().raise(LOCAL_INFO, "ERROR! File '"+file+"' not found!");
          } else {
            logger() << LogTags::debug << "Reading data from file '"+file+"." << EOM;
          }
          // Read numerical values from data file.
          ASCIItableReader tab (file);
          tab.setcolnames("x", "y");
          const std::vector<double> x = tab["x"];
          const std::vector<double> y = tab["y"];
          int pts = x.size();
          // Get first and last value of the "x" component.
          lo = x.front();
          up = x.back();
          acc = gsl_interp_accel_alloc();
          
          // Allocate and initialise interp object.
          if (interp_type == "cspline")
          {
            spline = gsl_spline_alloc(gsl_interp_cspline, pts);
          }
          else if (interp_type == "linear")
          {
            spline = gsl_spline_alloc(gsl_interp_linear, pts);
          }
          else
          {
            DarkBit_error().raise(LOCAL_INFO, "ERROR! Interpolation type not known, choose between 'bilinear' or 'bicubic'.");
          }

          gsl_spline_init(spline, &x[0], &y[0], pts);
        }

        // Destructor
        ~bullet_cluster_1dInterp_lnL()
        {
          gsl_spline_free (spline);
          gsl_interp_accel_free (acc);
        }

        // Likelihood_evaluator
        double interp_eval(double s0m) {return gsl_spline_eval(spline, s0m, acc);}
        
    };

    class bullet_cluster_2dInterp_lnL 
    {
      private:
        // The gsl objects for the interpolating functions that need to be available to the class routines.
        gsl_interp_accel *x_acc;
        gsl_interp_accel *y_acc;
        gsl_spline2d *spline;
        double* z;
        // Upper and lower "x" and "y" values available to the interpolating function.
        double x_lo, y_lo, x_up, y_up;

      public:
        // Constructor
        bullet_cluster_2dInterp_lnL(std::string file, std::string interp_type)
        {
          // Check if file exists.
          if (not(Utils::file_exists(file)))
          {
            DarkBit_error().raise(LOCAL_INFO, "ERROR! File '"+file+"' not found!");
          } else {
            logger() << LogTags::debug << "Reading data from file '"+file+"." << EOM;
          }

          // Read numerical values from data file.
          ASCIItableReader tab (file);
          tab.setcolnames("x", "y", "z");
          // Initialise gsl interpolation routine.
          // Get unique entries of "x" and "y" for the grid and grid size.
          std::vector<double> x_vec = tab["x"];
          sort(x_vec.begin(), x_vec.end());
          x_vec.erase(unique(x_vec.begin(), x_vec.end()), x_vec.end());
          int nx = x_vec.size();
          std::vector<double> y_vec = tab["y"];
          sort(y_vec.begin(), y_vec.end());
          y_vec.erase(unique(y_vec.begin(), y_vec.end()), y_vec.end());
          int ny = y_vec.size();
          int n_grid_pts = tab["z"].size();

          if (nx*ny != n_grid_pts)
          {
            DarkBit_error().raise(LOCAL_INFO, "ERROR! The number of grid points ("+std::to_string(n_grid_pts)+") for 2D Likelihood interpolation \
             does not equal the number of unique 'x' and 'y' values ("+std::to_string(nx)+" and "+std::to_string(ny)+ \
             ")!\n Check formatting of the file: '"+file+"'.");
          }

          const double* x = &x_vec[0];
          const double* y = &y_vec[0];
          // Allocate memory for "z" values array in gsl format
          z = (double*) malloc(nx * ny * sizeof(double));

          if (interp_type == "bicubic")
          {
            spline = gsl_spline2d_alloc(gsl_interp2d_bicubic, nx, ny);
          }
          else if (interp_type == "bilinear")
          {
            spline = gsl_spline2d_alloc(gsl_interp2d_bilinear, nx, ny);
          }
          else
          {
            DarkBit_error().raise(LOCAL_INFO, "ERROR! Interpolation type not known, choose between 'bilinear' or 'bicubic'.");
          }

          x_acc = gsl_interp_accel_alloc();
          y_acc = gsl_interp_accel_alloc();

          // Determine first and last "x" and "y" values and grid step size.
          x_lo = x_vec.front();
          x_up = x_vec.back();
          y_lo = y_vec.front();
          y_up = y_vec.back();
          double x_delta = (x_up-x_lo) / (nx-1);
          double y_delta = (y_up-y_lo) / (ny-1);

          // Intialise grid.
          for (int i = 0; i < n_grid_pts; i++)
          {
            // Determine appropriate indices for the grid points.
            double temp = (tab["x"][i]-x_lo) / x_delta;
            int ind_x = (int) (temp+0.5);
            temp = (tab["y"][i]-y_lo) / y_delta;
            int ind_y = (int) (temp+0.5);

            //std::cout << ind_x << "/" << nx-1 << " " << tab["x"][i] << " vs " << x[ind_x] << " " << ind_y << "/" << ny-1 << " " << tab["y"][i] << " vs " << y[ind_y] << std::endl;

            gsl_spline2d_set(spline, z, ind_x, ind_y, tab["z"][i]);
          }
          gsl_spline2d_init (spline, x, y, z, nx, ny);
        }

        // Destructor
        ~bullet_cluster_2dInterp_lnL()
        {
          gsl_spline2d_free (spline);
          gsl_interp_accel_free (x_acc);
          gsl_interp_accel_free (y_acc);
          free(z);
        }

        // Likelihood_evaluator
        double interp_eval(double f, double s0m) {return gsl_spline2d_eval(spline, f, s0m, x_acc, y_acc); }
        
    };
        
    void calc_1d_lnL_BulletClusterEvap (double& result)
    {
      using namespace Pipes::calc_1d_lnL_BulletClusterEvap;
      // Write a module function that calculates the total self-interaction cross-section over mass (sigma_tot/m)
      // from DM_mass and coupling 
      double sigma_over_m = *Dep::SigmaOverM_SelfInteraction;

      static std::string interp_type = runOptions->getValueOrDef<std::string>("cspline","interpolation_method");
      static std::string method = runOptions->getValueOrDef<std::string> ("bayesian","stat_method");
      static std::string inital_mlr_prior = runOptions->getValueOrDef<std::string> ("main_cluster_mlr","initial_mlr_prior");

      static std::string lnL_file_path;

      if (method == "frequentist")
      {
        lnL_file_path = GAMBIT_DIR "/DarkBit/data/BulletCluster/sigmaTot_over_m_lnL_cchi2.dat";
      }
      else if (method == "bayesian" && inital_mlr_prior == "mlr_distribution")
      {
        lnL_file_path = GAMBIT_DIR "/DarkBit/data/BulletCluster/sigmaTot_over_m_lnL_optimistic.dat";
      }
      else if (method == "bayesian" && inital_mlr_prior == "main_cluster_mlr")
      {
        lnL_file_path = GAMBIT_DIR "/DarkBit/data/BulletCluster/sigmaTot_over_m_lnL_pessimistic.dat";
      }
      else
      {
        DarkBit_error().raise(LOCAL_INFO, "ERROR! Prior for initial MLR marginalisation can only be over the observed \
        I-band MLR distribution ('mlr_distribution') or over a Gaussian defined by observed main cluster MLR \
        ('main_cluster_mlr').");
      }

      static bullet_cluster_1dInterp_lnL bullet_cluster_self_int_1DlnL(lnL_file_path,interp_type);
      result = bullet_cluster_self_int_1DlnL.interp_eval(sigma_over_m);
    }

    void calc_2d_lnL_BulletClusterEvap (double& result)
    {
      using namespace Pipes::calc_2d_lnL_BulletClusterEvap;
      // Write a module function that calculates the total self-interaction cross-section over mass (sigma_tot/m)
      // from DM_mass and coupling 
      double sigma_over_m = *Dep::SigmaOverM_SelfInteraction;
      double dm_fraction = *Dep::RD_fraction;

      static std::string interp_type = runOptions->getValueOrDef<std::string>("bicubic","interpolation_method");
      static std::string method = runOptions->getValueOrDef<std::string>("bayesian","stat_method");
      static std::string inital_mlr_prior = runOptions->getValueOrDef<std::string>("main_cluster_mlr","initial_mlr_prior");

      static std::string lnL_file_path;

      if (method == "frequentist")
      {
        lnL_file_path = GAMBIT_DIR "/DarkBit/data/BulletCluster/sigmaTot_over_m_vs_f_lnL_cchi2.dat";
      }
      else if (method == "bayesian" && inital_mlr_prior == "mlr_distribution")
      {
        lnL_file_path = GAMBIT_DIR "/DarkBit/data/BulletCluster/sigmaTot_over_m_vs_f_lnL_optimistic.dat";
      }
      else if (method == "bayesian" && inital_mlr_prior == "main_cluster_mlr")
      {
        lnL_file_path = GAMBIT_DIR "/DarkBit/data/BulletCluster/sigmaTot_over_m_vs_f_lnL_pessimistic.dat";
      }
      else
      {
        DarkBit_error().raise(LOCAL_INFO, "ERROR! Prior for initial MLR marginalisation can only be over the observed \
        I-band MLR distribution ('mlr_distribution') or over a Gaussian defined by observed main cluster MLR \
        ('main_cluster_mlr').");
      }

      static bullet_cluster_2dInterp_lnL bullet_cluster_self_int_2DlnL(lnL_file_path,interp_type);
      result = bullet_cluster_self_int_2DlnL.interp_eval(dm_fraction,sigma_over_m);

    }
  }
}
