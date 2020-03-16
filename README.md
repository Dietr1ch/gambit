GAMBITGAMBIT
======

GAMBIT (the Global And Modular BSM Inference Tool) is a software code for performing statistical global fits of generic physics models using a wide range of particle physics and astrophysics data. It consists of a series of modules that provide native simulations of collider and astrophysics experiments, a flexible system for interfacing external codes (the "backend" system), a fully featured statistical and parameter scanning framework, and a host of tools for implementing and using hierarchical models.

Citation(s)
--

Please cite the following GAMBIT papers, depending on your use of different modules:

 - GAMBIT Collaboration: P. Athron, et. al., **GAMBIT**: The Global and Modular Beyond-the-Standard-Model Inference Tool, Eur. Phys. J. C 77 (2017) 784, arXiv:1705.07908
 - GAMBIT Collider Workgroup: C. Balazs, et. al., **ColliderBit**: A GAMBIT module for the calculation of high energy collider observables and likelihoods, Eur. Phys. J. C 77 (2017) 795, arXiv:1705.07919
 - GAMBIT Models Workgroup: P. Athron, et. al., **SpecBit, DecayBit and PrecisionBit**: GAMBIT modules for computing mass spectra, particle decay rates and precision observables, Eur. Phys. J. C 78 (2018) 22, arXiv:1705.07936
 - GAMBIT Flavour Workgroup: F. U. Bernlochner, et. al., **FlavBit**: A GAMBIT module for computing flavour observables and likelihoods, Eur. Phys. J. C 77 (2017) 786, arXiv:1705.07933
 - GAMBIT Scanner Workgroup: G. D. Martinez, et. al., Comparison of statistical sampling methods with **ScannerBit**, the GAMBIT scanning module, Eur. Phys. J. C 77 (2017) 761, arXiv:1705.07959
 - GAMBIT Dark Matter Workgroup: T. Bringmann, et. al., **DarkBit**: A GAMBIT module for computing dark matter observables and likelihoods, Eur. Phys. J. C 77 (2017) 831, arXiv:1705.07920

