{
  inputs = {
    nixpkgs = {
      url = "github:cachix/devenv-nixpkgs/rolling";
    };
    systems = {
      url = "github:nix-systems/default";
    };

    devenv = {
      url = "github:cachix/devenv";
      inputs = {
        nixpkgs.follows = "nixpkgs";
      };
    };
  };

  nixConfig = {
    extra-trusted-public-keys = "devenv.cachix.org-1:w1cLUi8dv3hnoSPGAuibQv+f9TZLr6cv/Hm9XgU50cw=";
    extra-substituters = "https://devenv.cachix.org";
  };

  outputs = {
    self,
    nixpkgs,
    devenv,
    systems,
    ...
  } @ inputs: let
    forEachSystem = nixpkgs.lib.genAttrs (import systems);
  in {
    packages = forEachSystem (system: {
      devenv-up = self.devShells.${system}.default.config.procfileScript;
      devenv-test = self.devShells.${system}.default.config.test;
    });

    devShells =
      forEachSystem
      (system: let
        pkgs = nixpkgs.legacyPackages.${system};
      in {
        default = devenv.lib.mkShell {
          inherit inputs pkgs;
          modules = [
            {
              env = {
                "DICTIONARY" = "en_GB";
              };

              # https://devenv.sh/reference/options/
              languages = {
                cplusplus = {
                  # https://devenv.sh/reference/options/#languagescplusplusenable
                  enable = true;
                };
                fortran = {
                  # https://devenv.sh/reference/options/#languagesfortranenable
                  enable = true;
                };
                python = {
                  # https://devenv.sh/reference/options/#languagespythonenable
                  enable = true;
                };
              };

              # https://devenv.sh/reference/options/#packages
              packages = with pkgs;
                [
                  # Env
                  git
                  just
                  sqlite
                  doxygen

                  # LSP
                  cmake-language-server

                  # Build system
                  pkg-config
                  gnumake
                  cmake
                  mold

                  # Libs
                  boost
                  gsl
                  eigen
                  lapack
                  liblapack
                  libuuid
                  root

                  # More packages
                  castxml
                  zlib

                  # Fortran
                  gfortran # IDK why this isn't included when using languages.fortran.enable.

                  # Graphs
                  graphviz
                  gnuplot
                  # Misc
                  axel
                  mpi
                  highfive
                ]
                ++ (with python311Packages; [
                  ipython

                  pyyaml
                  future
                  pybind11

                  cython
                  scipy
                  numpy
                  dill
                  six
                  pandas
                  h5py
                  # pyhf
                  numba
                  configobj
                  tensorflow
                ]);

              # https://devenv.sh/reference/options/#pre-commit
              pre-commit = {
                # https://devenv.sh/reference/options/#pre-commithooks
                hooks = {
                  # Filesystem
                  check-symlinks = {
                    enable = true;
                  };

                  # TOML
                  check-toml = {
                    enable = true;
                  };

                  # Spelling
                  hunspell = {
                    enable = true;
                    entry = "${pkgs.hunspell}/bin/hunspell -d 'en_GB' -p .spelling/dictionary -l";
                  };

                  # Secrets
                  ripsecrets = {
                    enable = true;
                  };

                  # Nix
                  alejandra = {
                    enable = true;
                  };
                };
              };

              enterShell = ''
                git --version
                echo

                make --version
                cmake --version
                echo

                mold --version
                echo

                gfortran --version

                g++ --version

                clang++ --version
                echo

                python --version
              '';
            }
          ];
        };
      });
  };
}
