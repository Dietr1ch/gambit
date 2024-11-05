//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Example of GAMBIT NeutrinoBit standalone
///  main program.
///
///  *********************************************
///
///  Victims 💀 (add name and date if you modify):
///
///  \author Dietrich Daroch
///  \date 2024
///
///  *********************************************

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "gambit/Elements/standalone_module.hpp"
#include "gambit/Logs/logger.hpp"
#include "gambit/NeutrinoBit/NeutrinoBit_rollcall.hpp"
#include "gambit/NeutrinoBit/NeutrinoInterpolator.hpp"

namespace Gambit {
namespace NeutrinoBit {
void eval(const std::filesystem::path &file_path,
          const std::vector<double> &xs) {
  std::cout << "Reading '" << file_path << "'..." << std::endl;
  auto interp = NeutrinoInterpolator{/*file_name=*/file_path};

  std::cout << "Interpolating:" << std::endl;
  for (double x : xs) {
    std::cout << "  interp(" << x << ")=" << interp.eval(/*x=*/x) << std::endl;
  }
}
} // namespace NeutrinoBit
} // namespace Gambit

int main(int argc, char *argv[]) {
  std::cout << "Running NeutrinoBit Test standalone program!" << std::endl;

  // Parse Args
  // ==========
  if (argc <= 2) {
    std::cout << "Usage: ./standalone FILE_PATH [VALUES..]" << std::endl;
    return 123;
  }
  const std::filesystem::path file_path{argv[1]};
  std::vector<double> xs;
  for (auto i = 2; i < argc; i++) {
    const double x = std::stod(argv[i]);
    xs.push_back(x);
  }

  // Run
  // ===
  Gambit::NeutrinoBit::eval(file_path, xs);

  return 0;
}