GAMBIT contains interfaces to various external codes, along with scripts for downloading and configuring them. Please cite as appropriate if you use those codes:

 - **Capt'n General** GAMBIT Collaboration: P. Athron, C. Balazs, et. al., Global analyses of Higgs portal singlet dark matter models using GAMBIT, Eur. Phys. J. C 79 (2019) 38, arXiv:1808.10465
 - **DarkSUSY:** P. Gondolo, J. Edsjo, et. al., DarkSUSY: computing supersymmetric dark matter properties numerically, JCAP 7 (2004) 8, arXiv: astro-ph/0406204
 - **DDCalc:** GAMBIT Dark Matter Workgroup: T. Bringmann, J. Conrad, et. al., DarkBit: A GAMBIT module for computing dark matter observables and likelihoods, Eur. Phys. J. C 77 (2017) 831, arXiv:1705.07920
 - **FeynHiggs:** **(i)** H. Bahl, W. Hollik, Precise prediction for the light MSSM Higgs boson mass combining effective field theory and fixed-order calculations, Eur. Phys. J. C 76 (2016) 499,  arXiv:1608.01880, **(ii)** T. Hahn, S. Heinemeyer, W. Hollik, H. Rzehak, G. Weiglein, High-precision predictions for the light CP-even Higgs Boson Mass of the MSSM, Phys. Rev. Lett. 112 (2014) 141801, arXiv:1312.4937, **(iii)** M. Frank, T. Hahn, S. Heinemeyer, W. Hollik, H. Rzehak, G. Weiglein, The Higgs Boson Masses and Mixings of the Complex MSSM in the Feynman-Diagrammatic Approach, JHEP 0702 (2007) 047, arXiv:hep-ph/0611326, **(iv)** G. Degrassi, S. Heinemeyer, W. Hollik, P. Slavich, G. Weiglein, Towards High-Precision Predictions for the MSSM Higgs Sector, Eur. Phys. J. C 28 (2003) 133, arXiv:hep-ph/0212020, **(v)** S. Heinemeyer, W. Hollik, G. Weiglein, The Masses of the Neutral CP-even Higgs Bosons in the MSSM: Accurate Analysis at the Two-Loop Level, Eur.Phys.J.C9:343-366 (1999), arXiv:hep-ph/9812472, **(vi)** S. Heinemeyer, W. Hollik, G. Weiglein, FeynHiggs: a program for the calculation of the masses of the neutral CP-even Higgs bosons in the MSSM, Comput. Phys. Commun. 124 (2000) 76, arXiv:hep-ph/9812320
 - **FlexibleSUSY:** P. Athron, J.-h. Park, D. Stöckinger, and A. Voigt, FlexibleSUSY - A spectrum generator generator for supersymmetric models, Comp. Phys. Comm. 190 (2015) 139, arXiv:1406.2319
 - **GamLike:** GAMBIT Dark Matter Workgroup: T. Bringmann, J. Conrad, et. al., DarkBit: A GAMBIT module for computing dark matter observables and likelihoods, Eur. Phys. J. C 77 (2017) 831, arXiv:1705.07920
 - **GM2Calc:** P. Athron, et. al., GM2Calc: precise MSSM prediction for (g-2) of the muon, Eur. Phys. J. C 76 (2016) 62, arXiv:1510.08071
 - **HiggsBounds:** **(i)** P. Bechtle, O. Brein, S. Heinemeyer, G. Weiglein, and K. E. Williams, HiggsBounds: Confronting Arbitrary Higgs Sectors with Exclusion Bounds from LEP and the Tevatron, Comp. Phys. Comm. 181 (2010) 138, arXiv:0811.4169, **(ii)** P. Bechtle, O. Brein, S. Heinemeyer, G. Weiglein, and K. E. Williams, HiggsBounds 2.0.0: Confronting Neutral and Charged Higgs Sector Predictions with Exclusion Bounds from LEP and the Tevatron, Comp. Phys. Comm. 182 (2011) 2605, arXiv:1102.1898, **(iii)** P. Bechtle, O. Brein, et. al., HiggsBounds - 4: Improved Tests of Extended Higgs Sectors against Exclusion Bounds from LEP, the Tevatron and the LHC, Eur. Phys. J. C 74 (2014) 2693, arXiv:1311.0055
 - **HiggsSignals:** P. Bechtle, S. Heinemeyer, O. Stal, T. Stefaniak, and G. Weiglein, HiggsSignals: Confronting arbitrary Higgs sectors with measurements at the Tevatron and the LHC, Eur. Phys. J. C 74 (2014) 2711, arXiv:1305.1933
 - **MicrOmegas:** G. Belanger, F. Boudjema, A. Pukhov, A. Semenov, micrOMEGAs4.1: Two dark matter candidates, Comp. Phys. Comm. 192 (2015) 322, arXiv:1407.6129
 - **Multinest:** F. Feroz, M. P. Hobson, M. Bridges, MULTINEST: an efficient and robust Bayesian inference tool for cosmology and particle physics, MNRAS 398 (2009) 1601–1614, arXiv:0809.3437
 - **nulike:** **(i)** IceCube Collaboration: M. G. Aartsen et. al., Improved limits on dark matter annihilation in the Sun with the 79-string IceCube detector and implications for supersymmetry, JCAP 04 (2016) 022, arXiv:1601.00653, **(ii)** P. Scott, C. Savage, J. Edsjo the IceCube Collaboration: R. Abbasi et al., Use of event-level neutrino telescope data in global fits for theories of new physics, JCAP 11 (2012) 57, arXiv:1207.0810
 - **PolyChord:** W. J. Handley, M. P. Hobson, M. P. A. N. Lasenby, POLYCHORD: next-generation nested sampling, MNRAS 453 (2015) 4384, arXiv:1506.00171
 - **Pythia 8:** **(i)** T. Sjostrand, S. Ask, et. al., An Introduction to PYTHIA 8.2, Comp. Phys. Comm. 191 (2015) 159, arXiv:1410.3012, **(ii)**  T. Sjostrand, Stephen Mrenna, Peter Skands, Pythia 6.4 Physics and Manual, JHEP 0605 (2006) 026, arXiv:hep-ph/0603175
 - **Spheno:** **(i)** W. Porod, SPheno, a program for calculating supersymmetric spectra, SUSY particle decays and SUSY particle production at e+ e- colliders, Comput. Phys. Commun. 153 (2003) 275, arXiv:hep-ph/0301101, **(ii)** W. Porod, F. Staub,  SPheno 3.1: extensions including flavour, CP-phases and models beyond the MSSM, Comp. Phys. Comm. 183 (2012) 2458, arXiv:1104.1573
 - **SuperIso:** **(i)** F. Mahmoudi, SuperIso: A Program for calculating the isospin asymmetry of B -> K* gamma in the MSSM, Comp. Phys. Comm. 178 (2008) 745, arXiv:0710.2067, **(ii)** F. Mahmoudi, SuperIso v2.3: A Program for calculating flavor physics observables in Supersymmetry, Comp. Phys. Comm. 180 (2009) 1579, arXiv:0808.3144, **(iii)** F. Mahmoudi, SuperIso v3.0, flavor physics observables calculations: Extension to NMSSM, Comp. Phys. Comm. 180 (2009) 1718.
 - **SUSY-HD:** J. P. Vega, G. Villadoro, SusyHD: Higgs mass Determination in Supersymmetry, JHEP 07 (2015) 159, arXiv:1504.05200
 - **SUSY-HIT:** A. Djouadi, M. M. Mühlleitner M. Spira, Decays of supersymmetric particles: The Program SUSY-HIT (SUspect-SdecaY-Hdecay-InTerface), Acta Phys. Polon. 38 (2007) 635, arXiv: hep-ph/0609292

