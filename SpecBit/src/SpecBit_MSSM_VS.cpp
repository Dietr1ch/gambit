//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Functions of module SpecBit
///  for interfacing with VevaciousPlusPlus
///  and calculating likelihoods from VS in the MSSM
///
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author José Eliel Camargo-Molina
///           (elielcamargomolina@gmail.com)
///
///  \date Jun 2018
///
///  *********************************************

#ifdef WITH_MPI
#include "mpi.h"
#endif

#include <string>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>

#include <gsl/gsl_math.h>
#include <gsl/gsl_min.h>
#include "SLHAea/slhaea.h"

#include "gambit/Elements/gambit_module_headers.hpp"
#include "gambit/Elements/spectrum.hpp"
#include "gambit/Utils/stream_overloads.hpp"
#include "gambit/Utils/util_macros.hpp"
#include "gambit/SpecBit/SpecBit_rollcall.hpp"
#include "gambit/SpecBit/SpecBit_helpers.hpp"
#include "gambit/SpecBit/QedQcdWrapper.hpp"
#include "gambit/SpecBit/model_files_and_boxes.hpp" // #includes lots of flexiblesusy headers and defines interface classes
#include "gambit/Utils/stream_overloads.hpp"

// Switch for debug mode
//#define SPECBIT_DEBUG

namespace Gambit
{
  
  namespace SpecBit
  {
      using namespace LogTags;

		// This function initializes a VevaciousPlusPlus object that then can be fed  
		// a parameter point. 

// 	
// 	void Initialize_VevaciousPlusPlus(std::string const& inputFilename)
// 	{
// 		// Here we initialize Vevacious
// 
// 		
//  		return vevaciousPlusPlus;
//  	}

    bool firstrun = true;

    void make_vevaciousPlusPlus_inputs(std::string &inputspath)
        {
            namespace myPipe = Pipes::make_vevaciousPlusPlus_inputs;
            const Options& runOptions=*myPipe::runOptions;

            static bool firstrun = true;
            int rank;
            // Here we make sure files are only written the first time this is run
            if(firstrun) {

                std::string vevaciouslibpath = Backends::backendInfo().path_dir("vevacious","1.0");

                std::string vevaciouspath = vevaciouslibpath + "/../";


                // Get mpi rank

                #ifdef WITH_MPI
                MPI_Comm_rank(MPI_COMM_WORLD, &rank);
                #else
                rank = 0;
                #endif

                //Creating string with rank number
                std::string rankstring = std::to_string(rank);


                // Getting the run folder for saving initialization files
                inputspath = runOptions.getValue<std::string>("where_to_save_input");

                std::string initfilesPath = inputspath + "/InitializationFiles/mpirank_"+ rankstring + "/";
                std::string modelfilesPath = inputspath + "/ModelFiles/mpirank_"+ rankstring + "/";


                //Creating folders for initialization files

               try
               {
                  Utils::ensure_path_exists(initfilesPath);
                  Utils::ensure_path_exists(modelfilesPath);
               }
               catch(const std::exception& e)
               {
                    std::ostringstream errmsg;
                    errmsg << "Error creating vevacious initialization and model files folders for MPI process " << rankstring;
                    SpecBit_error().forced_throw(LOCAL_INFO,errmsg.str());
               }

               // Copying Vevacious Model files so that there is one for each process.

                std::string ScaleAndBlockFileSource = vevaciouspath +
                                                "ModelFiles/LagrangianParameters/MssmCompatibleWithSlhaOneAndSlhaTwo.xml";

                std::string ModelFileSource = vevaciouspath + "ModelFiles/PotentialFunctions/RealMssmWithStauAndStopVevs.vin";

                std::string ScaleAndBlockFile= modelfilesPath + "ScaleAndBlockFile.xml";

                std::string ModelFile = modelfilesPath + "ModelFile.vin";

                try
                {

                    std::ifstream  ScaleAndBlocksrc(ScaleAndBlockFileSource , std::ios::binary);
                    std::ofstream  ScaleAndBlockdst(ScaleAndBlockFile ,   std::ios::binary);

                    ScaleAndBlockdst << ScaleAndBlocksrc.rdbuf();

                    std::ifstream  ModelFilesrc(ModelFileSource , std::ios::binary);
                    std::ofstream  ModelFiledst(ModelFile ,   std::ios::binary);

                    ModelFiledst << ModelFilesrc.rdbuf();
                }
                catch(const std::exception& e)
                {
                    std::ostringstream errmsg;
                    errmsg << "Error copying model and scale/block vevacious files. Check they exist." << rankstring;
                    SpecBit_error().forced_throw(LOCAL_INFO,errmsg.str());
                }

                // Getting Minuit strategy option


                std::string MinuitStrategy = runOptions.getValueOrDef<std::string>(
                        "0", "minuit_strategy");
                // Writing potential function initialization file


                // Options that can be read from YAML file
                std::string potentialfunctioninitpath = initfilesPath + "/PotentialFunctionInitialization.xml";

                std::string PotentialFunctionClassType = runOptions.getValueOrDef<std::string>(
                "FixedScaleOneLoopPotential", "potential_type");
                // std::string PotentialFunctionClassType = "FixedScaleOneLoopPotential";



                // File contents
                std::string potentialfunctioninit =
                    "<VevaciousPlusPlusPotentialFunctionInitialization>\n"
                    " <LagrangianParameterManagerClass>\n"
                    "    <ClassType>\n"
                    "      SlhaCompatibleWithSarahManager\n"
                    "    </ClassType>\n"
                    "    <ConstructorArguments>\n"
                    "      <ScaleAndBlockFile>\n"
                    "      " + ScaleAndBlockFile + "\n"
                    "      </ScaleAndBlockFile>\n"
                    "    </ConstructorArguments>\n"
                    "  </LagrangianParameterManagerClass>\n"
                    "  <PotentialFunctionClass>\n"
                    "    <ClassType>\n"
                    "      " + PotentialFunctionClassType + "\n"
                    "    </ClassType>\n"
                    "    <ConstructorArguments>\n"
                    "      <ModelFile>\n"
                    "         " + ModelFile +
                    "\n"
                    "      </ModelFile>\n"
                    "      <AssumedPositiveOrNegativeTolerance>\n"
                    "        0.5\n"
                    "      </AssumedPositiveOrNegativeTolerance>\n"
                    "    </ConstructorArguments>\n"
                    "  </PotentialFunctionClass>\n"
                    "</VevaciousPlusPlusPotentialFunctionInitialization>";
                std::ofstream potentialfunctioninitwrite(potentialfunctioninitpath);
                potentialfunctioninitwrite << potentialfunctioninit;
                potentialfunctioninitwrite.close();
                // Check whether user wants hom4ps2 or PHC as homotopy continuation backend



                // Writing potential minimizer initialization file

                // Options that can be read from YAML file go here
                std::string homotopybackend = runOptions.getValueOrDef<std::string>("hom4ps",
                                                                                      "homotopy_backend");
                std::string globalIsPanic = runOptions.getValueOrDef<std::string>("false",
                                                                                    "global_minimum_is_panic");
                std::string potentialminimizerinitpath = initfilesPath +"/PotentialMinimizerInitialization.xml";
                std::string potentialminimizerinit;

                if(homotopybackend == "hom4ps") {

                    // Getting Path to hom4ps2

                    std::string PathToHom4ps2 = Backends::backendInfo().path_dir("hom4ps", "2.0");

                    // File contents
                    potentialminimizerinit =
                            "<VevaciousPlusPlusPotentialMinimizerInitialization>\n"
                            "  <PotentialMinimizerClass>\n"
                            "    <ClassType>\n"
                            "      GradientFromStartingPoints\n"
                            "    </ClassType>\n"
                            "    <ConstructorArguments>\n"
                            "      <StartingPointFinderClass>\n"
                            "        <ClassType>\n"
                            "          PolynomialAtFixedScalesSolver\n"
                            "        </ClassType>\n"
                            "        <ConstructorArguments>\n"
                            "          <NumberOfScales>\n"
                            "            1\n"
                            "          </NumberOfScales>\n"
                            "          <ReturnOnlyPolynomialMinima>\n"
                            "            No\n"
                            "          </ReturnOnlyPolynomialMinima>\n"
                            "          <PolynomialSystemSolver>\n"
                            "            <ClassType>\n"
                            "              Hom4ps2Runner\n"
                            "            </ClassType>\n"
                            "            <ConstructorArguments>\n"
                            "              <PathToHom4ps2>\n"
                            "        " + PathToHom4ps2 + "\n"
                             "              </PathToHom4ps2>\n"
                             "              <Hom4ps2Argument>\n"
                             "                1\n"
                             "              </Hom4ps2Argument>\n"
                             "              <ResolutionSize>\n"
                             "                1.0\n"
                             "              </ResolutionSize>\n"
                             "            </ConstructorArguments>\n"
                             "          </PolynomialSystemSolver>\n"
                             "        </ConstructorArguments>\n"
                             "      </StartingPointFinderClass>\n"
                             "      <GradientMinimizerClass>\n"
                             "        <ClassType>\n"
                             "          MinuitPotentialMinimizer\n"
                             "        </ClassType>\n"
                             "        <ConstructorArguments>\n"
                             "          <InitialStepSizeFraction>\n"
                             "            0.1\n"
                             "          </InitialStepSizeFraction>\n"
                             "          <MinimumInitialStepSize>\n"
                             "            1.0\n"
                             "          </MinimumInitialStepSize>\n"
                             "          <MinuitStrategy>\n"
                             "             "+ MinuitStrategy +"\n"
                             "          </MinuitStrategy>\n"
                             "        </ConstructorArguments>\n"
                             "      </GradientMinimizerClass>\n"
                             "      <ExtremumSeparationThresholdFraction>\n"
                             "        0.05\n"
                             "      </ExtremumSeparationThresholdFraction>\n"
                             "      <NonDsbRollingToDsbScalingFactor>\n"
                             "        4.0\n"
                             "      </NonDsbRollingToDsbScalingFactor>\n"
                             "      <GlobalIsPanic>\n"
                             "        " + globalIsPanic + "\n"
                             "      </GlobalIsPanic>\n"
                             "    </ConstructorArguments>\n"
                             "  </PotentialMinimizerClass>\n"
                             "</VevaciousPlusPlusObjectInitialization>\n";
                } else if(homotopybackend == "phc") {

                    // Getting path to PHC
                    std::string PathToPHC = Backends::backendInfo().path_dir("phc", "2.4.58");
                    // Creating symlink to PHC in run folder
                    std::string PHCSymlink = inputspath + "/Homotopy/mpirank_"+ rankstring + "/";

                    try
                    {
                        Utils::ensure_path_exists(PHCSymlink);
                    }
                    catch(const std::exception& e)
                    {
                        std::ostringstream errmsg;
                        errmsg << "Error creating PHC folder for MPI process " << rankstring;
                        SpecBit_error().forced_throw(LOCAL_INFO,errmsg.str());
                    }

                    std::string systemCommand( "ln -s " + PathToPHC + "/phc" + " " + PHCSymlink );

                    int systemReturn = system( systemCommand.c_str() ) ;
                    if( systemReturn == -1 )
                    {
                        std::ostringstream errmsg;
                        errmsg << "Error making symlink for PHC in process " << rankstring;
                        SpecBit_error().forced_throw(LOCAL_INFO,errmsg.str());
                    }

                    // File contents
                    potentialminimizerinit =
                            "<VevaciousPlusPlusPotentialMinimizerInitialization>\n"
                            "  <PotentialMinimizerClass>\n"
                            "    <ClassType>\n"
                            "      GradientFromStartingPoints\n"
                            "    </ClassType>\n"
                            "    <ConstructorArguments>\n"
                            "      <StartingPointFinderClass>\n"
                            "        <ClassType>\n"
                            "          PolynomialAtFixedScalesSolver\n"
                            "        </ClassType>\n"
                            "        <ConstructorArguments>\n"
                            "          <NumberOfScales>\n"
                            "            1\n"
                            "          </NumberOfScales>\n"
                            "          <ReturnOnlyPolynomialMinima>\n"
                            "            No\n"
                            "          </ReturnOnlyPolynomialMinima>\n"
                            "          <PolynomialSystemSolver>\n"
                            "            <ClassType>\n"
                            "              PHCRunner\n"
                            "            </ClassType>\n"
                            "            <ConstructorArguments>\n"
                            "              <PathToPHC>\n"
                            "        " + PHCSymlink + "\n"
                             "              </PathToPHC>\n"
                             "              <ResolutionSize>\n"
                             "                1.0\n"
                             "              </ResolutionSize>\n"
                             "            <Tasks>\n "
                             "             1                     "
                             "            </Tasks>\n            "
                             "            </ConstructorArguments>\n"
                             "          </PolynomialSystemSolver>\n"
                             "        </ConstructorArguments>\n"
                             "      </StartingPointFinderClass>\n"
                             "      <GradientMinimizerClass>\n"
                             "        <ClassType>\n"
                             "          MinuitPotentialMinimizer\n"
                             "        </ClassType>\n"
                             "        <ConstructorArguments>\n"
                             "          <InitialStepSizeFraction>\n"
                             "            0\n"
                             "          </InitialStepSizeFraction>\n"
                             "          <MinimumInitialStepSize>\n"
                             "            0.5\n"
                             "          </MinimumInitialStepSize>\n"
                             "          <MinuitStrategy>\n"
                             "            "+ MinuitStrategy +"\n"
                             "          </MinuitStrategy>\n"
                             "        </ConstructorArguments>\n"
                             "      </GradientMinimizerClass>\n"
                             "      <ExtremumSeparationThresholdFraction>\n"
                             "        0.1\n"
                             "      </ExtremumSeparationThresholdFraction>\n"
                             "      <NonDsbRollingToDsbScalingFactor>\n"
                             "        4.0\n"
                             "      </NonDsbRollingToDsbScalingFactor>\n"
                             "    </ConstructorArguments>\n"
                             "  </PotentialMinimizerClass>\n"
                             "</VevaciousPlusPlusObjectInitialization>\n";
                } else
                {
                    std::ostringstream errmsg;
                    errmsg << "The homotopy_backend option in the YAML file has not been set up properly. Check the input YAML file." << std::endl;
                    SpecBit_error().raise(LOCAL_INFO,errmsg.str());
                }

                std::ofstream potentialminimizerinitwrite(potentialminimizerinitpath);
                potentialminimizerinitwrite << potentialminimizerinit;
                potentialminimizerinitwrite.close();

                // Writing tunneling calculator initialization file
                std::string tunnelingcalculatorinitpath = initfilesPath +"/TunnelingCalculatorInitialization.xml";

                // Options that can be read from YAML file

                std::string TunnelingStrategy = runOptions.getValueOrDef<std::string>("JustQuantum",
                "tunneling_strategy");

                std::string pathFinidingTimeout = runOptions.getValueOrDef<std::string>("3600",
                                                                                        "path_finding_timeout");

                std::string SurvivalProbabilityThreshold = "0.01";


                // File contents
                std::string tunnelingcalculatorinit =
                    "<VevaciousPlusPlusObjectInitialization>\n"
                    "  <TunnelingClass>\n"
                    "    <ClassType>\n"
                    "      BounceAlongPathWithThreshold\n"
                    "    </ClassType>\n"
                    "    <ConstructorArguments>\n"
                    "      <TunnelingStrategy>\n"
                    "    " + TunnelingStrategy + "\n"
                    "      </TunnelingStrategy>\n"
                    "      <SurvivalProbabilityThreshold>\n"
                    "        " + SurvivalProbabilityThreshold + "\n"
                    "      </SurvivalProbabilityThreshold>\n"
                    "      <ThermalActionResolution>\n"
                    "        5\n"
                    "      </ThermalActionResolution>\n"
                    "      <CriticalTemperatureAccuracy>\n"
                    "        4\n"
                    "      </CriticalTemperatureAccuracy>\n"
                    "      <PathResolution>\n"
                    "        1000\n"
                    "      </PathResolution>\n"
                    "      <Timeout>\n"
                    "        "+ pathFinidingTimeout +"\n"
                    "      </Timeout>\n"
                    "      <MinimumVacuumSeparationFraction>\n"
                    "        0.2\n"
                    "      </MinimumVacuumSeparationFraction>\n"
                    "      <BouncePotentialFit>\n"
                    "        <ClassType>\n"
                    "          BubbleShootingOnPathInFieldSpace\n"
                    "        </ClassType>\n"
                    "        <ConstructorArguments>\n"
                    "          <NumberShootAttemptsAllowed>\n"
                    "            32\n"
                    "          </NumberShootAttemptsAllowed>\n"
                    "          <RadialResolution>\n"
                    "            0.05\n"
                    "          </RadialResolution>\n"
                    "        </ConstructorArguments>\n"
                    "      </BouncePotentialFit>\n"
                    "      <TunnelPathFinders>\n"
                    "        <PathFinder>\n"
                    "          <ClassType>\n"
                    "            MinuitOnPotentialOnParallelPlanes\n"
                    "          </ClassType>\n"
                    "          <ConstructorArguments>\n"
                    "            <NumberOfPathSegments>\n"
                    "              50 \n"
                    "            </NumberOfPathSegments>\n"
                    "            <MinuitStrategy>\n"
                    "             "+ MinuitStrategy +"\n"
                    "            </MinuitStrategy>\n"
                    "            <MinuitTolerance>\n"
                    "              1\n"
                    "            </MinuitTolerance>\n"
                    "          </ConstructorArguments>\n"
                    "        </PathFinder>\n"
                    "        <PathFinder>\n"
                    "          <ClassType>\n"
                    "            MinuitOnPotentialPerpendicularToPath\n"
                    "          </ClassType>\n"
                    "          <ConstructorArguments>\n"
                    "            <NumberOfPathSegments>\n"
                    "              100\n"
                    "            </NumberOfPathSegments>\n"
                    "            <NumberOfAllowedWorsenings>\n"
                    "              3\n"
                    "            </NumberOfAllowedWorsenings>\n"
                    "            <ConvergenceThresholdFraction>\n"
                    "              0.5\n"
                    "            </ConvergenceThresholdFraction>\n"
                    "            <MinuitDampingFraction>\n"
                    "              0.75\n"
                    "            </MinuitDampingFraction>\n"
                    "            <NeighborDisplacementWeights>\n"
                    "              0.5\n"
                    "              0.25\n"
                    "            </NeighborDisplacementWeights>\n"
                    "            <MinuitStrategy>\n"
                    "               "+ MinuitStrategy +"\n"
                    "            </MinuitStrategy>\n"
                    "            <MinuitTolerance>\n"
                    "              1\n"
                    "            </MinuitTolerance>\n"
                    "          </ConstructorArguments>\n"
                    "        </PathFinder>\n"
                    "      </TunnelPathFinders>\n"
                    "    </ConstructorArguments>\n"
                    "  </TunnelingClass>\n"
                    "</VevaciousPlusPlusObjectInitialization>";

                std::ofstream tunnelingcalculatorinitwrite(tunnelingcalculatorinitpath);
                tunnelingcalculatorinitwrite << tunnelingcalculatorinit;
                tunnelingcalculatorinitwrite.close();

                //Finally write the main input file for VevaciousPlusPlus

                std::string inputFilename =
                        inputspath + "/InitializationFiles/VevaciousPlusPlusObjectInitialization_mpirank_"+ rankstring +".xml";

                // File contents
                std::string inputfile =
                    "<VevaciousPlusPlusObjectInitialization>\n"
                    "  <PotentialFunctionInitializationFile>\n"
                    "   " + potentialfunctioninitpath + "\n"
                    "  </PotentialFunctionInitializationFile>\n"
                    "  <PotentialMinimizerInitializationFile>\n"
                    "   " + potentialminimizerinitpath + "\n"
                    "  </PotentialMinimizerInitializationFile>\n"
                    "  <TunnelingCalculatorInitializationFile>\n"
                    "   " +
                    tunnelingcalculatorinitpath + "\n"
                    "  </TunnelingCalculatorInitializationFile>\n"
                    "</VevaciousPlusPlusObjectInitialization>";
                std::ofstream inputwrite(inputFilename);
                inputwrite << inputfile;
                inputwrite.close();
                firstrun = false;
                }

        }