Documentation
--

Please see the above list of GAMBIT papers for the main documentation on the design and use of the GAMBIT core and modules. Further details on the code are available via the doxygen documentation in the gambit/doc directory.

Supported Compilers and Library Dependencies
--

GAMBIT is built using the CMake system (version 2.8.12 or greater required). The following libraries and packages must be installed prior to configuration:

COMPULSORY:

 - gcc/gfortran 4.8.4 or greater, or icc/ifort 12.1.0 or greater
 - Python 2.7 or greater (Python 3 *is* now supported)
 - Python modules: yaml, os, re, datetime, sys, getopt, shutil and itertools.
 - git
 - Boost 1.41 or greater
 - GNU Scientific Library (GSL) 1.10 or greater
 - Eigen 3.1.0 or greater
 - LAPACK
 - PKG Config

OPTIONAL:

 - HDF5 (for use of the hdf5 printer)
 - the h5py python module (for use of the hdf5 printer)
 - MPI (required for parallel sampling)
 - axel (speeds up downloads of backends and scanners)
 - graphviz (required for model hierarchy and dependency tree plots)
 - ROOT (required for RestFrames support in ColliderBit, or the GreAT scanner from ScannerBit)


Building GAMBIT
--

The basic build instructions are as follows, but note that cmake will fail to find some dependencies on some systems without guidance. More information is provided in the paper "GAMBIT: The Global and Modular Beyond-the-Standard-Model Inference Tool" (see the top of this README file for a link). For a list of commonly used cmake options, see the file CMAKE_FLAGS.md. Specific cluster configuration examples are available via gambit.hepforge.org.

Assuming that you have retrieved the git repository or the tarball and unpacked it:

```
  cd gambit
  mkdir build
  cd build
  cmake ..
  make -jn scanners (where n specifies the desired number of cores for the build, e.g. 4)
  cmake ..
  make -jn gambit
```

To build all backend codes:
```
  make -jn backends
```

If you don't want to wait for all backends to build, or if one fails to compile on your system for whatever reason, you can also build individual backends using:
```
  make -jn <backend name>
```

and clean them with:
```
  make clean-<backend name>
```

Running GAMBIT
--

For complete documentation on running GAMBIT, please see the Core and module papers (the latter include examples for both running within the full GAMBIT framework, and running standalone module examples). Here we provide a few basic examples.

To see which backends and scanners have been installed correctly, do:

```
  gambit backends
  gambit scanners
```

To run the GAMBIT spartan example, do
```
  gambit -f yaml_files/spartan.yaml
```

To run an example GAMBIT MSSM7 scan, do:
```
  gambit -f yaml_files/MSSM7.yaml
```

Other examples are provided in the yaml_files folder.  Further readmes and documentation can also be found in the doc folder.


Licensing
--

The BSD license below applies to all source files in the GAMBIT distribution, except for those contained in the contrib/fjcore-3.1.3 and contrib/MassSpectra directories.  The files in those directories belong to the FastJet and FlexibleSUSY/SOFTSUSY projects respectively, and are distributed under the GNU General Public License, with special exception (granted by the authors of these packages to GAMBIT) that their inclusion in GAMBIT does not require that the rest of GAMBIT be distributed under the GPL.  Note that all provisions of the GPL continue to apply to any further redistribution of the files in contrib/fjcore-3.1.3 and contrib/MassSpectra, whether in source or binary form.

License
--
Copyright (c) 2017, The GAMBIT Collaboration
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