        // This function gives back the result for absolute stability, either "Stable" or "Metastable".
 	void check_stability_MSSM(ddpair &lifetimeAndThermalProbability)
    {
        // Vevacious backend path
       // std::string vevaciouspath = std::string(GAMBIT_DIR) +
       //                            "/Backends/installed/VevaciousPlusPlus/"+
       //                           Backends::backendInfo().working_versions("VevaciousPlusPlus").back()+
       //                            "/VevaciousPlusPlus/";
        namespace myPipe = Pipes::check_stability_MSSM;

        static std::string inputspath =  *myPipe::Dep::make_vevaciousPlusPlus_inputs;

        // Writing Vevacious input files


        // if(firstrun){
        //    make_vevaciousPlusPlus_inputs(&vevaciouspath);
        //    firstrun = false;
        //}
        // Initilization of Vevacious Object

        // Getting mpi rank

        int rank;
        #ifdef WITH_MPI
                    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        #else
                    rank = 0;
        #endif

        std::string rankstring = std::to_string(rank);

        std::string inputFilename = inputspath + "/InitializationFiles/VevaciousPlusPlusObjectInitialization_mpirank_"+ rankstring +".xml";
        vevacious_1_0::VevaciousPlusPlus::VevaciousPlusPlus vevaciousPlusPlus( inputFilename );

        // Get the spectrum object for MSSM

        const Spectrum& fullspectrum = *myPipe::Dep::unimproved_MSSM_spectrum;
        //double runscale = 1000;
        const SubSpectrum& spectrumHE = fullspectrum.get_HE();
        //std::unique_ptr<SubSpectrum> SpecRun = spectrumHE.clone();
        //SpecRun->RunToScale(runscale);
        // Here we get the SLHAea::Coll object from the spectrum
        SLHAea::Coll slhaea = spectrumHE.getSLHAea(2);

        // Here we get the scale from the high-energy spectrum for Vevacious.
		double scale = spectrumHE.GetScale();
        cout << "VEVACIOUS SCALE:  "<< scale << endl;

        // Here we start passing the parameters form the SLHAea::Coll  object

		std::vector<std::pair<int,double>> gaugecouplings = 
		{ { 1 , SLHAea::to<double>(slhaea.at("GAUGE").at(1).at(1))  }, { 2, SLHAea::to<double>(slhaea.at("GAUGE").at(2).at(1)) }, { 3, SLHAea::to<double>(slhaea.at("GAUGE").at(3).at(1)) } };
		
		vevaciousPlusPlus.ReadLhaBlock( "GAUGE", scale , gaugecouplings, 1 );

		std::vector<std::pair<int,double>> Hmix = { { 1 , SLHAea::to<double>(slhaea.at("HMIX").at(1).at(1))},
													{ 101, SLHAea::to<double>(slhaea.at("HMIX").at(101).at(1))},
													{ 102, SLHAea::to<double>(slhaea.at("HMIX").at(102).at(1))},
													{ 103, SLHAea::to<double>(slhaea.at("HMIX").at(103).at(1))},
													{ 3, SLHAea::to<double>(slhaea.at("HMIX").at(3).at(1))}
													};

		vevaciousPlusPlus.ReadLhaBlock( "HMIX", scale , Hmix, 1 );
	
   
		std::vector<std::pair<int,double>> minpar = {  
													{ 3, SLHAea::to<double>(slhaea.at("MINPAR").at(3).at(1))}
													};
												
		vevaciousPlusPlus.ReadLhaBlock( "MINPAR", scale , minpar, 1 );
	
		std::vector<std::pair<int,double>> msoft = { { 21 , SLHAea::to<double>(slhaea.at("MSOFT").at(21).at(1))},
													{ 22  , SLHAea::to<double>(slhaea.at("MSOFT").at(22).at(1))},
													{ 1  ,  SLHAea::to<double>(slhaea.at("MSOFT").at(1).at(1))},
													{ 2  ,  SLHAea::to<double>(slhaea.at("MSOFT").at(2).at(1))},
													{ 3   , SLHAea::to<double>(slhaea.at("MSOFT").at(3).at(1))}
													};
													
		vevaciousPlusPlus.ReadLhaBlock( "MSOFT", scale , msoft, 1 );
        // Here we check if the block "TREEMSOFT" is present
        try {

            std::vector<std::pair<int, double>> treemsoft = {{21, SLHAea::to<double>(slhaea.at("TREEMSOFT").at(21).at(1))},
                                                             {22, SLHAea::to<double>(slhaea.at("TREEMSOFT").at(22).at(1))} };

            vevaciousPlusPlus.ReadLhaBlock("TREEMSOFT", scale, treemsoft, 1);
        }
        catch (const std::exception& e)
        {
            cout << "No TREEMSOFT, skipping" << endl;
        }
        // Here we check if the block "LOOPMSOFT" is present

        try {
            std::vector<std::pair<int, double>> loopmsoft = {{21, SLHAea::to<double>(slhaea.at("LOOPMSOFT").at(21).at(1))},
                                                             {22, SLHAea::to<double>(slhaea.at("LOOPMSOFT").at(22).at(1))}};

            vevaciousPlusPlus.ReadLhaBlock("LOOPMSOFT", scale, loopmsoft, 1);
        }
        catch (const std::exception& e)
        {
            cout << "No LOOPMSOFT, skipping" << endl;
        }

        bool diagonalYukawas = false;
        // Here we check if the Yukawas are diagonal or not, i.e if the "YX" blocks have off-diagonal entries.
        try {
            SLHAea::to<double>(slhaea.at("YU").at(1,2));
        }
        catch (const std::exception& e)
        {
            cout << "Diagonal Yukawas detected"<< endl;
            diagonalYukawas = true;
        }

        // If diagonal pass the diagonal values to Vevacious
        if (diagonalYukawas) {
            std::vector<std::pair<int,double>> Yu = { { 11 , SLHAea::to<double>(slhaea.at("YU").at(1,1).at(2))},
                                                      { 12, 0},
                                                      { 13, 0},
                                                      { 21, 0},
                                                      { 22, SLHAea::to<double>(slhaea.at("YU").at(2,2).at(2))},
                                                      { 23, 0},
                                                      { 31, 0},
                                                      { 32, 0},
                                                      { 33, SLHAea::to<double>(slhaea.at("YU").at(3,3).at(2))}
            };

            vevaciousPlusPlus.ReadLhaBlock( "YU", scale , Yu, 2 );

            std::vector<std::pair<int,double>> Yd = { { 11 , SLHAea::to<double>(slhaea.at("YD").at(1,1).at(2))},
                                                      { 12, 0},
                                                      { 13, 0},
                                                      { 21, 0},
                                                      { 22, SLHAea::to<double>(slhaea.at("YD").at(2,2).at(2))},
                                                      { 23, 0},
                                                      { 31, 0},
                                                      { 32, 0},
                                                      { 33, SLHAea::to<double>(slhaea.at("YD").at(3,3).at(2))}
            };

            vevaciousPlusPlus.ReadLhaBlock( "YD", scale , Yd, 2 );

            std::vector<std::pair<int,double>> Ye = { { 11 , SLHAea::to<double>(slhaea.at("YE").at(1,1).at(2))},
                                                      { 12, 0},
                                                      { 13, 0},
                                                      { 21, 0},
                                                      { 22, SLHAea::to<double>(slhaea.at("YE").at(2,2).at(2))},
                                                      { 23, 0},
                                                      { 31, 0},
                                                      { 32, 0},
                                                      { 33, SLHAea::to<double>(slhaea.at("YE").at(3,3).at(2))}
            };

            vevaciousPlusPlus.ReadLhaBlock( "YE", scale , Ye, 2 );
        } else { // If NOT diagonal pass values to Vevacious
            std::vector<std::pair<int, double>> Yu = {{11, SLHAea::to<double>(slhaea.at("YU").at(1, 1).at(2))},
                                                      {12, SLHAea::to<double>(slhaea.at("YU").at(1, 2).at(2))},
                                                      {13, SLHAea::to<double>(slhaea.at("YU").at(1, 3).at(2))},
                                                      {21, SLHAea::to<double>(slhaea.at("YU").at(2, 1).at(2))},
                                                      {22, SLHAea::to<double>(slhaea.at("YU").at(2, 2).at(2))},
                                                      {23, SLHAea::to<double>(slhaea.at("YU").at(2, 3).at(2))},
                                                      {31, SLHAea::to<double>(slhaea.at("YU").at(3, 1).at(2))},
                                                      {32, SLHAea::to<double>(slhaea.at("YU").at(3, 2).at(2))},
                                                      {33, SLHAea::to<double>(slhaea.at("YU").at(3, 3).at(2))}
            };

            vevaciousPlusPlus.ReadLhaBlock("YU", scale, Yu, 2);

            std::vector<std::pair<int, double>> Yd = {{11, SLHAea::to<double>(slhaea.at("YD").at(1, 1).at(2))},
                                                      {12, SLHAea::to<double>(slhaea.at("YD").at(1, 2).at(2))},
                                                      {13, SLHAea::to<double>(slhaea.at("YD").at(1, 3).at(2))},
                                                      {21, SLHAea::to<double>(slhaea.at("YD").at(2, 1).at(2))},
                                                      {22, SLHAea::to<double>(slhaea.at("YD").at(2, 2).at(2))},
                                                      {23, SLHAea::to<double>(slhaea.at("YD").at(2, 3).at(2))},
                                                      {31, SLHAea::to<double>(slhaea.at("YD").at(3, 1).at(2))},
                                                      {32, SLHAea::to<double>(slhaea.at("YD").at(3, 2).at(2))},
                                                      {33, SLHAea::to<double>(slhaea.at("YD").at(3, 3).at(2))}
            };

            vevaciousPlusPlus.ReadLhaBlock("YD", scale, Yd, 2);

            std::vector<std::pair<int, double>> Ye = {{11, SLHAea::to<double>(slhaea.at("YE").at(1, 1).at(2))},
                                                      {12, SLHAea::to<double>(slhaea.at("YE").at(1, 2).at(2))},
                                                      {13, SLHAea::to<double>(slhaea.at("YE").at(1, 3).at(2))},
                                                      {21, SLHAea::to<double>(slhaea.at("YE").at(2, 1).at(2))},
                                                      {22, SLHAea::to<double>(slhaea.at("YE").at(2, 2).at(2))},
                                                      {23, SLHAea::to<double>(slhaea.at("YE").at(2, 3).at(2))},
                                                      {31, SLHAea::to<double>(slhaea.at("YE").at(3, 1).at(2))},
                                                      {32, SLHAea::to<double>(slhaea.at("YE").at(3, 2).at(2))},
                                                      {33, SLHAea::to<double>(slhaea.at("YE").at(3, 3).at(2))}
            };

            vevaciousPlusPlus.ReadLhaBlock("YE", scale, Ye, 2);
        }
            std::vector<std::pair<int, double>> Tu = {{11, SLHAea::to<double>(slhaea.at("TU").at(1, 1).at(2))},
                                                      {12, SLHAea::to<double>(slhaea.at("TU").at(1, 2).at(2))},
                                                      {13, SLHAea::to<double>(slhaea.at("TU").at(1, 3).at(2))},
                                                      {21, SLHAea::to<double>(slhaea.at("TU").at(2, 1).at(2))},
                                                      {22, SLHAea::to<double>(slhaea.at("TU").at(2, 2).at(2))},
                                                      {23, SLHAea::to<double>(slhaea.at("TU").at(2, 3).at(2))},
                                                      {31, SLHAea::to<double>(slhaea.at("TU").at(3, 1).at(2))},
                                                      {32, SLHAea::to<double>(slhaea.at("TU").at(3, 2).at(2))},
                                                      {33, SLHAea::to<double>(slhaea.at("TU").at(3, 3).at(2))}
            };

		vevaciousPlusPlus.ReadLhaBlock( "TU", scale , Tu, 2 );

        std::vector<std::pair<int,double>> Td = { { 11 , SLHAea::to<double>(slhaea.at("TD").at(1,1).at(2))},
                                                  { 12, SLHAea::to<double>(slhaea.at("TD").at(1,2).at(2))},
                                                  { 13, SLHAea::to<double>(slhaea.at("TD").at(1,3).at(2))},
                                                  { 21, SLHAea::to<double>(slhaea.at("TD").at(2,1).at(2))},
                                                  { 22, SLHAea::to<double>(slhaea.at("TD").at(2,2).at(2))},
                                                  { 23, SLHAea::to<double>(slhaea.at("TD").at(2,3).at(2))},
                                                  { 31, SLHAea::to<double>(slhaea.at("TD").at(3,1).at(2))},
                                                  { 32, SLHAea::to<double>(slhaea.at("TD").at(3,2).at(2))},
                                                  { 33, SLHAea::to<double>(slhaea.at("TD").at(3,3).at(2))}
        };
													
		vevaciousPlusPlus.ReadLhaBlock( "TD", scale , Td, 2 );

        std::vector<std::pair<int,double>> Te = { { 11 , SLHAea::to<double>(slhaea.at("TE").at(1,1).at(2))},
                                                  { 12, SLHAea::to<double>(slhaea.at("TE").at(1,2).at(2))},
                                                  { 13, SLHAea::to<double>(slhaea.at("TE").at(1,3).at(2))},
                                                  { 21, SLHAea::to<double>(slhaea.at("TE").at(2,1).at(2))},
                                                  { 22, SLHAea::to<double>(slhaea.at("TE").at(2,2).at(2))},
                                                  { 23, SLHAea::to<double>(slhaea.at("TE").at(2,3).at(2))},
                                                  { 31, SLHAea::to<double>(slhaea.at("TE").at(3,1).at(2))},
                                                  { 32, SLHAea::to<double>(slhaea.at("TE").at(3,2).at(2))},
                                                  { 33, SLHAea::to<double>(slhaea.at("TE").at(3,3).at(2))}
        };
													
		vevaciousPlusPlus.ReadLhaBlock( "TE", scale , Te, 2 );


        std::vector<std::pair<int,double>> msq2 = { { 11 , SLHAea::to<double>(slhaea.at("MSQ2").at(1,1).at(2))},
                                                  { 12, SLHAea::to<double>(slhaea.at("MSQ2").at(1,2).at(2))},
                                                  { 13, SLHAea::to<double>(slhaea.at("MSQ2").at(1,3).at(2))},
                                                  { 21, SLHAea::to<double>(slhaea.at("MSQ2").at(2,1).at(2))},
                                                  { 22, SLHAea::to<double>(slhaea.at("MSQ2").at(2,2).at(2))},
                                                  { 23, SLHAea::to<double>(slhaea.at("MSQ2").at(2,3).at(2))},
                                                  { 31, SLHAea::to<double>(slhaea.at("MSQ2").at(3,1).at(2))},
                                                  { 32, SLHAea::to<double>(slhaea.at("MSQ2").at(3,2).at(2))},
                                                  { 33, SLHAea::to<double>(slhaea.at("MSQ2").at(3,3).at(2))}
        };
													
		vevaciousPlusPlus.ReadLhaBlock( "MSQ2", scale , msq2, 2 );

        std::vector<std::pair<int,double>> msl2 = { { 11 , SLHAea::to<double>(slhaea.at("MSL2").at(1,1).at(2))},
                                                    { 12, SLHAea::to<double>(slhaea.at("MSL2").at(1,2).at(2))},
                                                    { 13, SLHAea::to<double>(slhaea.at("MSL2").at(1,3).at(2))},
                                                    { 21, SLHAea::to<double>(slhaea.at("MSL2").at(2,1).at(2))},
                                                    { 22, SLHAea::to<double>(slhaea.at("MSL2").at(2,2).at(2))},
                                                    { 23, SLHAea::to<double>(slhaea.at("MSL2").at(2,3).at(2))},
                                                    { 31, SLHAea::to<double>(slhaea.at("MSL2").at(3,1).at(2))},
                                                    { 32, SLHAea::to<double>(slhaea.at("MSL2").at(3,2).at(2))},
                                                    { 33, SLHAea::to<double>(slhaea.at("MSL2").at(3,3).at(2))}
        };
													
		vevaciousPlusPlus.ReadLhaBlock( "MSL2", scale , msl2, 2 );

        std::vector<std::pair<int,double>> msd2 = { { 11 , SLHAea::to<double>(slhaea.at("MSD2").at(1,1).at(2))},
                                                    { 12, SLHAea::to<double>(slhaea.at("MSD2").at(1,2).at(2))},
                                                    { 13, SLHAea::to<double>(slhaea.at("MSD2").at(1,3).at(2))},
                                                    { 21, SLHAea::to<double>(slhaea.at("MSD2").at(2,1).at(2))},
                                                    { 22, SLHAea::to<double>(slhaea.at("MSD2").at(2,2).at(2))},
                                                    { 23, SLHAea::to<double>(slhaea.at("MSD2").at(2,3).at(2))},
                                                    { 31, SLHAea::to<double>(slhaea.at("MSD2").at(3,1).at(2))},
                                                    { 32, SLHAea::to<double>(slhaea.at("MSD2").at(3,2).at(2))},
                                                    { 33, SLHAea::to<double>(slhaea.at("MSD2").at(3,3).at(2))}
        };
													
		vevaciousPlusPlus.ReadLhaBlock( "MSD2", scale , msd2, 2 );

        std::vector<std::pair<int,double>> mse2 = { { 11 , SLHAea::to<double>(slhaea.at("MSE2").at(1,1).at(2))},
                                                    { 12, SLHAea::to<double>(slhaea.at("MSE2").at(1,2).at(2))},
                                                    { 13, SLHAea::to<double>(slhaea.at("MSE2").at(1,3).at(2))},
                                                    { 21, SLHAea::to<double>(slhaea.at("MSE2").at(2,1).at(2))},
                                                    { 22, SLHAea::to<double>(slhaea.at("MSE2").at(2,2).at(2))},
                                                    { 23, SLHAea::to<double>(slhaea.at("MSE2").at(2,3).at(2))},
                                                    { 31, SLHAea::to<double>(slhaea.at("MSE2").at(3,1).at(2))},
                                                    { 32, SLHAea::to<double>(slhaea.at("MSE2").at(3,2).at(2))},
                                                    { 33, SLHAea::to<double>(slhaea.at("MSE2").at(3,3).at(2))}
        };
													
		vevaciousPlusPlus.ReadLhaBlock( "MSE2", scale , mse2, 2 );

        std::vector<std::pair<int,double>> msu2 = { { 11 , SLHAea::to<double>(slhaea.at("MSU2").at(1,1).at(2))},
                                                    { 12, SLHAea::to<double>(slhaea.at("MSU2").at(1,2).at(2))},
                                                    { 13, SLHAea::to<double>(slhaea.at("MSU2").at(1,3).at(2))},
                                                    { 21, SLHAea::to<double>(slhaea.at("MSU2").at(2,1).at(2))},
                                                    { 22, SLHAea::to<double>(slhaea.at("MSU2").at(2,2).at(2))},
                                                    { 23, SLHAea::to<double>(slhaea.at("MSU2").at(2,3).at(2))},
                                                    { 31, SLHAea::to<double>(slhaea.at("MSU2").at(3,1).at(2))},
                                                    { 32, SLHAea::to<double>(slhaea.at("MSU2").at(3,2).at(2))},
                                                    { 33, SLHAea::to<double>(slhaea.at("MSU2").at(3,3).at(2))}
        };
													
		vevaciousPlusPlus.ReadLhaBlock( "MSU2", scale , msu2, 2 );

		double M0input = *myPipe::Param["M0"];
		double M12input = *myPipe::Param["M12"];
		double A0input = *myPipe::Param["A0"];
		double TanBetainput = *myPipe::Param["TanBeta"];
		double SignMuinput = *myPipe::Param["SignMu"];

		std::ostringstream InputsForLog;
        //spectrumHE.writeSLHAfile(2, "SpecBit/VevaciousTest.slha");
        InputsForLog << std::fixed << std::setprecision(12) << "Running Vevacious with parameters: " << "M0="  << M0input << " M12="  << M12input << " A0=" << A0input << " Tanb=" << TanBetainput << " Sign Mu=" << SignMuinput ;
        std::string InputsForLogString = InputsForLog.str();
        logger() << InputsForLogString << EOM;
	    // Tell Vevacious we are using the point we just read by giving it "internal".
        try {
            //spectrumHE.writeSLHAfile(2, "SpecBit/ProblemPoint.slha");
            // Run vevacious
            struct stat buffer; //Checking if file exists, fastest method.
            std::string HomotopyLockfile = inputspath + "/Homotopy/busy.lock";
            // Check if homotopy binary is being used

            //Here I check it the busy.lock file exists and if it does I go into a
            // while loop that either breaks when the file is deleted or after
            // 30 seconds have passed.
            // This deals with the problem of MARCONI not liking a binary accessed by too many
            // processes at the same time

            std::chrono::system_clock::time_point tStart = Utils::get_clock_now();

            while(stat(HomotopyLockfile.c_str(), &buffer)==0)
            {
            std::chrono::system_clock::time_point tNow = Utils::get_clock_now();

            std::chrono::seconds tSofar = std::chrono::duration_cast<std::chrono::seconds>(tNow - tStart);

            if(tSofar >= std::chrono::seconds(30) ){
                remove( HomotopyLockfile.c_str());
                break; }

            }

            vevaciousPlusPlus.RunPoint("internal");

            double lifetime= vevaciousPlusPlus.GetLifetimeInSeconds();
            double thermalProbability = vevaciousPlusPlus.GetThermalProbability();

            if(lifetime == -1 && thermalProbability == -1 ){ // Here -1 from Vevacious Means that the point is stable. 
                lifetimeAndThermalProbability.first = 3.0E+100;
                lifetimeAndThermalProbability.second= 1;
            }
            else if(lifetime == -1 && thermalProbability != -1)
            {
                lifetimeAndThermalProbability.first = 3.0E+100;
                lifetimeAndThermalProbability.second= thermalProbability;
            }
            else if(lifetime != -1 && thermalProbability == -1)
            {
                lifetimeAndThermalProbability.first = lifetime;
                lifetimeAndThermalProbability.second= 1;
            }
            else {

                lifetimeAndThermalProbability.first = lifetime;
                lifetimeAndThermalProbability.second= thermalProbability;
            }
            cout << "VEVACIOUS LIFETIME:  "<< lifetimeAndThermalProbability.first << endl;
            cout << "VEVACIOUS Prob. non zero temp:  "<< lifetimeAndThermalProbability.second << endl;
            std::string result = vevaciousPlusPlus.GetResultsAsString();
            cout << "VEVACIOUS RESULT:  "<< result << endl;
        }
        catch(const std::exception& e)
        {
            //spectrumHE.writeSLHAfile(2, "SpecBit/VevaciousCrashed.slha");
            lifetimeAndThermalProbability.first = 2.0E+100; //Vevacious has crashed
            lifetimeAndThermalProbability.second= 1;
            cout << "VEVACIOUS LIFETIME:  "<< lifetimeAndThermalProbability.first << endl;
            std::string result = "Inconclusive";
            cout << "VEVACIOUS RESULT:  "<< result << endl;
            cout << "Vevacious could not calculate lifetime. Conservatively setting it to large value."<<endl;
            //std::ostringstream errmsg;
            //errmsg << "Vevacious could not calculate lifetime. Conservatively setting it to large value." << rankstring;
            //SpecBit_error().forced_throw(LOCAL_INFO,errmsg.str());
        }

 	}



 	void get_likelihood_VS_MSSM(double &result)
    {
        namespace myPipe = Pipes::get_likelihood_VS_MSSM;
        double lifetime =  myPipe::Dep::check_stability_MSSM->first;

        // This is based on the estimation of the past lightcone from 1806.11281
        double conversion = (6.5821195e-25)/(31536000);
        result=((- ( 1 / ( lifetime/conversion ) ) * exp(140) * (1/ (1.2e19) ) )  );
    }

    void get_likelihood_VS_MSSM_thermal(double &result)
    {
        namespace myPipe = Pipes::get_likelihood_VS_MSSM_thermal;
        double ThermalProbability =  myPipe::Dep::check_stability_MSSM->second;
         if(ThermalProbability == 0)
         {
             result = -1e100;
         } else
         {
             result= std::log(ThermalProbability);
         }

    }


  } // end namespace SpecBit
} // end namespace Gambit

