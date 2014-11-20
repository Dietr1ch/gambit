!#######################################################################
MODULE DDCALC0
!#######################################################################

!#######################################################################
! DIRECT DETECTION RATES AND LIKELIHOODS (SIMPLE VERSION)
! Routines to calculate the dark matter direct detection event rates
! and corresponding likelihoods/exclusion levels.
! 
! To compile:
!     gfortran -O3 -fno-range-check -o DDCalc0 DDCalc0.f90
!     ifort -fast -o DDCalc0 DDCalc0.f90
! 
! To see usage:
!     ./DDCalc0 --help
! 
! 
!   Created by Chris Savage
!   University of Utah   (2013 - 2014)
!   Nordita              (2014 -     )
! 
!   With contributions from:
!     Andre Scaffidi            University of Adelaide (2014)
! 
! 
! 
!   --------========<<<<<<<< FILE LAYOUT >>>>>>>>========--------
! 
! PUBLIC ROUTINE DECLARATIONS
!   Declares which routines can be accessed externally.  Routines
!   not explicitly listed here are not externally accessible due to
!   globally declared default private scope.
! 
! CONSTANTS/GLOBAL VARIABLES/STRUCTURES
!   Contains various constants, structures definitions, and global
!   variables used throughout the module.
! 
! INTERFACES
!   Define names for accessing routines (useful in some circumstances).
! 
! MODULE ROUTINES
!  * SIMPLE INTERFACE ROUTINES
!    Basic routines that can be used to initialize the module and
!    set WIMP parameters.  Meant to provide a simple, basic interface
!    for external access to the module.
! Experiment specific:
!  * LUX 2013 ROUTINES
!    Routines for calculating rates/likelihoods for the LUX 2013
!    analysis.
!  * DARWIN ARGON 2014 ROUTINES
!    Routines for calculating rates/likelihoods for the proposed
!    DARWIN argon-based detector, using a 2014 parameters estimate.
!  * DARWIN XENON 2014 ROUTINES
!    Routines for calculating rates/likelihoods for the proposed
!    DARWIN xenon-based detector, using a 2014 parameters estimate.
! Generic:
!  * MAIN ROUTINES
!    Main routines used for each program mode.
!  * INITIALIZATION
!    Various general initialization routines.
!  * OUTPUT ROUTINES
!    Routines for printing out info, parameters, and results.
!  * WIMP ROUTINES
!    Routines for initializing, modifying, or viewing WIMP properties,
!    mainly the mass and couplings.
!  * COUPLING CONVERSION ROUTINES
!    Routines for converting between WIMP-nucleon couplings G, f & a,
!    with different notation corresponding to different normalization.
!  * CROSS-SECTION/COUPLING CONVERSION ROUTINES
!    Routines to convert between WIMP-nucleon couplings G and WIMP-
!    nucleon cross-sections sigma.
!  * HALO ROUTINES
!    Routines for initializing and modifying the dark matter halo
!    distribution, as well as perform the mean inverse speed (eta)
!    calculation needed for recoil rates.
!  * ISOTOPES/NUCLEAR ROUTINES (INCLUDING FORM FACTORS)
!    Routines for determining isotope and nuclear properties, including
!    isotopic compositions, nuclear masses, and form factors (both
!    spin-independent and spin-dependent).
!    [NOTE: LIMITED IMPLEMENTATION FOR SD CASE -- MOSTLY SET TO ZERO.]
!  * DETECTOR SETUP ROUTINES
!    Routines for initializing and modifying the detector setup,
!    notatably by setting isotopes to use, loading efficiencies from
!    file, and tabulating form factors.
!  * RATE ROUTINES
!    Routines to calculate and view rates.  The CalcRates() routine
!    here must be called after changing the WIMP mass and/or couplings.
!  * LIKELIHOOD/P-VALUE ROUTINES
!    Routines to calculate the log-likelihood (Poisson with background)
!    or p-value (Poisson or Maximum Gap method, without background
!    subtraction) of the current result.  Must have called CalcRates()
!    for these routines to return appropriate values for the current
!    WIMP mass and couplings.
!  * USAGE
!    Routines for outputting usage instructions.
!  * COMMAND LINE PROCESSING
!    Utility routines for processing command line arguments, mainly
!    of the form '-x', '--flag', or '--key=value'.
!  * EXPORT/IMPORT DATA
!    Utility routines for import and export of data.
!  * TABULATION ROUTINES
!    Routines for determining tabulation points.
!  * INTERPOLATION
!    Interpolation routines, using several functional forms.
!  * RANDOM NUMBER GENERATOR
!    Internal random number generator routines (safe for use with
!    OpenMP).
!  * MATH FUNCTIONS
!    Various math functions.
!  * PROBABILITY DISTRIBUTIONS
!    Various probability related functions for several distributions.
!    NOTE: _not_ high precision in the case of Poisson and Binomial.
!  * ARRAY SORT/SEARCH
!    Routines to sort or search arrays.
!  * TIME UTILITY FUNCTIONS
!    Utility functions for examining wall or CPU time.
! 
! PROGRAM
!   A simple program that just calls the main() routine in the module.
! 
!#######################################################################


!#######################################################################
! SETUP
!#######################################################################

! All types must be explicit
IMPLICIT NONE

! Set default private.
! Constants/routines must be explicitly set to public for
! external access.
PRIVATE



!#######################################################################
! PUBLIC ROUTINE DECLARATIONS
!#######################################################################

! Here, we declare which of the routines defined below should have
! public access.  Due to the global private default scope, only
! routines explicitly listed here are externally accessible.
! 
! Interfaces are used to provide an exernally accessible name that
! differs from the actual function name (the latter will remain only
! privately accessible).  The externally accessible names all include
! a 'DDCalc0_' prefix.
! 
! Public constants are declared in the constants section below.

! SIMPLE INTERFACE ROUTINES --------------------------------------------

! These are basic routines for externally accessing the module that
! can be used for initialization and setting WIMP parameters.  These
! are meant to allow for a simpler interface to this module; other
! routines are more robust and provide more capabilities.

! Module initialization:
!     SUBROUTINE DDCalc0_Init()
PUBLIC :: DDCalc0_Init

! Standard Halo Model (SHM) settings:
!     SUBROUTINE DDCalc0_SetSHM(rho,vrot,v0,vesc)
! where the arguments are the local dark matter density [GeV/cm^3],
! the local disk rotation speed [km/s], the most probable dark matter
! speed in the galactic rest frame [km/s], and the galactic escape
! speed [km/s].  This routine need only be called if non-default
! values are desired (0.4, 235, 235, and 550, respectively).
PUBLIC :: DDCalc0_SetSHM

! WIMP parameter settings:
!     SUBROUTINE DDCalc0_SetWIMP_mfa(m,fp,fn,ap,an)
!     SUBROUTINE DDCalc0_SetWIMP_mG(m,GpSI,GnSI,GpSD,GnSD)
!     SUBROUTINE DDCalc0_SetWIMP_msigma(m,sigmapSI,sigmanSI,sigmapSD,sigmanSD)
! where m is the WIMP mass [GeV], f is spin-independent (SI) WIMP-
! nucleon coupling [GeV^-2], a is the spin-dependent (SD) WIMP-nucleon
! coupling [unitless], G are the effective 4-fermion vertex couplings,
! related by:
!     GpSI = 2 fp        GpSD = 2\sqrt{2} G_F ap
!     GnSI = 2 fn        GnSD = 2\sqrt{2} G_F an
! and sigma is the WIMP-nucleon scattering cross-section [pb].
! Negative cross-sections indicate the corresponding coupling should
! be negative.  In all cases, 'p' refers to proton and 'n' to neutron.
PUBLIC :: DDCalc0_SetWIMP_mfa,DDCalc0_SetWIMP_mG,DDCalc0_SetWIMP_msigma


! EXPERIMENT-SPECIFIC ROUTINES -----------------------------------------

! To use any of the routines for a particular experiment, the
! corresponding init() routine must be called once.  The corresponding
! calc() routine must be called after each change in the WIMP
! parameters.

! Detector initialization:
!     SUBROUTINE Init(intervals)
! where intervals is logical indicating if calculations should be
! performed for analysis sub-intervals (i.e. intervals between
! observed events).  This is only necessary for maximum gap
! calculations and can be set to .FALSE. for likelihood analyses.
PUBLIC :: LUX_2013_Init,DARWIN_Ar_2014_Init,DARWIN_Xe_2014_Init

! Rate calculation:
!     SUBROUTINE CalcRates()
! Performs the rate calculations used for likelihoods/confidence
! intervals.  Must be called after any changes to the WIMP parameters.
! Actual calculation values are accessed through other routines.
PUBLIC :: LUX_2013_CalcRates,DARWIN_Ar_2014_CalcRates,DARWIN_Xe_2014_CalcRates

! Number of observed events in the analysis:
!     INTEGER FUNCTION Events()
PUBLIC :: LUX_2013_Events,DARWIN_Ar_2014_Events,DARWIN_Xe_2014_Events

! Average expected number of background events in the analysis:
!     REAL*8 FUNCTION Background()
PUBLIC :: LUX_2013_Background,DARWIN_Ar_2014_Background,DARWIN_Xe_2014_Background

! Average expected number of signal events in the analysis:
!     REAL*8 FUNCTION Signal()
! Or the separate spin-independent and spin-dependent contributions:
!     REAL*8 FUNCTION SignalSI()
!     REAL*8 FUNCTION SignalSD()
PUBLIC :: LUX_2013_Signal,  DARWIN_Ar_2014_Signal,  DARWIN_Xe_2014_Signal
PUBLIC :: LUX_2013_SignalSI,DARWIN_Ar_2014_SignalSI,DARWIN_Xe_2014_SignalSI
PUBLIC :: LUX_2013_SignalSD,DARWIN_Ar_2014_SignalSD,DARWIN_Xe_2014_SignalSD

! Log-likelihood for current WIMP:
!     REAL*8 FUNCTION LogLikelihood()
! Based upon a Poisson distribution in the number of observed events
! given the expected background+signal.  Calc() must be called first.
PUBLIC :: LUX_2013_LogLikelihood,DARWIN_Xe_2014_LogLikelihood,DARWIN_Ar_2014_LogLikelihood

! Logarithm of the p-value for current WIMP:
!     REAL*8 FUNCTION LogPValue()
! Based upon the maximum gap method if Init() called with intervals =
! .TRUE., a signal-only (no-background) Poisson distribution otherwise.
! Calc() must be called first.
PUBLIC :: LUX_2013_LogPValue,DARWIN_Xe_2014_LogPValue,DARWIN_Ar_2014_LogPValue

! Scale by which the current WIMP cross-sections must be multiplied to
! achieve the given p-value:
!     REAL*8 FUNCTION ScaleToPValue(lnp)
! where lnp is the logarithm of the desired p-value (p=1-CL).
! Calc() must be called first.
PUBLIC :: LUX_2013_ScaleToPValue,DARWIN_Xe_2014_ScaleToPValue,DARWIN_Ar_2014_ScaleToPValue

! Detector structure initialization (ADVANCED USAGE ONLY):
!     SUBROUTINE InitTo(D,intervals)
! where D is a DetectorRateStruct structure and intervals is as
! described for the initialization routines above.  The D structure
! can be used in the generic routines below.  These routines are
! not intended for standard usage, so ignore them unless you are
! familiar with the internals of this code.
PUBLIC :: LUX_2013_InitTo,DARWIN_Ar_2014_InitTo,DARWIN_Xe_2014_InitTo


! MAIN ROUTINES --------------------------------------------------------

! Main routine
PUBLIC :: DDCalc0_Main
INTERFACE DDCalc0_Main
  MODULE PROCEDURE main
END INTERFACE

! Main routines for specific running modes
PUBLIC :: DDCalc0_MainEventsAndLikelihoods,                             &
          DDCalc0_MainEventsAndLikelihoodsInteractive,                  &
          DDCalc0_MainLogLikelihood,DDCalc0_MainLogLikelihoodInteractive,&
          DDCalc0_MainLogPValue,DDCalc0_MainLogPValueInteractive,       &
          DDCalc0_MainSpectrum,DDCalc0_MainEventsByMass,                &
          DDCalc0_MainConstraintsSI,DDCalc0_MainConstraintsSD,          &
          DDCalc0_MainLimitsSI,DDCalc0_MainLimitsSD
INTERFACE DDCalc0_MainEventsAndLikelihoods
  MODULE PROCEDURE MainEventsAndLikelihoods
END INTERFACE
INTERFACE DDCalc0_MainEventsAndLikelihoodsInteractive
  MODULE PROCEDURE MainEventsAndLikelihoodsInteractive
END INTERFACE
INTERFACE DDCalc0_MainLogLikelihood
  MODULE PROCEDURE MainLogLikelihood
END INTERFACE
INTERFACE DDCalc0_MainLogLikelihoodInteractive
  MODULE PROCEDURE MainLogLikelihoodInteractive
END INTERFACE
INTERFACE DDCalc0_MainLogPValue
  MODULE PROCEDURE MainLogPValue
END INTERFACE
INTERFACE DDCalc0_MainLogPValueInteractive
  MODULE PROCEDURE MainLogPValueInteractive
END INTERFACE
INTERFACE DDCalc0_MainSpectrum
  MODULE PROCEDURE MainSpectrum
END INTERFACE
INTERFACE DDCalc0_MainEventsByMass
  MODULE PROCEDURE MainEventsByMass
END INTERFACE
INTERFACE DDCalc0_MainConstraintsSI
  MODULE PROCEDURE MainConstraintsSI
END INTERFACE
INTERFACE DDCalc0_MainConstraintsSD
  MODULE PROCEDURE MainConstraintsSD
END INTERFACE
INTERFACE DDCalc0_MainLimitsSI
  MODULE PROCEDURE MainLimitsSI
END INTERFACE
INTERFACE DDCalc0_MainLimitsSD
  MODULE PROCEDURE MainLimitsSD
END INTERFACE


! GENERIC ROUTINES -----------------------------------------------------

! Initialization routines
PUBLIC :: DDCalc0_Initialize,DDCalc0_InitializeCL
INTERFACE DDCalc0_Initialize
  MODULE PROCEDURE Initialize
END INTERFACE
INTERFACE DDCalc0_InitializeCL
  MODULE PROCEDURE InitializeCL
END INTERFACE

! WIMP mass & couplings routines
PUBLIC :: DDCalc0_GetWIMP,DDCalc0_SetWIMP,                              &
          DDCalc0_InitWIMP,DDCalc0_InitWIMPCL
INTERFACE DDCalc0_GetWIMP
  MODULE PROCEDURE GetWIMP
END INTERFACE
INTERFACE DDCalc0_SetWIMP
  MODULE PROCEDURE SetWIMP
END INTERFACE
INTERFACE DDCalc0_InitWIMP
  MODULE PROCEDURE InitWIMP
END INTERFACE
INTERFACE DDCalc0_InitWIMPCL
  MODULE PROCEDURE InitWIMPCL
END INTERFACE

! Dark matter halo distribution routines
PUBLIC :: DDCalc0_GetHalo,DDCalc0_SetHalo,                              &
          DDCalc0_InitHalo,DDCalc0_InitHaloCL
INTERFACE DDCalc0_GetHalo
  MODULE PROCEDURE GetHalo
END INTERFACE
INTERFACE DDCalc0_SetHalo
  MODULE PROCEDURE SetHalo
END INTERFACE
INTERFACE DDCalc0_InitHalo
  MODULE PROCEDURE InitHalo
END INTERFACE
INTERFACE DDCalc0_InitHaloCL
  MODULE PROCEDURE InitHaloCL
END INTERFACE

! Detector properties routines
PUBLIC :: DDCalc0_GetDetector,DDCalc0_SetDetector,                      &
          DDCalc0_InitDetector,DDCalc0_InitDetectorCL
INTERFACE DDCalc0_GetDetector
  MODULE PROCEDURE GetDetector
END INTERFACE
INTERFACE DDCalc0_SetDetector
  MODULE PROCEDURE SetDetector
END INTERFACE
INTERFACE DDCalc0_InitDetector
  MODULE PROCEDURE InitDetector
END INTERFACE
INTERFACE DDCalc0_InitDetectorCL
  MODULE PROCEDURE InitDetectorCL
END INTERFACE

! Rate calculation routines
PUBLIC :: DDCalc0_GetRates,DDCalc0_CalcRates
INTERFACE DDCalc0_GetRates
  MODULE PROCEDURE GetRates
END INTERFACE
INTERFACE DDCalc0_CalcRates
  MODULE PROCEDURE CalcRates
END INTERFACE

! Likelihood/p-value routines
PUBLIC :: DDCalc0_LogLikelihood,DDCalc0_LogPValue,DDCalc0_ScaleToPValue
INTERFACE DDCalc0_LogLikelihood
  MODULE PROCEDURE LogLikelihood
END INTERFACE
INTERFACE DDCalc0_LogPValue
  MODULE PROCEDURE LogPValue
END INTERFACE
INTERFACE DDCalc0_ScaleToPValue
  MODULE PROCEDURE ScaleToPValue
END INTERFACE



!#######################################################################
! CONSTANTS/GLOBAL VARIABLES/STRUCTURES
!#######################################################################

! VERSION --------------------------------------------------------------

! Version of this software
CHARACTER*(*), PUBLIC, PARAMETER :: VERSION_STRING = '0.11'


! MATH CONSTANTS -------------------------------------------------------

REAL*8, PUBLIC, PARAMETER :: PI         = 3.1415926535897932d0   ! Pi
REAL*8, PUBLIC, PARAMETER :: TWOPI      = 6.2831853071795865d0   ! 2*Pi
REAL*8, PUBLIC, PARAMETER :: HALFPI     = 1.5707963267948966d0   ! Pi/2
REAL*8, PUBLIC, PARAMETER :: SQRTPI     = 1.7724538509055160d0   ! Sqrt(Pi)
REAL*8, PUBLIC, PARAMETER :: SQRT2PI    = 2.5066282746310005d0   ! Sqrt(2*Pi)
REAL*8, PUBLIC, PARAMETER :: INVPI      = 0.31830988618379067d0  ! 1/Pi
REAL*8, PUBLIC, PARAMETER :: INV2PI     = 0.15915494309189534d0  ! 1/2Pi
REAL*8, PUBLIC, PARAMETER :: INVSQRTPI  = 0.56418958354775629d0  ! 1/Sqrt(Pi)
REAL*8, PUBLIC, PARAMETER :: INVSQRT2PI = 0.39894228040143268d0  ! 1/Sqrt(2*Pi)

REAL*8, PUBLIC, PARAMETER :: SQRT2  = 1.4142135623730950d0   ! Sqrt(2)
REAL*8, PUBLIC, PARAMETER :: SQRT3  = 1.7320508075688773d0   ! Sqrt(3)


! PHYSICS CONSTANTS ----------------------------------------------------

! Speed of light [m/s]
REAL*8, PUBLIC, PARAMETER :: SPEED_OF_LIGHT = 2.99792458d8

! Planck constant times speed of light [GeV fm]
REAL*8, PUBLIC, PARAMETER :: HBARC = 0.1973269718d0

! Fermi coupling constant, in units of /(hbar c)^3 [GeV^-2]
REAL*8, PUBLIC, PARAMETER :: FERMI_COUPLING_CONSTANT = 1.1663787d-5

! Proton and neutron masses [GeV]
REAL*8, PUBLIC, PARAMETER :: PROTON_MASS    = 0.9382720d0
REAL*8, PUBLIC, PARAMETER :: NEUTRON_MASS   = 0.9395654d0


! ARGUMENTS ------------------------------------------------------------

! Will store some command line arguments globally to make parsing
! easier.
!   options     Arguments that begin with '--', usually of the form
!               --<flag> or --<flag>=<value>
!   parameters  Arguments that are not options
!   values      Conversion of parameters to floating point (if possible)
! 
! Currently, only the parameters are used (options are parsed directly).
! 
TYPE, PRIVATE :: ArgumentStructure
  INTEGER :: Noptions    = -1
  INTEGER :: Nparameters = -1
  CHARACTER*256, ALLOCATABLE :: options(:)
  CHARACTER*256, ALLOCATABLE :: parameters(:)
  REAL*8, ALLOCATABLE :: values(:)
END TYPE
  
! For easier parsing, will store command line arguments here
TYPE (ArgumentStructure), PRIVATE :: Arguments


! VERBOSITY ------------------------------------------------------------

! Various parameters that are not accounted for elsewhere

! Verbosity
! Affects the level of output.  Generally, higher magnitude
! means more output; positive will include headers, while
! negative will not.
! 
INTEGER, PRIVATE :: VerbosityLevel = 1


! WIMP -----------------------------------------------------------------

! Structure containing WIMP mass and couplings.
! 
TYPE, PRIVATE :: WIMPStruct
  ! WIMP mass [GeV]
  REAL*8 :: m
  
  ! Effective couplings to the proton and neutron in units of [GeV^-2].
  ! In terms of more commonly used notation:
  !   SI (scalar):        G = 2f
  !   SD (axial-vector):  G = 2\sqrt{2} G_F a
  ! where G, f, and a have 'p' and 'n' subscripts.
  REAL*8 :: GpSI,GnSI,GpSD,GnSD
  
  ! Couplings that yield \sigma = 1 pb in each case.
  REAL*8 :: GpSI0,GnSI0,GpSD0,GnSD0
END TYPE

! Default (internal) WIMP structure
TYPE(WIMPStruct), PRIVATE :: WIMP


! HALO -----------------------------------------------------------------

! Parameters describing the dark matter halo.  Only the Standard Halo
! Model (SHM) can be used for the velocity distribution (i.e. Maxwell-
! Boltzmann distribution with a finite cutoff).

! Local dark matter halo density [GeV/cm^3]:
!   0.3 [standard (old)]
! * Catena & Ullio, JCAP 1008, 004 (2010) [arXiv:0907.0018]
!   For spherical halos, not including structure
!     Einasto profile: 0.385 +/- 0.027
!     NFW profile:     0.389 +/- 0.025
! * Weber & de Boer, arXiv:0910.4272
!     0.2 - 0.4 (depending on model)
! * Salucci et al., arXiv:1003.3101
!   Model independent technique?
!     0.430 +/- 0.113 (alpha) +/- 0.096 (r)
! * Pato et al., PRD 82, 023531 (2010) [arXiv:1006.1322]
!   Density at disk may be 1.01 - 1.41 times larger than shell
!   averaged quantity, so above measurements might be underestimates
!   of local density.
! DEFAULT: 0.4 GeV/cm^3

! Sun's peculiar velocity [km/s]:
! motion relative to local standard of rest (LSR)
! * Mignard, Astron. Astrophys. 354, 522 (2000)
! * Schoenrich, Binney & Dehnen, arXiv:0912.3693
! DEFAULT: (11,12,7) km/s

! Disk rotation velocity [km/s]:
! * Kerr & Lynden-Bell, MNRAS 221, 1023 (1986)
!     220 [standard]
!     222 +/- 20 (average over multiple measurements, could be biased
!                 by systematics)
! * Reid et al., Astrophys. J. 700, 137 (2009) [arXiv:0902.3913]
!   Estimate based on masers.
!     254 +/- 16
! * McMillan & Binney, MNRAS 402, 934 (2010) [arXiv:0907.4685]
!   Reanalysis of Reid et al. masers.  Range of estimates based on
!   various models; suggest Sun's velocity with respect to LSR should
!   be modified.
!     200 +/- 20 to 279 +/- 33
! * Bovy, Hogg & Rix, ApJ 704, 1704 (2009) [arXiv:0907.5423]
!     244 +/- 13 (masers only)
!     236 +/- 11 (combined estimate)
! DEFAULT: 235 km/s

! The Local Standard of Rest (LSR) [km/s], which we take to be
! (0,vrot,0).
! DEFAULT: (0,235,0) km/s

! Sun's velocity vector relative to the galactic rest frame [km/s],
! sum of LSR and peculiar velocity, where LSR = (0,vrot,0):
! DEFAULT: (0,235,0) + (11,12,7) km/s

! Sun's speed relative to the galactic rest frame [km/s].
! Equal to magnitude of Sun's velocity.
! DEFAULT: sqrt{11^2 + (235+12)^2 + 7^2} km/s

! Most probable speed (characterizing velocity dispersion) [km/s]:
! Typically similar to rotation velocity.
!     vrms = sqrt(3/2) v0    [rms velocity]
!     vmp  = v0              [most probably velocity]
!     vave = sqrt(4/pi) v0   [mean velocity]
! DEFAULT: 235 km/s

! Local escape velocity [km/s]:
!   650 [standard (old)]
! * Smith et al., MNRAS 379, 755 (2007) [astro-ph/0611671]
!   Note from Fig 7 that the distribution is asymmetric.  The following
!   results assume vrot = 220.
!     544 (mean), 498 - 608 (90% CL)
!     462 - 640 (90% CL when also fitting parameter k)
! DEFAULT: 550 km/s

! Structure containing halo parameters
TYPE, PRIVATE :: HaloStruct
  ! Galactic motions ---------------------------
  ! Local galactic disk rotation speed [km/s]
  REAL*8 :: vrot = 235d0
  ! Local standard of rest velocity vector [km/s], defined relative to
  ! galactic rest frame.
  REAL*8 :: vlsr(3) = (/ 0d0, 235d0, 0d0 /)
  ! Sun's peculiar velocity vector [km/s], defined relative to local
  ! standard of rest.
  REAL*8 :: vpec(3) = (/ 11d0, 12d0, 7d0 /)
  ! Sun's velocity vector [km/s], defined relative to galactic rest
  ! frame.
  REAL*8 :: vsun(3) = (/ 0d0, 235d0, 0d0 /) + (/ 11d0, 12d0, 7d0 /)
  ! Sun's speed (or observer's speed) [km/s], defined relative to
  ! galactic rest frame.
  REAL*8 :: vobs = SQRT(11d0**2 + (235d0+12d0)**2 + 7d0**2)
  
  ! Local DM density ---------------------------
  ! Local dark matter density [GeV/cm^3]
  REAL*8 :: rho = 0.4d0
  
  ! DM distribution (SHM) ----------------------
  ! Truncated Maxwell-Boltzmann ("MB") distribution.
  
  ! Bulk velocity of the dark matter [km/s] (i.e. the velocity of
  ! the MB rest frame), defined relative to the galactic rest frame.
  REAL*8 :: vbulk(3) = (/ 0d0, 0d0, 0d0 /)
  ! Most probable speed [km/s] in the MB rest frame.
  REAL*8 :: v0 = 235d0
  ! Escape speed [km/s] in the MB rest frame.
  REAL*8 :: vesc = 550d0
  
  ! DM distribution (tabulated) ----------------
  ! Instead of being calculated for SHM above, mean inverse speed
  ! can be given explicitly as a tabulation.
  LOGICAL :: tabulated = .FALSE.
  CHARACTER(LEN=1024) :: eta_file = ''
  INTEGER :: Nvmin = -1
  REAL*8, ALLOCATABLE :: vmin(:)
  REAL*8, ALLOCATABLE :: eta(:)
END TYPE

! Default (internal) WIMP structure
TYPE(HaloStruct), PRIVATE :: Halo


! DETECTOR EFFICIENCY --------------------------------------------------

! Structure to contain tabulated detection efficiencies as a
! function of energy, for the overall analysis range and possibly
! for subintervals/bins.
! 
TYPE, PUBLIC :: EfficiencyStruct
  
  ! File containing efficiencies
  CHARACTER(LEN=1024) :: file = ''
  
  ! Number of tabulation points (energies).
  INTEGER :: NE = -1
  
  ! Tabulated energies [keV].  Array of size [1:NE].
  REAL*8, ALLOCATABLE :: E(:)
  
  ! Number of S1 bins/intervals with efficiencies.
  ! Will calculate rates for each bin/interval plus total.
  INTEGER :: Neff = -1
  
  ! Array of size [1:NE,0:Neff] with the second index for the S1
  ! bin/interval (zero for full range)
  REAL*8, ALLOCATABLE :: eff(:,:)
  
END TYPE


! DETECTOR RATES -------------------------------------------------------

! Structure to contain tabulated rates as a function of energy.
! 
TYPE, PUBLIC :: DetectorRateStruct
  
  ! Label --------------------------------------
  ! Label for the experimental result contained in this structure.
  ! Must be at most 12 characters as it will be used in column headers.
  CHARACTER(LEN=12) :: label = ''
  ! More detailed description.
  CHARACTER(LEN=1024) :: description = ''
  
  ! Exposure -----------------------------------
  ! Detector fiducial mass [kg]
  REAL*8 :: mass = 118d0
  
  ! Detector exposure time [day]
  REAL*8 :: time = 85.3d0
  
  ! Total detector exposure [kg*day]
  REAL*8 :: exposure = 118d0*85.3d0
  
  ! Events -------------------------------------
  ! Observed number of events
  INTEGER :: Nevents = -1
  
  ! Average expected background events
  REAL*8 :: MuBackground = 0d0
  
  ! Isotopes -----------------------------------
  ! Number of isotopes
  INTEGER :: Niso = -1
  
  ! Detector isotopes, their mass fractions, and nuclear masses [GeV]
  INTEGER, ALLOCATABLE :: Ziso(:)
  INTEGER, ALLOCATABLE :: Aiso(:)
  REAL*8, ALLOCATABLE  :: fiso(:)
  REAL*8, ALLOCATABLE  :: Miso(:)  ! Calculated internally
  
  ! Tabulation ---------------------------------
  ! Number of tabulation points (energies).
  ! NOTE: This tabulation is fixed to that used by the efficiency data.
  INTEGER :: NE = -1
  
  ! Tabulated energies [keV].  Array of size [1:NE].
  REAL*8, ALLOCATABLE :: E(:)
  
  ! Efficiencies -------------------------------
  ! Tabulated detection efficiencies.
  ! File containing efficiencies
  CHARACTER(LEN=1024) :: eff_file = ''
  
  ! Number of S1 bins/intervals with efficiencies.
  ! Will calculate rates for each bin/interval plus total.
  INTEGER :: Neff = -1
  
  ! Array of size [1:NE,0:Neff] with the second index for the S1
  ! bin/interval (zero for full range)
  REAL*8, ALLOCATABLE :: eff(:,:)
  
  ! Form factors -------------------------------
  ! Tabulated spin-independent or spin-dependent form factors combined
  ! with prefactors.  Arrays of size [-1:1,1:NE,1:Niso].  Defined as
  ! [unitless]:
  !   Wsi(+1,:,:) = (1/pi) Z^2 F^2(q)        ! SI proton
  !   Wsi( 0,:,:) = (1/pi) 2*Z*(A-Z) F^2(q)  ! SI crossterm
  !   Wsi(-1,:,:) = (1/pi) (A-Z)^2 F^2(q)    ! SI neutron
  !   Wsd(+1,:,:) = 4/(2J+1) Spp(q)          ! SD proton
  !   Wsd( 0,:,:) = 4/(2J+1) Spn(q)          ! SD crossterm
  !   Wsd(-1,:,:) = 4/(2J+1) Snn(q)          ! SD neutron
  ! The above definitions give for the conventional SI and SD
  ! cross-sections:
  !   \sigma(q) = \mu^2 (hbar c)^2 [W(1)*Gp^2 + W(0)*Gp*Gn + W(-1)*Gn^2]
  ! where Gp and Gn are the effective proton and neutron couplings
  ! in units of [GeV^-2] and \mu is the reduced mass.  In terms of
  ! more commonly used notation:
  !   SI (scalar):        G = 2f
  !   SD (axial-vector):  G = 2\sqrt{2} G_F a
  ! where G, f, and a have 'p' and 'n' subscripts.  While form factors
  ! are often a function of the momentum transfer, we tabulate them
  ! here as a function of recoil energy E = q^2/2M.
  ! NOTE: Need only be calculated once.
  REAL*8, ALLOCATABLE :: Wsi(:,:,:),Wsd(:,:,:)
  
  ! Halo ---------------------------------------
  ! The minimum velocity for producing a recoil of energy E, given
  ! by vmin = sqrt{M E/(2\mu^2)} [km/s].
  ! Array of size [1:NE,1:Niso] that needs to be recalculated when the
  ! WIMP mass changes.
  REAL*8, ALLOCATABLE :: vmin(:,:)
  
  ! Tabulated mean inverse speed (eta) [s/km] at the above vmin.
  REAL*8, ALLOCATABLE :: eta(:,:)
  
  ! Differential rates (reference couplings) ---
  ! Reference differential rates calculated at \sigma = 1 pb for each
  ! coupling and tabulated by energy.  Differential arrays are of size
  ! [-1:1,1:NE] and are given separately for SI and SD couplings.  The
  ! first index is for the proton & neutron components at sigma = 1 pb
  ! in each case and the second index is that of the energy (given by
  ! the E array at the same index).  These represent raw rates prior to
  ! any efficiency cuts. [cpd/kg/keV]
  REAL*8, ALLOCATABLE :: dRdEsi0(:,:),dRdEsd0(:,:)
  
  ! Integrated rates (reference couplings) -----
  ! Reference integrated rates calculated at \sigma = 1 pb for each
  ! coupling, with the efficiency-weighted integral done separately
  ! for each available efficiency curve.  Arrays are of size
  ! [-1:1,0:Neff].  The first index is for the proton & neutron
  ! components at sigma = 1 pb in each case and the second index is for
  ! the S1 bin/interval (0 for full range). [cpd/kg]
  REAL*8, ALLOCATABLE :: Rsi0(:,:),Rsd0(:,:)
  
  ! Differential rates (actual couplings) ------
  ! Differential rates at given couplings, tabulated by energy.
  ! Arrays are of size [1:NE], given separately for SI and SD couplings
  ! as well as the SI+SD total. [cpd/kg/keV]
  REAL*8, ALLOCATABLE :: dRdEsi(:),dRdEsd(:),dRdE(:)
  
  ! Integrated rates (actual couplings) --------
  ! Efficiency-corrected rates at given couplings.  Arrays are of size
  ! [0:Neff] with the index being that of the S1 bin/interval
  ! efficiency curve used in the integral (0 for full range).  Given
  ! separately for SI and SD components as well as the SI+SD total.
  ! [cpd/kg]
  REAL*8, ALLOCATABLE :: Rsi(:),Rsd(:),R(:)
  
  ! Events -------------------------------------
  ! Expected number of signal events at reference couplings.
  ! Arrays of size [-1:1,0:Neff].
  REAL*8, ALLOCATABLE :: MuSignalSI0(:,:),MuSignalSD0(:,:)
  ! Expected number of signal events.  Arrays of size [0:Neff].
  REAL*8, ALLOCATABLE :: MuSignalSI(:),MuSignalSD(:),MuSignal(:)
  
END TYPE

! Default (internal) detector rate structure.
TYPE(DetectorRateStruct), TARGET, PRIVATE :: DefaultDetector

! Structures for specific experiments
! NOTE: These will be initialized to internally defined states and
! are not externally modifiable.
TYPE(DetectorRateStruct), PRIVATE :: LUX_2013
TYPE(DetectorRateStruct), PRIVATE :: DARWIN_Ar_2014,DARWIN_Xe_2014


! TABULATION -----------------------------------------------------------

! Structure to contain fixed spacing linear or logarithmic tabulation
! parametrization.
! 
TYPE, PUBLIC :: TabulationStruct
  ! Linear or logarithmic spacing?
  LOGICAL :: logarithmic = .FALSE.
  ! Number of intervals between tabulation points
  INTEGER :: N
  ! Range of tabulation
  REAL*8 :: xmin,xmax
  ! Logarithm of tabulation range values
  REAL*8 :: lnxmin,lnxmax
  ! Spacing between tabulation points:
  !   linear:       delta = x_{k+1} - x_k
  !   logarithmic:  delta = ln(x_{k+1}) - ln(x_k) = ln(x_{k+1}/x_k)
  REAL*8 :: delta
END TYPE


! FORMATTING -----------------------------------------------------------

! Prefix to place at beginning of comment and data lines
CHARACTER*(*), PARAMETER :: COMMENT_PREFIX = '# '
CHARACTER*(*), PARAMETER :: DATA_PREFIX    = '  '
CHARACTER*(*), PARAMETER :: COMMENT_LINE   = &
    '# ----------------------------------'   &
    // '------------------------------------'


! RANDOM NUMBER GENERATOR STATE ----------------------------------------

! The structure here stores state data for the uniform random number
! generator based on the algorithm of Marsaglia & Zaman (and later
! Tsang).  See the 'RANDOM NUMBER GENERATOR' section below for
! references.
! 
TYPE, PRIVATE :: RandState
  LOGICAL :: initialized = .FALSE.
  INTEGER :: I,J      ! I97,J97
  REAL*8 ::  U(1:97)
  REAL*8 ::  C,CD,CM
END TYPE

! Instantiation for default
! OpenMP directive ensures each thread maintains own copy
TYPE(RandState), PRIVATE :: DEFAULT_RandState
!$OMP THREADPRIVATE(DEFAULT_RandState)



!#######################################################################
! INTERFACES
!#######################################################################

! Here define interfaces, which allow for the following:
!  *) Provide a new name for accessing a routine.
!  *) Allow private routines to be externally accessed
!     under the new name (if interface is public).
!  *) Allow for multiple routines to be accessed in the same
!     name, as long as signatures are not ambiguous.

! Convert E to vmin given the scalar or vector arguments.
! Access all versions under function name 'EToVmin'.
INTERFACE EToVmin
  MODULE PROCEDURE EToVmin0,EToVmin1,EToVmin2
END INTERFACE

! Calculate mean inverse speed from vmin given the scalar or vector
! arguments. Access all versions under function name 'MeanInverseSpeed'.
INTERFACE MeanInverseSpeed
  MODULE PROCEDURE MeanInverseSpeed0,MeanInverseSpeed1,MeanInverseSpeed2
END INTERFACE



!#######################################################################
! ROUTINES
!#######################################################################

CONTAINS


!=======================================================================
! SIMPLE INTERFACE ROUTINES
! Basic versions of routines that are required for using this module
! externally.  These are meant to allow for a simpler interface to
! this module; other routines are more robust and provide more
! capabilities.
!=======================================================================

!-----------------------------------------------------------------------
! Initializes the module.  Must be run once before use of any
! module routines.
! 
! For more detailed initialization, see:
!   Initialize() [interface name: DDCalc0_Initialize]
! 
SUBROUTINE DDCalc0_Init()
  IMPLICIT NONE
  CALL Initialize(.TRUE.)
END SUBROUTINE


!-----------------------------------------------------------------------
! Sets the dark matter halo to the Standard Halo Model (SHM) with the
! given parameters.  Need only be run if non-default parameters are
! to be used.
! 
! For more detailed halo settings, see:
!   SetHalo() [interface name: DDCalc0_SetHalo]
! 
! Input arguments:
!   rho         Local dark matter density [GeV/cm^3].  Default is
!               0.4 GeV/cm^3.
!   vrot        Local galactic disk rotation speed [km/s].  Default is
!               235 km/s.
!   v0          Most probable speed [km/s] in the galactic rest frame.
!               For the conventional isothermal sphere, this should be
!               the same as vrot.  Default is 235 km/s.
!   vesc        Galactic escape speed [km/s] in the galactic rest
!               frame.  Default is 550 km/s.
! 
SUBROUTINE DDCalc0_SetSHM(rho,vrot,v0,vesc)
  IMPLICIT NONE
  REAL*8, INTENT(IN) :: rho,vrot,v0,vesc
  CALL SetHalo(rho=rho,vrot=vrot,v0=v0,vesc=vesc)
END SUBROUTINE


!-----------------------------------------------------------------------
! Sets the WIMP mass and couplings.  Couplings are specified via
! the commonly used fp/fn (spin-independent) and ap/an (spin-dependent)
! normalizations.
! 
! For more detailed WIMP settings, see:
!   SetWIMP() [interface name: DDCalc0_SetWIMP]
! 
! Input arguments:
!   m           WIMP mass [GeV].
!   fp          Spin-independent WIMP-proton coupling [GeV^-2].
!               Related by GpSI = 2 fp.
!   fn          Spin-independent WIMP-neutron coupling [GeV^-2].
!               Related by GnSI = 2 fn.
!   ap          Spin-dependent WIMP-proton coupling [unitless].
!               Related by GpSD = 2\sqrt{2} G_F ap.
!   an          Spin-dependent WIMP-neutron coupling [unitless].
!               Related by GnSD = 2\sqrt{2} G_F an.
! 
SUBROUTINE DDCalc0_SetWIMP_mfa(m,fp,fn,ap,an)
  IMPLICIT NONE
  REAL*8, INTENT(IN) :: m,fp,fn,ap,an
  CALL SetWIMP(m=m,fp=fp,fn=fn,ap=ap,an=an)
END SUBROUTINE


!-----------------------------------------------------------------------
! Sets the WIMP mass and couplings.  Couplings are specified via
! their effective 4-fermion vertex couplings 'G'.
! 
! For more detailed WIMP settings, see:
!   SetWIMP() [interface name: DDCalc0_SetWIMP]
! 
! Input arguments:
!   m           WIMP mass [GeV].
!   GpSI        Spin-independent WIMP-proton coupling [GeV^-2].
!               Related by GpSI = 2 fp.
!   GnSI        Spin-independent WIMP-neutron coupling [GeV^-2].
!               Related by GnSI = 2 fn.
!   GpSD        Spin-dependent WIMP-proton coupling [GeV^-2].
!               Related by GpSD = 2\sqrt{2} G_F ap.
!   GnSD        Spin-dependent WIMP-neutron coupling [GeV^-2].
!               Related by GnSD = 2\sqrt{2} G_F an.
! 
SUBROUTINE DDCalc0_SetWIMP_mG(m,GpSI,GnSI,GpSD,GnSD)
  IMPLICIT NONE
  REAL*8, INTENT(IN) :: m,GpSI,GnSI,GpSD,GnSD
  CALL SetWIMP(m=m,GpSI=GpSI,GnSI=GnSI,GpSD=GpSD,GnSD=GnSD)
END SUBROUTINE


!-----------------------------------------------------------------------
! Sets the WIMP mass and couplings.  Couplings are specified via
! their effective 4-fermion vertex couplings 'G'.
! 
! For more detailed WIMP settings, see:
!   SetWIMP() [interface name: DDCalc0_SetWIMP].
! 
! Input arguments, give negative vale to cross-sections inputs to
! set the corresponding coupling negative:
!   m           WIMP mass [GeV].
!   sigmapSI    Spin-independent WIMP-proton cross-section [pb].
!   sigmanSI    Spin-independent WIMP-neutron cross-section [pb].
!   sigmapSD    Spin-dependent WIMP-proton cross-section [pb].
!   sigmanSD    Spin-dependent WIMP-neutron cross-section [pb].
! 
SUBROUTINE DDCalc0_SetWIMP_msigma(m,sigmapSI,sigmanSI,sigmapSD,sigmanSD)
  IMPLICIT NONE
  REAL*8, INTENT(IN) :: m,sigmapSI,sigmanSI,sigmapSD,sigmanSD
  CALL SetWIMP(m=m,sigmapSI=sigmapSI,sigmanSI=sigmanSI,                 &
               sigmapSD=sigmapSD,sigmanSD=sigmanSD)
END SUBROUTINE



!=======================================================================
! LUX 2013 ANALYSIS ROUTINES
! Based upon the LUX 2013 analysis [1310.8214].  One event seen in
! the analysis region.
!=======================================================================

!-----------------------------------------------------------------------
! Initializes the module to perform calculations for the LUX 2013
! analysis.  This must be called if any of the following LUX 2013
! routines are to be used.
! 
! Required input arguments:
!     intervals   Indicates if sub-intervals should be included.
!                 Only necessary if confidence intervals using the
!                 maximum gap method are desired.
! 
SUBROUTINE LUX_2013_Init(intervals)
  IMPLICIT NONE
  LOGICAL, INTENT(IN) :: intervals
  
  CALL LUX_2013_InitTo(LUX_2013,intervals)
  
END SUBROUTINE


! ----------------------------------------------------------------------
! Calculates various rate quantities using the current WIMP.
! Must be called each time the WIMP parameters are modified.
! 
SUBROUTINE LUX_2013_CalcRates()
  IMPLICIT NONE
  CALL CalcRates(LUX_2013)
END SUBROUTINE


! ----------------------------------------------------------------------
! Returns the observed number of events.
! 
FUNCTION LUX_2013_Events() RESULT(N)
  IMPLICIT NONE
  INTEGER :: N
  CALL GetRates(LUX_2013,Nevents=N)
END FUNCTION


! ----------------------------------------------------------------------
! Returns the average expected number of background events.
! 
FUNCTION LUX_2013_Background() RESULT(b)
  IMPLICIT NONE
  REAL*8 :: b
  CALL GetRates(LUX_2013,background=b)
END FUNCTION


! ----------------------------------------------------------------------
! Returns the average expected number of signal events for the
! current WIMP.
! 
FUNCTION LUX_2013_Signal() RESULT(s)
  IMPLICIT NONE
  REAL*8 :: s
  CALL GetRates(LUX_2013,signal=s)
END FUNCTION


! ----------------------------------------------------------------------
! Returns the average expected number of spin-independent signal events
! for the current WIMP.
! 
FUNCTION LUX_2013_SignalSI() RESULT(s)
  IMPLICIT NONE
  REAL*8 :: s
  CALL GetRates(LUX_2013,signal_si=s)
END FUNCTION


! ----------------------------------------------------------------------
! Returns the average expected number of spin-dependent signal events
! for the current WIMP.
! 
FUNCTION LUX_2013_SignalSD() RESULT(s)
  IMPLICIT NONE
  REAL*8 :: s
  CALL GetRates(LUX_2013,signal_sd=s)
END FUNCTION


! ----------------------------------------------------------------------
! Calculates the log-likelihood for the current WIMP mass and couplings.
! Uses a Poisson distribution in the number of observed events N:
!    P(N|s+b)
! where s is the average expected signal and b is the average expected
! background.
! 
FUNCTION LUX_2013_LogLikelihood() RESULT(lnlike)
  IMPLICIT NONE
  REAL*8 :: lnlike
  lnlike = LogLikelihood(LUX_2013)
END FUNCTION


! ----------------------------------------------------------------------
! Calculates the log of the p-value for the current WIMP mass and
! couplings (NO BACKGROUND SUBTRACTION).  Uses the maximum gap method
! if LUX_2013_Init was called with argument intervals=.TRUE.,
! otherwise uses a Poisson distribution in the number of observed
! events N:
!    P(N|s)
! where s is the average expected signal (background contributions are
! ignored).
! 
FUNCTION LUX_2013_LogPValue() RESULT(lnp)
  IMPLICIT NONE
  REAL*8 :: lnp
  lnp = LogPValue(LUX_2013)
END FUNCTION


! ----------------------------------------------------------------------
! Calculates the factor x by which the cross-sections must be scaled
! (sigma -> x*sigma) to achieve the desired p-value (given as log(p)).
! See LogPValue() above for a description of the statistics.
! 
! Required input argument:
!   lnp         The logarithm of the desired p-value (p = 1-CL).
! 
FUNCTION LUX_2013_ScaleToPValue(lnp) RESULT(x)
  IMPLICIT NONE
  REAL*8 :: x
  REAL*8, INTENT(IN) :: lnp
  x = ScaleToPValue(LUX_2013,lnp)
END FUNCTION


!-----------------------------------------------------------------------
! INTERNAL ROUTINE.
! Initializes the given DetectorRateStruct to the LUX 2013 analysis.
! This is meant as an internal routine; external access should be
! through LUX_2013_InitDetector instead.
! 
! The efficiencies used here were generated using TPCMC.
! 
! Required input arguments:
!     D           The DetectorRateStruct to initialize
!     intervals   Indicates if sub-intervals should be included
! 
SUBROUTINE LUX_2013_InitTo(D,intervals)
  IMPLICIT NONE
  TYPE(DetectorRateStruct), INTENT(OUT) :: D
  LOGICAL, INTENT(IN) :: intervals
  INTEGER, PARAMETER :: NE = 151
  INTEGER, PARAMETER :: NEFF = 2
  ! Efficiency curves energy tabulation points
  REAL*8, PARAMETER :: E(NE)                                            &
      =       (/ 0.10000d0, 0.10471d0, 0.10965d0, 0.11482d0, 0.12023d0, &
      0.12589d0, 0.13183d0, 0.13804d0, 0.14454d0, 0.15136d0, 0.15849d0, &
      0.16596d0, 0.17378d0, 0.18197d0, 0.19055d0, 0.19953d0, 0.20893d0, &
      0.21878d0, 0.22909d0, 0.23988d0, 0.25119d0, 0.26303d0, 0.27542d0, &
      0.28840d0, 0.30200d0, 0.31623d0, 0.33113d0, 0.34674d0, 0.36308d0, &
      0.38019d0, 0.39811d0, 0.41687d0, 0.43652d0, 0.45709d0, 0.47863d0, &
      0.50119d0, 0.52481d0, 0.54954d0, 0.57544d0, 0.60256d0, 0.63096d0, &
      0.66069d0, 0.69183d0, 0.72444d0, 0.75858d0, 0.79433d0, 0.83176d0, &
      0.87096d0, 0.91201d0, 0.95499d0, 1.0000d0,  1.0471d0,  1.0965d0,  &
      1.1482d0,  1.2023d0,  1.2589d0,  1.3183d0,  1.3804d0,  1.4454d0,  &
      1.5136d0,  1.5849d0,  1.6596d0,  1.7378d0,  1.8197d0,  1.9055d0,  &
      1.9953d0,  2.0893d0,  2.1878d0,  2.2909d0,  2.3988d0,  2.5119d0,  &
      2.6303d0,  2.7542d0,  2.8840d0,  3.0200d0,  3.1623d0,  3.3113d0,  &
      3.4674d0,  3.6308d0,  3.8019d0,  3.9811d0,  4.1687d0,  4.3652d0,  &
      4.5709d0,  4.7863d0,  5.0119d0,  5.2481d0,  5.4954d0,  5.7544d0,  &
      6.0256d0,  6.3096d0,  6.6069d0,  6.9183d0,  7.2444d0,  7.5858d0,  &
      7.9433d0,  8.3176d0,  8.7096d0,  9.1201d0,  9.5499d0, 10.000d0,   &
     10.471d0,  10.965d0,  11.482d0,  12.023d0,  12.589d0,  13.183d0,   &
     13.804d0,  14.454d0,  15.136d0,  15.849d0,  16.596d0,  17.378d0,   &
     18.197d0,  19.055d0,  19.953d0,  20.893d0,  21.878d0,  22.909d0,   &
     23.988d0,  25.119d0,  26.303d0,  27.542d0,  28.840d0,  30.200d0,   &
     31.623d0,  33.113d0,  34.674d0,  36.308d0,  38.019d0,  39.811d0,   &
     41.687d0,  43.652d0,  45.709d0,  47.863d0,  50.119d0,  52.481d0,   &
     54.954d0,  57.544d0,  60.256d0,  63.096d0,  66.069d0,  69.183d0,   &
     72.444d0,  75.858d0,  79.433d0,  83.176d0,  87.096d0,  91.201d0,   &
     95.499d0,100.00d0 /)
  ! Efficiency (total)
  REAL*8, PARAMETER :: EFF0(NE)                                         &
      =       (/ 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 2.00000d-7,1.00000d-7,2.00000d-7,5.00000d-7,5.00000d-7,&
      7.00000d-7,7.00000d-7,1.20000d-6,1.80000d-6,2.30000d-6,3.90000d-6,&
      5.40000d-6,8.50000d-6,1.23000d-5,1.70000d-5,2.38000d-5,3.31000d-5,&
      4.61000d-5,5.94000d-5,9.26000d-5,1.21600d-4,1.72500d-4,2.34500d-4,&
      3.33300d-4,4.56700d-4,6.38400d-4,8.84300d-4,1.33540d-3,1.84720d-3,&
      2.51600d-3,3.31170d-3,4.43180d-3,5.93570d-3,7.78490d-3,1.02271d-2,&
      1.33162d-2,1.72063d-2,2.22009d-2,2.73286d-2,3.45888d-2,4.36821d-2,&
      5.45799d-2,6.77302d-2,8.32638d-2,1.00680d-1,1.21170d-1,1.43630d-1,&
      1.68140d-1,1.94250d-1,2.21300d-1,2.48970d-1,2.76390d-1,3.02480d-1,&
      3.26300d-1,3.48160d-1,3.66100d-1,3.79720d-1,3.89880d-1,3.95620d-1,&
      3.97780d-1,3.96640d-1,3.92580d-1,3.87090d-1,3.79880d-1,3.73050d-1,&
      3.65850d-1,3.58740d-1,3.53390d-1,3.48230d-1,3.43970d-1,3.41220d-1,&
      3.38810d-1,3.37650d-1,3.36940d-1,3.36130d-1,3.35760d-1,3.34690d-1,&
      3.33280d-1,3.32160d-1,3.31870d-1,3.32150d-1,3.32200d-1,3.31440d-1,&
      3.28070d-1,3.20960d-1,3.07340d-1,2.84620d-1,2.50940d-1,2.07520d-1,&
      1.58600d-1,1.10370d-1,6.96978d-2,3.92375d-2,1.98434d-2,8.79110d-3,&
      3.44320d-3,1.18140d-3,3.65300d-4,9.83000d-5,2.27000d-5,5.00000d-6,&
      3.00000d-7,0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0 /)
  ! Efficiency (first interval)
  REAL*8, PARAMETER :: EFF1(NE)                                         &
      =       (/ 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 2.00000d-7,1.00000d-7,2.00000d-7,5.00000d-7,5.00000d-7,&
      6.00000d-7,7.00000d-7,1.20000d-6,1.80000d-6,2.30000d-6,3.80000d-6,&
      5.20000d-6,7.90000d-6,1.18000d-5,1.66000d-5,2.28000d-5,3.16000d-5,&
      4.33000d-5,5.57000d-5,8.52000d-5,1.12700d-4,1.59900d-4,2.15100d-4,&
      3.04800d-4,4.11400d-4,5.74500d-4,7.88300d-4,1.16420d-3,1.59150d-3,&
      2.16230d-3,2.78910d-3,3.71100d-3,4.89860d-3,6.29290d-3,8.15860d-3,&
      1.03830d-2,1.31709d-2,1.66408d-2,2.02664d-2,2.50647d-2,3.07883d-2,&
      3.72727d-2,4.46930d-2,5.28245d-2,6.11301d-2,7.03714d-2,7.93184d-2,&
      8.75027d-2,9.49859d-2,1.01120d-1,1.05340d-1,1.07160d-1,1.06570d-1,&
      1.03200d-1,9.76218d-2,8.97488d-2,7.98012d-2,6.90608d-2,5.76738d-2,&
      4.66902d-2,3.63217d-2,2.71772d-2,1.96357d-2,1.34951d-2,8.86060d-3,&
      5.57630d-3,3.35120d-3,1.90010d-3,1.02690d-3,5.29500d-4,2.54600d-4,&
      1.14600d-4,4.86000d-5,1.93000d-5,6.70000d-6,2.90000d-6,1.40000d-6,&
      0.00000d0, 1.00000d-7,0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0 /)
  ! Efficiency (second interval)
  REAL*8, PARAMETER :: EFF2(NE)                                         &
      =       (/ 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      1.00000d-7,0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 1.00000d-7,&
      2.00000d-7,6.00000d-7,5.00000d-7,4.00000d-7,1.00000d-6,1.50000d-6,&
      2.80000d-6,3.70000d-6,7.40000d-6,8.90000d-6,1.26000d-5,1.94000d-5,&
      2.85000d-5,4.53000d-5,6.39000d-5,9.60000d-5,1.71200d-4,2.55700d-4,&
      3.53700d-4,5.22600d-4,7.20800d-4,1.03710d-3,1.49200d-3,2.06850d-3,&
      2.93320d-3,4.03540d-3,5.56010d-3,7.06220d-3,9.52410d-3,1.28938d-2,&
      1.73072d-2,2.30372d-2,3.04393d-2,3.95450d-2,5.07938d-2,6.43164d-2,&
      8.06401d-2,9.92666d-2,1.20180d-1,1.43630d-1,1.69230d-1,1.95910d-1,&
      2.23100d-1,2.50540d-1,2.76350d-1,2.99920d-1,3.20820d-1,3.37950d-1,&
      3.51090d-1,3.60320d-1,3.65400d-1,3.67460d-1,3.66390d-1,3.64190d-1,&
      3.60270d-1,3.55390d-1,3.51490d-1,3.47210d-1,3.43440d-1,3.40970d-1,&
      3.38690d-1,3.37600d-1,3.36920d-1,3.36130d-1,3.35750d-1,3.34690d-1,&
      3.33280d-1,3.32160d-1,3.31870d-1,3.32150d-1,3.32200d-1,3.31440d-1,&
      3.28070d-1,3.20960d-1,3.07340d-1,2.84620d-1,2.50940d-1,2.07520d-1,&
      1.58600d-1,1.10370d-1,6.96978d-2,3.92375d-2,1.98434d-2,8.79110d-3,&
      3.44320d-3,1.18140d-3,3.65300d-4,9.83000d-5,2.27000d-5,5.00000d-6,&
      3.00000d-7,0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0 /)
  ! Efficiencies array (2D)
  REAL*8, PARAMETER :: EFF(NE,0:NEFF)                                   &
      = RESHAPE( (/ EFF0(:), EFF1(:), EFF2(:) /) ,SHAPE(EFF))
  
  ! One call for all settings.
  ! Most of these _must_ be there to ensure everything get initialized.
  IF (intervals) THEN
    CALL SetDetector(D,mass=118d0,time=85.3d0,Nevents=1,                &
                     background=0.64d0,Zelem=54,                        &
                     NE=NE,E=E,Neff=NEFF,eff=EFF)
  ELSE
    CALL SetDetector(D,mass=118d0,time=85.3d0,Nevents=1,                &
                     background=0.64d0,Zelem=54,                        &
                     NE=NE,E=E,Neff=0,eff=EFF(:,0:0))
  END IF
  D%eff_file = '[LUX 2013]'
  
END SUBROUTINE



!=======================================================================
! DARWIN ARGON ANALYSIS ROUTINES
! Based upon a DARWIN argon-based analysis (2014 estimated parameters)
! [14MM.XXXX].  Zero events assumed in the analysis region.
!=======================================================================

!-----------------------------------------------------------------------
! Initializes the module to perform calculations for a DARWIN
! argon-based analysis (2014 estimates).  This must be called if any
! of the following DARWIN routines are to be used.
! 
! Required input arguments:
!     intervals   Indicates if sub-intervals should be included.
!                 Only necessary if confidence intervals using the
!                 maximum gap method are desired.
! 
SUBROUTINE DARWIN_Ar_2014_Init(intervals)
  IMPLICIT NONE
  LOGICAL, INTENT(IN) :: intervals
  
  CALL DARWIN_Ar_2014_InitTo(DARWIN_Ar_2014,intervals)
  
END SUBROUTINE


! ----------------------------------------------------------------------
! Calculates various rate quantities using the current WIMP.
! Must be called each time the WIMP parameters are modified.
! 
SUBROUTINE DARWIN_Ar_2014_CalcRates()
  IMPLICIT NONE
  CALL CalcRates(DARWIN_Ar_2014)
END SUBROUTINE


! ----------------------------------------------------------------------
! Returns the observed number of events.
! 
FUNCTION DARWIN_Ar_2014_Events() RESULT(N)
  IMPLICIT NONE
  INTEGER :: N
  CALL GetRates(DARWIN_Ar_2014,Nevents=N)
END FUNCTION


! ----------------------------------------------------------------------
! Returns the average expected number of background events.
! 
FUNCTION DARWIN_Ar_2014_Background() RESULT(b)
  IMPLICIT NONE
  REAL*8 :: b
  CALL GetRates(DARWIN_Ar_2014,background=b)
END FUNCTION


! ----------------------------------------------------------------------
! Returns the average expected number of signal events for the
! current WIMP.
! 
FUNCTION DARWIN_Ar_2014_Signal() RESULT(s)
  IMPLICIT NONE
  REAL*8 :: s
  CALL GetRates(DARWIN_Ar_2014,signal=s)
END FUNCTION


! ----------------------------------------------------------------------
! Returns the average expected number of spin-independent signal events
! for the current WIMP.
! 
FUNCTION DARWIN_Ar_2014_SignalSI() RESULT(s)
  IMPLICIT NONE
  REAL*8 :: s
  CALL GetRates(DARWIN_Ar_2014,signal_si=s)
END FUNCTION


! ----------------------------------------------------------------------
! Returns the average expected number of spin-dependent signal events
! for the current WIMP.
! 
FUNCTION DARWIN_Ar_2014_SignalSD() RESULT(s)
  IMPLICIT NONE
  REAL*8 :: s
  CALL GetRates(DARWIN_Ar_2014,signal_sd=s)
END FUNCTION


! ----------------------------------------------------------------------
! Calculates the log-likelihood for the current WIMP mass and couplings.
! Uses a Poisson distribution in the number of observed events N:
!    P(N|s+b)
! where s is the average expected signal and b is the average expected
! background.
! 
FUNCTION DARWIN_Ar_2014_LogLikelihood() RESULT(lnlike)
  IMPLICIT NONE
  REAL*8 :: lnlike
  lnlike = LogLikelihood(DARWIN_Ar_2014)
END FUNCTION


! ----------------------------------------------------------------------
! Calculates the log of the p-value for the current WIMP mass and
! couplings (NO BACKGROUND SUBTRACTION).  Uses the maximum gap method
! if DARWIN_Ar_2014_Init was called with argument intervals=.TRUE.,
! otherwise uses a Poisson distribution in the number of observed
! events N:
!    P(N|s)
! where s is the average expected signal (background contributions are
! ignored).
! 
FUNCTION DARWIN_Ar_2014_LogPValue() RESULT(lnp)
  IMPLICIT NONE
  REAL*8 :: lnp
  lnp = LogPValue(DARWIN_Ar_2014)
END FUNCTION


! ----------------------------------------------------------------------
! Calculates the factor x by which the cross-sections must be scaled
! (sigma -> x*sigma) to achieve the desired p-value (given as log(p)).
! See LogPValue() above for a description of the statistics.
! 
! Required input argument:
!   lnp         The logarithm of the desired p-value (p = 1-CL).
! 
FUNCTION DARWIN_Ar_2014_ScaleToPValue(lnp) RESULT(x)
  IMPLICIT NONE
  REAL*8 :: x
  REAL*8, INTENT(IN) :: lnp
  x = ScaleToPValue(DARWIN_Ar_2014,lnp)
END FUNCTION


!-----------------------------------------------------------------------
! INTERNAL ROUTINE.
! Initializes the given DetectorRateStruct to the proposed DARWIN
! argon-based detector (2014 estimate).  This is meant as an internal
! routine; external access should be through
! DARWIN_Ar_2014_InitDetector instead.
! 
! The efficiencies used here were generated using TPCMC.
! 
! Required input arguments:
!     D           The DetectorRateStruct to initialize
!     intervals   Indicates if sub-intervals should be included
! 
SUBROUTINE DARWIN_Ar_2014_InitTo(D,intervals)
  IMPLICIT NONE
  TYPE(DetectorRateStruct), INTENT(OUT) :: D
  LOGICAL, INTENT(IN) :: intervals
  INTEGER, PARAMETER :: NE = 151
  INTEGER, PARAMETER :: NEFF = 1
  ! Efficiency curves energy tabulation points
  REAL*8, PARAMETER :: E(NE)                                            &
      =       (/ 0.10000d0, 0.10471d0, 0.10965d0, 0.11482d0, 0.12023d0, &
      0.12589d0, 0.13183d0, 0.13804d0, 0.14454d0, 0.15136d0, 0.15849d0, &
      0.16596d0, 0.17378d0, 0.18197d0, 0.19055d0, 0.19953d0, 0.20893d0, &
      0.21878d0, 0.22909d0, 0.23988d0, 0.25119d0, 0.26303d0, 0.27542d0, &
      0.28840d0, 0.30200d0, 0.31623d0, 0.33113d0, 0.34674d0, 0.36308d0, &
      0.38019d0, 0.39811d0, 0.41687d0, 0.43652d0, 0.45709d0, 0.47863d0, &
      0.50119d0, 0.52481d0, 0.54954d0, 0.57544d0, 0.60256d0, 0.63096d0, &
      0.66069d0, 0.69183d0, 0.72444d0, 0.75858d0, 0.79433d0, 0.83176d0, &
      0.87096d0, 0.91201d0, 0.95499d0, 1.0000d0,  1.0471d0,  1.0965d0,  &
      1.1482d0,  1.2023d0,  1.2589d0,  1.3183d0,  1.3804d0,  1.4454d0,  &
      1.5136d0,  1.5849d0,  1.6596d0,  1.7378d0,  1.8197d0,  1.9055d0,  &
      1.9953d0,  2.0893d0,  2.1878d0,  2.2909d0,  2.3988d0,  2.5119d0,  &
      2.6303d0,  2.7542d0,  2.8840d0,  3.0200d0,  3.1623d0,  3.3113d0,  &
      3.4674d0,  3.6308d0,  3.8019d0,  3.9811d0,  4.1687d0,  4.3652d0,  &
      4.5709d0,  4.7863d0,  5.0119d0,  5.2481d0,  5.4954d0,  5.7544d0,  &
      6.0256d0,  6.3096d0,  6.6069d0,  6.9183d0,  7.2444d0,  7.5858d0,  &
      7.9433d0,  8.3176d0,  8.7096d0,  9.1201d0,  9.5499d0, 10.000d0,   &
     10.471d0,  10.965d0,  11.482d0,  12.023d0,  12.589d0,  13.183d0,   &
     13.804d0,  14.454d0,  15.136d0,  15.849d0,  16.596d0,  17.378d0,   &
     18.197d0,  19.055d0,  19.953d0,  20.893d0,  21.878d0,  22.909d0,   &
     23.988d0,  25.119d0,  26.303d0,  27.542d0,  28.840d0,  30.200d0,   &
     31.623d0,  33.113d0,  34.674d0,  36.308d0,  38.019d0,  39.811d0,   &
     41.687d0,  43.652d0,  45.709d0,  47.863d0,  50.119d0,  52.481d0,   &
     54.954d0,  57.544d0,  60.256d0,  63.096d0,  66.069d0,  69.183d0,   &
     72.444d0,  75.858d0,  79.433d0,  83.176d0,  87.096d0,  91.201d0,   &
     95.499d0,100.00d0 /)
  ! Efficiency (total)
  REAL*8, PARAMETER :: EFF0(NE)                                         &
      =       (/ 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      1.00000d-6,1.00000d-6,3.00000d-6,5.00000d-6,4.00000d-6,4.00000d-6,&
      6.00000d-6,9.00000d-6,1.20000d-5,1.70000d-5,3.60000d-5,5.40000d-5,&
      7.00000d-5,1.06000d-4,1.48000d-4,2.48000d-4,3.74000d-4,5.11000d-4,&
      6.61000d-4,9.53000d-4,1.32400d-3,1.91900d-3,2.57200d-3,3.42800d-3,&
      4.78200d-3,6.31100d-3,8.37200d-3,1.11320d-2,1.43600d-2,1.88560d-2,&
      2.39690d-2,3.04640d-2,3.85180d-2,4.79960d-2,5.92610d-2,7.20370d-2,&
      8.75540d-2,1.04670d-1,1.24360d-1,1.44260d-1,1.67600d-1,1.92550d-1,&
      2.19830d-1,2.45220d-1,2.71260d-1,2.95940d-1,3.20590d-1,3.44590d-1,&
      3.66380d-1,3.85290d-1,4.02490d-1,4.17410d-1,4.30070d-1,4.40120d-1,&
      4.47510d-1,4.55340d-1,4.59120d-1,4.63940d-1,4.66210d-1,4.67680d-1,&
      4.70120d-1,4.70570d-1,4.72180d-1,4.73500d-1,4.75110d-1,4.75610d-1,&
      4.76660d-1,4.77150d-1,4.77350d-1,4.77890d-1,4.78970d-1,4.79310d-1,&
      4.80410d-1,4.81470d-1,4.81580d-1,4.83040d-1,4.82920d-1,4.82570d-1,&
      4.83820d-1,4.84220d-1,4.85050d-1,4.84690d-1,4.85190d-1,4.84650d-1,&
      4.84290d-1,4.83720d-1,4.84160d-1,4.83160d-1,4.81950d-1,4.79380d-1,&
      4.76860d-1,4.73220d-1,4.62790d-1,4.43970d-1,4.14520d-1,3.73500d-1,&
      3.19880d-1,2.59970d-1,1.96280d-1,1.36860d-1,8.72900d-2,5.09020d-2,&
      2.65530d-2,1.23380d-2,5.26700d-3,1.86600d-3,6.02000d-4,1.50000d-4,&
      4.60000d-5,3.00000d-6,3.00000d-6,0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0 /)
  ! Efficiency (first and only interval)
  REAL*8, PARAMETER :: EFF1(NE) = EFF0
  ! Efficiencies array (2D)
  REAL*8, PARAMETER :: EFF(NE,0:NEFF)                                   &
      = RESHAPE( (/ EFF0(:), EFF1(:) /) ,SHAPE(EFF))
  
  ! One call for all settings.
  ! Most of these _must_ be there to ensure everything get initialized.
  IF (intervals) THEN
    CALL SetDetector(D,mass=20d3,time=2d0*365d0,Nevents=0,              &
                     background=0.5d0,Zelem=18,                         &
                     NE=NE,E=E,Neff=NEFF,eff=EFF)
  ELSE
    CALL SetDetector(D,mass=20d3,time=2d0*365d0,Nevents=0,              &
                     background=0.5d0,Zelem=18,                         &
                     NE=NE,E=E,Neff=0,eff=EFF(:,0:0))
  END IF
  D%eff_file = '[DARWIN Ar 2014]'
  
END SUBROUTINE



!=======================================================================
! DARWIN XENON ANALYSIS ROUTINES
! Based upon a DARWIN xenon-based analysis (2014 estimated parameters)
! [14MM.XXXX].  Zero events assumed in the analysis region.
!=======================================================================

!-----------------------------------------------------------------------
! Initializes the module to perform calculations for a DARWIN
! xenon-based analysis (2014 estimates).  This must be called if any
! of the following DARWIN routines are to be used.
! 
! Required input arguments:
!     intervals   Indicates if sub-intervals should be included.
!                 Only necessary if confidence intervals using the
!                 maximum gap method are desired.
! 
SUBROUTINE DARWIN_Xe_2014_Init(intervals)
  IMPLICIT NONE
  LOGICAL, INTENT(IN) :: intervals
  
  CALL DARWIN_Xe_2014_InitTo(DARWIN_Xe_2014,intervals)
  
END SUBROUTINE


! ----------------------------------------------------------------------
! Calculates various rate quantities using the current WIMP.
! Must be called each time the WIMP parameters are modified.
! 
SUBROUTINE DARWIN_Xe_2014_CalcRates()
  IMPLICIT NONE
  CALL CalcRates(DARWIN_Xe_2014)
END SUBROUTINE


! ----------------------------------------------------------------------
! Returns the observed number of events.
! 
FUNCTION DARWIN_Xe_2014_Events() RESULT(N)
  IMPLICIT NONE
  INTEGER :: N
  CALL GetRates(DARWIN_Xe_2014,Nevents=N)
END FUNCTION


! ----------------------------------------------------------------------
! Returns the average expected number of background events.
! 
FUNCTION DARWIN_Xe_2014_Background() RESULT(b)
  IMPLICIT NONE
  REAL*8 :: b
  CALL GetRates(DARWIN_Xe_2014,background=b)
END FUNCTION


! ----------------------------------------------------------------------
! Returns the average expected number of signal events for the
! current WIMP.
! 
FUNCTION DARWIN_Xe_2014_Signal() RESULT(s)
  IMPLICIT NONE
  REAL*8 :: s
  CALL GetRates(DARWIN_Xe_2014,signal=s)
END FUNCTION


! ----------------------------------------------------------------------
! Returns the average expected number of spin-independent signal events
! for the current WIMP.
! 
FUNCTION DARWIN_Xe_2014_SignalSI() RESULT(s)
  IMPLICIT NONE
  REAL*8 :: s
  CALL GetRates(DARWIN_Xe_2014,signal_si=s)
END FUNCTION


! ----------------------------------------------------------------------
! Returns the average expected number of spin-dependent signal events
! for the current WIMP.
! 
FUNCTION DARWIN_Xe_2014_SignalSD() RESULT(s)
  IMPLICIT NONE
  REAL*8 :: s
  CALL GetRates(DARWIN_Xe_2014,signal_sd=s)
END FUNCTION


! ----------------------------------------------------------------------
! Calculates the log-likelihood for the current WIMP mass and couplings.
! Uses a Poisson distribution in the number of observed events N:
!    P(N|s+b)
! where s is the average expected signal and b is the average expected
! background.
! 
FUNCTION DARWIN_Xe_2014_LogLikelihood() RESULT(lnlike)
  IMPLICIT NONE
  REAL*8 :: lnlike
  lnlike = LogLikelihood(DARWIN_Xe_2014)
END FUNCTION


! ----------------------------------------------------------------------
! Calculates the log of the p-value for the current WIMP mass and
! couplings (NO BACKGROUND SUBTRACTION).  Uses the maximum gap method
! if DARWIN_Xe_2014_Init was called with argument intervals=.TRUE.,
! otherwise uses a Poisson distribution in the number of observed
! events N:
!    P(N|s)
! where s is the average expected signal (background contributions are
! ignored).
! 
FUNCTION DARWIN_Xe_2014_LogPValue() RESULT(lnp)
  IMPLICIT NONE
  REAL*8 :: lnp
  lnp = LogPValue(DARWIN_Xe_2014)
END FUNCTION


! ----------------------------------------------------------------------
! Calculates the factor x by which the cross-sections must be scaled
! (sigma -> x*sigma) to achieve the desired p-value (given as log(p)).
! See LogPValue() above for a description of the statistics.
! 
! Required input argument:
!   lnp         The logarithm of the desired p-value (p = 1-CL).
! 
FUNCTION DARWIN_Xe_2014_ScaleToPValue(lnp) RESULT(x)
  IMPLICIT NONE
  REAL*8 :: x
  REAL*8, INTENT(IN) :: lnp
  x = ScaleToPValue(DARWIN_Xe_2014,lnp)
END FUNCTION


!-----------------------------------------------------------------------
! INTERNAL ROUTINE.
! Initializes the given DetectorRateStruct to the proposed DARWIN
! xenon-based detector (2014 estimate).  This is meant as an internal
! routine; external access should be through
! DARWIN_Xe_2014_InitDetector instead.
! 
! The efficiencies used here were generated using TPCMC.
! 
! Required input arguments:
!     D           The DetectorRateStruct to initialize
!     intervals   Indicates if sub-intervals should be included
! 
SUBROUTINE DARWIN_Xe_2014_InitTo(D,intervals)
  IMPLICIT NONE
  TYPE(DetectorRateStruct), INTENT(OUT) :: D
  LOGICAL, INTENT(IN) :: intervals
  INTEGER, PARAMETER :: NE = 151
  INTEGER, PARAMETER :: NEFF = 1
  ! Efficiency curves energy tabulation points
  REAL*8, PARAMETER :: E(NE)                                            &
      =       (/ 0.10000d0, 0.10471d0, 0.10965d0, 0.11482d0, 0.12023d0, &
      0.12589d0, 0.13183d0, 0.13804d0, 0.14454d0, 0.15136d0, 0.15849d0, &
      0.16596d0, 0.17378d0, 0.18197d0, 0.19055d0, 0.19953d0, 0.20893d0, &
      0.21878d0, 0.22909d0, 0.23988d0, 0.25119d0, 0.26303d0, 0.27542d0, &
      0.28840d0, 0.30200d0, 0.31623d0, 0.33113d0, 0.34674d0, 0.36308d0, &
      0.38019d0, 0.39811d0, 0.41687d0, 0.43652d0, 0.45709d0, 0.47863d0, &
      0.50119d0, 0.52481d0, 0.54954d0, 0.57544d0, 0.60256d0, 0.63096d0, &
      0.66069d0, 0.69183d0, 0.72444d0, 0.75858d0, 0.79433d0, 0.83176d0, &
      0.87096d0, 0.91201d0, 0.95499d0, 1.0000d0,  1.0471d0,  1.0965d0,  &
      1.1482d0,  1.2023d0,  1.2589d0,  1.3183d0,  1.3804d0,  1.4454d0,  &
      1.5136d0,  1.5849d0,  1.6596d0,  1.7378d0,  1.8197d0,  1.9055d0,  &
      1.9953d0,  2.0893d0,  2.1878d0,  2.2909d0,  2.3988d0,  2.5119d0,  &
      2.6303d0,  2.7542d0,  2.8840d0,  3.0200d0,  3.1623d0,  3.3113d0,  &
      3.4674d0,  3.6308d0,  3.8019d0,  3.9811d0,  4.1687d0,  4.3652d0,  &
      4.5709d0,  4.7863d0,  5.0119d0,  5.2481d0,  5.4954d0,  5.7544d0,  &
      6.0256d0,  6.3096d0,  6.6069d0,  6.9183d0,  7.2444d0,  7.5858d0,  &
      7.9433d0,  8.3176d0,  8.7096d0,  9.1201d0,  9.5499d0, 10.000d0,   &
     10.471d0,  10.965d0,  11.482d0,  12.023d0,  12.589d0,  13.183d0,   &
     13.804d0,  14.454d0,  15.136d0,  15.849d0,  16.596d0,  17.378d0,   &
     18.197d0,  19.055d0,  19.953d0,  20.893d0,  21.878d0,  22.909d0,   &
     23.988d0,  25.119d0,  26.303d0,  27.542d0,  28.840d0,  30.200d0,   &
     31.623d0,  33.113d0,  34.674d0,  36.308d0,  38.019d0,  39.811d0,   &
     41.687d0,  43.652d0,  45.709d0,  47.863d0,  50.119d0,  52.481d0,   &
     54.954d0,  57.544d0,  60.256d0,  63.096d0,  66.069d0,  69.183d0,   &
     72.444d0,  75.858d0,  79.433d0,  83.176d0,  87.096d0,  91.201d0,   &
     95.499d0,100.00d0 /)
  ! Efficiency (total)
  REAL*8, PARAMETER :: EFF0(NE)                                         &
      =       (/ 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0, 0.00000d0, 1.00000d-5,2.00000d-5,0.00000d0, &
      1.00000d-5,2.00000d-5,1.00000d-5,3.00000d-5,6.00000d-5,4.00000d-5,&
      1.60000d-4,2.50000d-4,3.30000d-4,4.30000d-4,5.80000d-4,8.30000d-4,&
      1.29000d-3,1.73000d-3,2.61000d-3,3.38000d-3,4.71000d-3,6.67000d-3,&
      8.93000d-3,1.18500d-2,1.66700d-2,2.11800d-2,2.76600d-2,3.70100d-2,&
      4.62800d-2,5.57800d-2,7.03000d-2,8.48500d-2,1.00920d-1,1.19770d-1,&
      1.40370d-1,1.59900d-1,1.83670d-1,2.13730d-1,2.34620d-1,2.60470d-1,&
      2.90490d-1,3.10700d-1,3.37130d-1,3.59870d-1,3.81220d-1,3.97590d-1,&
      4.13050d-1,4.29350d-1,4.33960d-1,4.43460d-1,4.50340d-1,4.54710d-1,&
      4.58700d-1,4.64210d-1,4.63840d-1,4.65210d-1,4.67910d-1,4.69700d-1,&
      4.71550d-1,4.70310d-1,4.72880d-1,4.73510d-1,4.77040d-1,4.76320d-1,&
      4.78180d-1,4.74830d-1,4.77390d-1,4.79300d-1,4.80260d-1,4.81980d-1,&
      4.79980d-1,4.83400d-1,4.85070d-1,4.83660d-1,4.86320d-1,4.83460d-1,&
      4.84910d-1,4.85100d-1,4.81940d-1,4.83840d-1,4.83400d-1,4.78140d-1,&
      4.77990d-1,4.70180d-1,4.48760d-1,4.18240d-1,3.67580d-1,3.03320d-1,&
      2.31100d-1,1.54890d-1,9.42000d-2,5.02400d-2,2.18100d-2,8.18000d-3,&
      2.44000d-3,7.40000d-4,1.70000d-4,1.00000d-5,1.00000d-5,0.00000d0, &
      0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, 0.00000d0, &
      0.00000d0, 0.00000d0 /)
  ! Efficiency (first and only interval)
  REAL*8, PARAMETER :: EFF1(NE) = EFF0
  ! Efficiencies array (2D)
  REAL*8, PARAMETER :: EFF(NE,0:NEFF)                                   &
      = RESHAPE( (/ EFF0(:), EFF1(:) /) ,SHAPE(EFF))
  
  ! One call for all settings.
  ! Most of these _must_ be there to ensure everything get initialized.
  IF (intervals) THEN
    CALL SetDetector(D,mass=12d3,time=2d0*365d0,Nevents=0,              &
                     background=0.5d0,Zelem=54,                         &
                     NE=NE,E=E,Neff=NEFF,eff=EFF)
  ELSE
    CALL SetDetector(D,mass=12d3,time=2d0*365d0,Nevents=0,              &
                     background=0.5d0,Zelem=54,                         &
                     NE=NE,E=E,Neff=0,eff=EFF(:,0:0))
  END IF
  D%eff_file = '[DARWIN Xe 2014]'
  
END SUBROUTINE



!=======================================================================
! MAIN ROUTINES
!=======================================================================

! ----------------------------------------------------------------------
! Generic main routine.
! 
SUBROUTINE main()
  IMPLICIT NONE
  LOGICAL :: interactive
  
  ! Show usage and exit
  IF (ShowUsageRequested()) THEN
    CALL ShowUsage()
    STOP
  END IF
  
  ! Load arguments
  CALL ReadArguments()
  
  ! Interactive mode?
  ! True if --interactive given or if the WIMP mass is not specified
  ! through the command-line (either with --m=<val> option or non-option
  ! argument).
  interactive = GetLongArg('interactive') .OR.                          &
                ((Arguments%Nparameters .EQ. 0) .AND. .NOT. GetLongArg('m'))
  
  ! Determine program mode =============
  
  ! ------------------------------------
  ! Calculates the log-likelihood (w/ background)
  IF (GetLongArg('log-likelihood')) THEN
    IF (interactive) THEN
      CALL MainLogLikelihoodInteractive()
    ELSE
      CALL MainLogLikelihood()
    END IF
  ! ------------------------------------
  ! Calculates the log of the p-value (no background subtraction)
  ELSE IF (GetLongArg('log-pvalue')) THEN
    IF (interactive) THEN
      CALL MainLogPValueInteractive()
    ELSE
      CALL MainLogPValue()
    END IF
  ! ------------------------------------
  ! Prints the raw recoil spectrum dR/dE
  ELSE IF (GetLongArg('spectrum')) THEN
    CALL MainSpectrum()
  ! ------------------------------------
  ! Calculates expected events, tabulated by WIMP mass
  ELSE IF (GetLongArg('events-by-mass')) THEN
    CALL MainEventsByMass()
  ! ------------------------------------
  ! Calculates spin-independent likelihood contraints,
  ! tabulated by WIMP mass
  ELSE IF (GetLongArg('constraints-SI')) THEN
    CALL MainConstraintsSI()
  ! ------------------------------------
  ! Calculates spin-dependent likelihood contraints,
  ! tabulated by WIMP mass
  ELSE IF (GetLongArg('constraints-SD')) THEN
    CALL MainConstraintsSD()
  ! ------------------------------------
  ! Calculates spin-independent no-background-subtraction limits,
  ! tabulated by WIMP mass
  ELSE IF (GetLongArg('limits-SI')) THEN
    CALL MainLimitsSI()
  ! ------------------------------------
  ! Calculates spin-dependent no-background-subtraction limits,
  ! tabulated by WIMP mass
  ELSE IF (GetLongArg('limits-SD')) THEN
    CALL MainLimitsSD()
  ! ------------------------------------
  ! Default case:
  ! Calculate both expected events and likelihoods
  ELSE
    IF (interactive) THEN
      CALL MainEventsAndLikelihoodsInteractive()
    ELSE IF (Arguments%Nparameters .GE. 1) THEN
      CALL MainEventsAndLikelihoods()
    ELSE
      CALL MainEventsAndLikelihoodsInteractive()
    END IF
  END IF
  
END SUBROUTINE


! ----------------------------------------------------------------------
! Main routine to calculate log-likelihood.
! 
SUBROUTINE MainLogLikelihood()
  IMPLICIT NONE
  REAL*8 :: lnLike
  
  ! Show usage and exit
  IF (ShowUsageRequested()) THEN
    CALL ShowUsage()
    STOP
  END IF
  
  ! Initializes various global structures.
  ! Will calculate only total rates (not sub-intervals).
  CALL InitializeCL(intervals=.FALSE.)
  
  ! Print header
  IF (VerbosityLevel .GE. 2) THEN
    CALL WriteCommandHeader()
    CALL WriteWIMPHeader()
    CALL WriteHaloHeader()
    CALL WriteDetectorHeader()
    CALL WriteLogLikelihoodHeader()
  END IF
  
  ! Do rate calculations
  CALL CalcRates()
  
  ! Get log-likelihood
  lnLike = LogLikelihood()
  
  ! Print results
  WRITE(*,*)  lnLike
  
END SUBROUTINE


! ----------------------------------------------------------------------
! Main routine to calculate log-likelihood (interactive mode).
! Reads one line of input containing WIMP parameters and writes out the
! corresponding log-likelihood, terminating when the input stream ends
! (EOF) or a blank line is given.
! 
SUBROUTINE MainLogLikelihoodInteractive()
  IMPLICIT NONE
  REAL*8 :: lnLike
  
  ! Show usage and exit
  IF (ShowUsageRequested()) THEN
    CALL ShowUsage()
    STOP
  END IF
  
  ! Initializes various global structures.
  ! Will calculate only total rates (not sub-intervals).
  CALL InitializeCL(intervals=.FALSE.)
  
  ! Print header
  IF (VerbosityLevel .GE. 2) THEN
    CALL WriteCommandHeader()
    CALL WriteHaloHeader()
    CALL WriteDetectorHeader()
    CALL WriteLogLikelihoodHeader()
  END IF
  
  ! Print instructions
  IF (VerbosityLevel .GE. 1) THEN
    CALL WriteInteractiveHeader()
  END IF
  
  ! Cycle over input.
  ! ParseWIMPInput() reads line containing WIMP parameters from
  ! standard input and parses it, returning false if a blank line
  ! or EOF is found.  See ParseWIMPInput() for description of input
  ! line.
  DO WHILE (ParseWIMPInput())
    ! Do rate calculations
    CALL CalcRates()
    ! Get log-likelihood
    lnLike = LogLikelihood()
    ! Print results to standard output
    WRITE(*,*)  lnLike
  END DO
  
END SUBROUTINE


! ----------------------------------------------------------------------
! Main routine to calculate log of p-value using the maximum gap method.
! See S. Yellin, PRD 66, 032005 (2002) [physics/0203002].  Uses Poisson
! distribution if efficiencies for sub-intervals are not available (but
! no background subtraction!).
! 
SUBROUTINE MainLogPValue()
  IMPLICIT NONE
  REAL*8 :: lnP
  
  ! Show usage and exit
  IF (ShowUsageRequested()) THEN
    CALL ShowUsage()
    STOP
  END IF
  
  ! Initializes various global structures.
  ! Will calculate rates for sub-intervals as these are used by
  ! maximum gap method.
  CALL InitializeCL(intervals=.TRUE.)
  
  ! Print header
  IF (VerbosityLevel .GE. 2) THEN
    CALL WriteCommandHeader()
    CALL WriteWIMPHeader()
    CALL WriteHaloHeader()
    CALL WriteDetectorHeader()
    CALL WriteLogPValueHeader()
  END IF
  
  ! Do rate calculations
  CALL CalcRates()
  
  ! Get log of p-value
  lnP = LogPValue()
  
  ! Print results
  WRITE(*,*)  lnP
  
END SUBROUTINE


! ----------------------------------------------------------------------
! Main routine to calculate log of p-value using the maximum gap method
! (interactive mode).  See S. Yellin, PRD 66, 032005 (2002)
! [physics/0203002].  Uses Poisson distribution if efficiencies for
! sub-intervals are not available (but no background subtraction!).
! Reads one line of input containing WIMP parameters and writes out the
! corresponding log of the p-value, terminating when the input stream
! ends (EOF) or a blank line is given.
! 
SUBROUTINE MainLogPValueInteractive()
  IMPLICIT NONE
  REAL*8 :: lnP
  
  ! Show usage and exit
  IF (ShowUsageRequested()) THEN
    CALL ShowUsage()
    STOP
  END IF
  
  ! Initializes various global structures.
  ! Will calculate rates for sub-intervals as these are used by
  ! maximum gap method.
  CALL InitializeCL(intervals=.TRUE.)
  
  ! Print header
  IF (VerbosityLevel .GE. 2) THEN
    CALL WriteCommandHeader()
    CALL WriteHaloHeader()
    CALL WriteDetectorHeader()
    CALL WriteLogPValueHeader()
  END IF
  
  ! Print instructions
  IF (VerbosityLevel .GE. 1) THEN
    CALL WriteInteractiveHeader()
  END IF
  
  ! Cycle over input.
  ! ParseWIMPInput() reads line containing WIMP parameters from
  ! standard input and parses it, returning false if a blank line
  ! or EOF is found.  See ParseWIMPInput() for description of input
  ! line.
  DO WHILE (ParseWIMPInput())
    ! Do rate calculations
    CALL CalcRates()
    ! Get log of p-value
    lnP = LogPValue()
    ! Print results to standard output
    WRITE(*,*)  lnP
  END DO
  
END SUBROUTINE


! ----------------------------------------------------------------------
! Main routine to print expected events and likelihoods.
! 
SUBROUTINE MainEventsAndLikelihoods()
  IMPLICIT NONE
  
  ! Show usage and exit
  IF (ShowUsageRequested()) THEN
    CALL ShowUsage()
    STOP
  END IF
  
  ! Initializes various global structures.
  CALL InitializeCL()
  
  ! Print header
  IF (VerbosityLevel .GE. 2) THEN
    CALL WriteCommandHeader()
    IF (VerbosityLevel .EQ. 2) CALL WriteWIMPHeader()
    CALL WriteHaloHeader()
    CALL WriteDetectorHeader()
  END IF
  IF (VerbosityLevel .GE. 1) THEN
    CALL WriteEventsAndLikelihoodsHeader()
    CALL WriteEventsAndLikelihoodsColumnHeader()
  END IF
  
  ! Do rate calculations
  CALL CalcRates()
  
  ! Print results
  CALL WriteEventsAndLikelihoodsData()
  
END SUBROUTINE


! ----------------------------------------------------------------------
! Main routine to print expected events and likelihoods.
! 
SUBROUTINE MainEventsAndLikelihoodsInteractive()
  IMPLICIT NONE
  
  ! Show usage and exit
  IF (ShowUsageRequested()) THEN
    CALL ShowUsage()
    STOP
  END IF
  
  ! Initializes various global structures.
  CALL InitializeCL()
  
  ! Print header
  IF (VerbosityLevel .GE. 2) THEN
    CALL WriteCommandHeader()
    CALL WriteHaloHeader()
    CALL WriteDetectorHeader()
    CALL WriteEventsAndLikelihoodsHeader()
  END IF
  
  ! Print instructions
  IF (VerbosityLevel .GE. 1) THEN
    CALL WriteInteractiveHeader(1)
  END IF
  IF (VerbosityLevel .GE. 1) THEN
    CALL WriteEventsAndLikelihoodsColumnHeader()
  END IF
  
  ! Cycle over input.
  ! ParseWIMPInput() reads line containing WIMP parameters from
  ! standard input and parses it, returning false if a blank line
  ! or EOF is found.  See ParseWIMPInput() for description of input
  ! line.
  DO WHILE (ParseWIMPInput())
    ! Do rate calculations
    CALL CalcRates()
    ! Print results to standard output
    CALL WriteEventsAndLikelihoodsData()
  END DO
  
END SUBROUTINE


! ----------------------------------------------------------------------
! Main routine to calculate raw differential rates as a function of
! energy, i.e. the raw recoil energy spectrum (not including
! efficiencies or energy resolution).
! 
SUBROUTINE MainSpectrum()
  IMPLICIT NONE
  LOGICAL :: use_log
  INTEGER :: NE,K
  REAL*8 :: Emin,Emax
  REAL*8, ALLOCATABLE :: E(:),eff(:,:)
  TYPE(TabulationStruct) :: TS
  
  ! Show usage and exit
  IF (ShowUsageRequested()) THEN
    CALL ShowUsage()
    STOP
  END IF
  
  ! Initializes various global structures.
  CALL InitializeCL()
  
  ! Set tabulation energies; set efficiencies to 1.
  Emin = 0.1d0
  Emax = 1000d0
  NE   = -50
  use_log = .TRUE.
  CALL GetTabulationArgs('E-tabulation',Emin,Emax,NE,use_log)
  CALL InitTabulation(TS,Emin,Emax,NE,.TRUE.)
  NE = TS%N+2
  ALLOCATE(E(1:NE),eff(1:NE,0:0))
  E(1) = 0d0
  DO K = 2,NE
    E(K) = TabulationValue(TS,K-2)
  END DO
  eff = 1d0
  CALL SetDetector(NE=NE,E=E,Neff=0,eff=eff)
  
  ! For high verbosity level, we are printing reference rates, so
  ! set "actual" rates to same.
  IF (VerbosityLevel .GE. 4) CALL SetWIMP(sigmaSI=1d0,sigmaSD=1d0)
  
  ! Do rate calculations
  CALL CalcRates()
  
  ! Write out header.
  IF (VerbosityLevel .GE. 2) THEN
    CALL WriteCommandHeader()
    CALL WriteWIMPHeader()
    CALL WriteHaloHeader()
    CALL WriteDetectorHeader()
    CALL WriteSpectrumHeader()
  END IF
  
  IF (VerbosityLevel .GE. 1) THEN
    CALL WriteSpectrumColumnHeader()
  END IF
  
  ! Write out table.
  CALL WriteSpectrumData()
  
END SUBROUTINE


! ----------------------------------------------------------------------
! Main routine to calculate events as a function of mass.
! Given for fixed WIMP-nucleon cross-sections.
! 
SUBROUTINE MainEventsByMass()
  IMPLICIT NONE
  LOGICAL :: use_log
  INTEGER :: Nm,I
  REAL*8 :: m,mmin,mmax,sigmapSI,sigmanSI,sigmapSD,sigmanSD
  TYPE(TabulationStruct) :: TS
  
  ! Show usage and exit
  IF (ShowUsageRequested()) THEN
    CALL ShowUsage()
    STOP
  END IF
  
  ! Initializes various global structures.
  CALL InitializeCL()
  
  ! Determine WIMP mass tabulation
  mmin    =    1d0
  mmax    = 1000d0
  Nm      =  -20
  use_log = .TRUE.
  CALL GetTabulationArgs('m-tabulation',mmin,mmax,Nm,use_log)
  CALL InitTabulation(TS,mmin,mmax,Nm,use_log)
  
  ! Get fixed cross-sections; will need to be reset at each mass.
  ! Set to negative if coupling is negative (intended meaning for
  ! negative cross-sections as input).
  CALL GetWIMP(sigmapSI=sigmapSI,sigmanSI=sigmanSI,sigmapSD=sigmapSD,sigmanSD=sigmanSD)
  IF (WIMP%GpSI .LT. 0d0) sigmapSI = -ABS(sigmapSI)
  IF (WIMP%GnSI .LT. 0d0) sigmanSI = -ABS(sigmanSI)
  IF (WIMP%GpSD .LT. 0d0) sigmapSD = -ABS(sigmapSD)
  IF (WIMP%GnSD .LT. 0d0) sigmanSD = -ABS(sigmanSD)
  
  ! Write out header.
  IF (VerbosityLevel .GE. 2) THEN
    CALL WriteCommandHeader()
    CALL WriteHaloHeader()
    CALL WriteDetectorHeader()
    CALL WriteEventsByMassHeader()
  END IF
  
  IF (VerbosityLevel .GE. 1) THEN
    CALL WriteEventsByMassColumnHeader()
  END IF
  
  ! Cycle over masses
  DO I = 0,TS%N
    ! Set WIMP mass and cross-sections
    m = TabulationValue(TS,I)
    CALL SetWIMP(m=m,sigmapSI=sigmapSI,sigmanSI=sigmanSI,sigmapSD=sigmapSD,sigmanSD=sigmanSD)
    ! Do rate calculations
    CALL CalcRates()
    ! Write out table data line.
    CALL WriteEventsByMassData()
  END DO
  
END SUBROUTINE


! ----------------------------------------------------------------------
! Main routine to calculate likelihood constraints as a function of
! mass for SI couplings.
! 
SUBROUTINE MainConstraintsSI()
  IMPLICIT NONE
  LOGICAL :: use_log
  INTEGER :: Nm,I
  REAL*8 :: m,mmin,mmax,x,lnp,thetaG,Gp,Gn,s1,s2
  TYPE(TabulationStruct) :: TS
  
  ! Show usage and exit
  IF (ShowUsageRequested()) THEN
    CALL ShowUsage()
    STOP
  END IF
  
  ! Initializes various global structures.
  ! Do not need sub-intervals as we only use the total events.
  CALL InitializeCL(intervals=.FALSE.)
  
  ! Determine WIMP mass tabulation
  mmin    =    1d0
  mmax    = 1000d0
  Nm      =  -20
  use_log = .TRUE.
  CALL GetTabulationArgs('m-tabulation',mmin,mmax,Nm,use_log)
  CALL InitTabulation(TS,mmin,mmax,Nm,use_log)
  
  ! Get p-value for exclusion limit
  CALL ReadLogPValue(lnp)
  
  ! Get angle of (Gp,Gn), which will be kept fixed.
  IF (GetLongArgReal('theta-SI',x)) THEN
    thetaG = x
  ELSE IF (GetLongArgReal('theta-SI-pi',x)) THEN
    thetaG = PI*x
  ELSE
    thetaG = 0.25d0*PI
  END IF
  
  ! Will initially use these couplings at every mass;
  ! constraint calculations will perform appropriate rescaling.
  Gp = 1d0*COS(thetaG)
  Gn = 1d0*SIN(thetaG)
  IF (ABS(Gp) .LT. 1d-8) Gp = 0d0
  IF (ABS(Gn) .LT. 1d-8) Gn = 0d0
  
  ! Calculate allowed signal rates
  CALL FeldmanCousinsPoissonCI(lnp,DefaultDetector%Nevents,             &
                               DefaultDetector%MuBackground,s1,s2)
  
  ! Write out header.
  IF (VerbosityLevel .GE. 2) THEN
    CALL WriteCommandHeader()
    CALL WriteHaloHeader()
    CALL WriteDetectorHeader()
    CALL WriteConstraintsSIHeader(lnp,thetaG,s1,s2)
  END IF
  
  IF (VerbosityLevel .GE. 1) THEN
    CALL WriteConstraintsSIColumnHeader()
  END IF
  
  ! Cycle over masses
  DO I = 0,TS%N
    ! Set WIMP mass and cross-sections
    m = TabulationValue(TS,I)
    CALL SetWIMP(m=m,GpSI=Gp,GnSI=Gn,GpSD=0d0,GnSD=0d0)
    ! Do rate calculations
    CALL CalcRates()
    ! Write out table data line.
    CALL WriteConstraintsSIData(s1,s2)
  END DO
  
END SUBROUTINE


! ----------------------------------------------------------------------
! Main routine to calculate likelihood constraints as a function of
! mass for SD couplings.
! 
SUBROUTINE MainConstraintsSD()
  IMPLICIT NONE
  LOGICAL :: use_log
  INTEGER :: Nm,I
  REAL*8 :: m,mmin,mmax,x,lnp,thetaG,Gp,Gn,s1,s2
  TYPE(TabulationStruct) :: TS
  
  ! Show usage and exit
  IF (ShowUsageRequested()) THEN
    CALL ShowUsage()
    STOP
  END IF
  
  ! Initializes various global structures.
  ! Do not need sub-intervals as we only use the total events.
  CALL InitializeCL(intervals=.FALSE.)
  
  ! Determine WIMP mass tabulation
  mmin    =    1d0
  mmax    = 1000d0
  Nm      =  -20
  use_log = .TRUE.
  CALL GetTabulationArgs('m-tabulation',mmin,mmax,Nm,use_log)
  CALL InitTabulation(TS,mmin,mmax,Nm,use_log)
  
  ! Get p-value for exclusion limit
  CALL ReadLogPValue(lnp)
  
  ! Get angle of (Gp,Gn), which will be kept fixed.
  IF (GetLongArgReal('theta-SD',x)) THEN
    thetaG = x
  ELSE IF (GetLongArgReal('theta-SD-pi',x)) THEN
    thetaG = PI*x
  ELSE
    thetaG = 0.25d0*PI
  END IF
  
  ! Will initially use these couplings at every mass;
  ! constraint calculations will perform appropriate rescaling.
  Gp = 1d0*COS(thetaG)
  Gn = 1d0*SIN(thetaG)
  IF (ABS(Gp) .LT. 1d-8) Gp = 0d0
  IF (ABS(Gn) .LT. 1d-8) Gn = 0d0
  
  ! Calculate allowed signal rates
  CALL FeldmanCousinsPoissonCI(lnp,DefaultDetector%Nevents,             &
                               DefaultDetector%MuBackground,s1,s2)
  
  ! Write out header.
  IF (VerbosityLevel .GE. 2) THEN
    CALL WriteCommandHeader()
    CALL WriteHaloHeader()
    CALL WriteDetectorHeader()
    CALL WriteConstraintsSDHeader(lnp,thetaG,s1,s2)
  END IF
  
  IF (VerbosityLevel .GE. 1) THEN
    CALL WriteConstraintsSDColumnHeader()
  END IF
  
  ! Cycle over masses
  DO I = 0,TS%N
    ! Set WIMP mass and cross-sections
    m = TabulationValue(TS,I)
    CALL SetWIMP(m=m,GpSI=0d0,GnSI=0d0,GpSD=Gp,GnSD=Gn)
    ! Do rate calculations
    CALL CalcRates()
    ! Write out table data line.
    CALL WriteConstraintsSDData(s1,s2)
  END DO
  
END SUBROUTINE


! ----------------------------------------------------------------------
! Main routine to calculate exclusion limits as a function of mass for
! SI couplings.
! 
SUBROUTINE MainLimitsSI()
  IMPLICIT NONE
  LOGICAL :: use_log
  INTEGER :: Nm,I
  REAL*8 :: m,mmin,mmax,x,lnp,thetaG,Gp,Gn
  TYPE(TabulationStruct) :: TS
  
  ! Show usage and exit
  IF (ShowUsageRequested()) THEN
    CALL ShowUsage()
    STOP
  END IF
  
  ! Initializes various global structures.
  ! Will calculate rates for sub-intervals as these are used by
  ! maximum gap method.
  CALL InitializeCL(intervals=.TRUE.)
  
  ! Determine WIMP mass tabulation
  mmin    =    1d0
  mmax    = 1000d0
  Nm      =  -20
  use_log = .TRUE.
  CALL GetTabulationArgs('m-tabulation',mmin,mmax,Nm,use_log)
  CALL InitTabulation(TS,mmin,mmax,Nm,use_log)
  
  ! Get p-value for exclusion limit
  CALL ReadLogPValue(lnp)
  
  ! Get angle of (Gp,Gn), which will be kept fixed.
  IF (GetLongArgReal('theta-SI',x)) THEN
    thetaG = x
  ELSE IF (GetLongArgReal('theta-SI-pi',x)) THEN
    thetaG = PI*x
  ELSE
    thetaG = 0.25d0*PI
  END IF
  
  ! Will initially use these couplings at every mass;
  ! limit calculations will perform appropriate rescaling.
  Gp = 1d0*COS(thetaG)
  Gn = 1d0*SIN(thetaG)
  IF (ABS(Gp) .LT. 1d-8) Gp = 0d0
  IF (ABS(Gn) .LT. 1d-8) Gn = 0d0
  
  ! Write out header.
  IF (VerbosityLevel .GE. 2) THEN
    CALL WriteCommandHeader()
    CALL WriteHaloHeader()
    CALL WriteDetectorHeader()
    CALL WriteLimitsSIHeader(lnp,thetaG)
  END IF
  
  IF (VerbosityLevel .GE. 1) THEN
    CALL WriteLimitsSIColumnHeader()
  END IF
  
  ! Cycle over masses
  DO I = 0,TS%N
    ! Set WIMP mass and cross-sections
    m = TabulationValue(TS,I)
    CALL SetWIMP(m=m,GpSI=Gp,GnSI=Gn,GpSD=0d0,GnSD=0d0)
    ! Do rate calculations
    CALL CalcRates()
    ! Write out table data line.
    CALL WriteLimitsSIData(lnp)
  END DO
  
END SUBROUTINE


! ----------------------------------------------------------------------
! Main routine to calculate exclusion limits as a function of mass for
! SD couplings.
! 
SUBROUTINE MainLimitsSD()
  IMPLICIT NONE
  LOGICAL :: use_log
  INTEGER :: Nm,I
  REAL*8 :: m,mmin,mmax,x,lnp,thetaG,Gp,Gn
  TYPE(TabulationStruct) :: TS
  
  ! Show usage and exit
  IF (ShowUsageRequested()) THEN
    CALL ShowUsage()
    STOP
  END IF
  
  ! Initializes various global structures.
  ! Will calculate rates for sub-intervals as these are used by
  ! maximum gap method.
  CALL InitializeCL(intervals=.TRUE.)
  
  ! Determine WIMP mass tabulation
  mmin    =    1d0
  mmax    = 1000d0
  Nm      =  -20
  use_log = .TRUE.
  CALL GetTabulationArgs('m-tabulation',mmin,mmax,Nm,use_log)
  CALL InitTabulation(TS,mmin,mmax,Nm,use_log)
  
  ! Get p-value for exclusion limit
  CALL ReadLogPValue(lnp)
  
  ! Get angle of (Gp,Gn), which will be kept fixed.
  IF (GetLongArgReal('theta-SD',x)) THEN
    thetaG = x
  ELSE IF (GetLongArgReal('theta-SD-pi',x)) THEN
    thetaG = PI*x
  ELSE
    thetaG = 0.25d0*PI
  END IF
  
  ! Will initially use these couplings at every mass;
  ! limit calculations will perform appropriate rescaling.
  Gp = 1d0*COS(thetaG)
  Gn = 1d0*SIN(thetaG)
  IF (ABS(Gp) .LT. 1d-8) Gp = 0d0
  IF (ABS(Gn) .LT. 1d-8) Gn = 0d0
  
  ! Write out header.
  IF (VerbosityLevel .GE. 2) THEN
    CALL WriteCommandHeader()
    CALL WriteHaloHeader()
    CALL WriteDetectorHeader()
    CALL WriteLimitsSDHeader(lnp,thetaG)
  END IF
  
  IF (VerbosityLevel .GE. 1) THEN
    CALL WriteLimitsSDColumnHeader()
  END IF
  
  ! Cycle over masses
  DO I = 0,TS%N
    ! Set WIMP mass and cross-sections
    m = TabulationValue(TS,I)
    CALL SetWIMP(m=m,GpSI=0d0,GnSI=0d0,GpSD=Gp,GnSD=Gn)
    ! Do rate calculations
    CALL CalcRates()
    ! Write out table data line.
    CALL WriteLimitsSDData(lnp)
  END DO
  
END SUBROUTINE



!=======================================================================
! INITIALIZATION
!=======================================================================

! ----------------------------------------------------------------------
! Initialization routine that sets various parameters to default values.
! This must be called before using the module (unless one of the other
! initialization routines is called).
! 
! The current default initialization is to that of the LUX 2013
! analysis.  If a different setup is desired, the various Set() methods
! must be used.
! 
! Optional input arguments:
!   intervals   Indicates if rates for sub-intervals should be
!               calculated as well as the total rate (default: true).
!               These are unnecessary for some calculations.
! 
SUBROUTINE Initialize(intervals)
  IMPLICIT NONE
  LOGICAL, INTENT(IN), OPTIONAL :: intervals
  LOGICAL :: intervals0
  
  intervals0 = .TRUE.
  IF (PRESENT(intervals)) intervals0 = intervals
  
  ! Initialize WIMP mass and couplings.
  CALL InitWIMP()
  
  ! Initialize halo.
  CALL InitHalo()
  
  ! Initialize detector isotopes, efficiencies, array sizing, etc.
  CALL InitDetector(intervals=intervals0)
  
END SUBROUTINE


! ----------------------------------------------------------------------
! Initialization routine that sets various parameters to default values
! or values specified on the command line.  This must be called before
! using the module (unless one of the other initialization routines is
! called).
! 
! This version of the routine uses command-line options to set up the
! various structures.  It should _only_ be used in programs that intend
! to use such command-line options for setup; otherwise, the
! Initialize() routine above should be used instead.
! 
! Optional input arguments:
!   cmdline     Specifies if the command-line should be checked for
!               values (default: false).  Should be set false if this
!               module is used by external programs.
!   intervals   Indicates if rates for any sub-intervals available from
!               the efficiencies file should be calculated as well as
!               the total rate (default: true).  These are unnecessary
!               for some calculations.
! 
SUBROUTINE InitializeCL(intervals)
  IMPLICIT NONE
  LOGICAL, INTENT(IN), OPTIONAL :: intervals
  LOGICAL :: intervals0
  
  intervals0 = .TRUE.
  IF (PRESENT(intervals)) intervals0 = intervals
  
  ! Extract parameters and options from command-line.
  CALL ReadArguments()
  
  ! Set the output verbosity level.
  CALL InitVerbosity()
  
  ! Initialize WIMP mass and couplings.
  ! Requires ReadArguments to have been called.
  CALL InitWIMPCL()
  
  ! Initialize halo.
  CALL InitHaloCL()
  
  ! Initialize detector isotopes, efficiencies, array sizing, etc.
  CALL InitDetectorCL(intervals=intervals0)
  
END SUBROUTINE


! ----------------------------------------------------------------------
! Reads the command line arguments and separates them into options
! and parameters.  See the definition of the ArgumentStructure
! structure above for details.
! 
SUBROUTINE ReadArguments()
  IMPLICIT NONE
  LOGICAL :: L
  CHARACTER*2 :: firstchars
  INTEGER :: Narg,Nopt,Nparam,I,Iopt,Iparam,ios
  REAL*8 :: x
  
  Narg   = IARGC()
  Nopt   = 0
  Nparam = 0
  
  ! NOTE: cannot use flags of form -<flag> as negative numbers
  ! will be improperly parsed
  
  ! Count argument types
  DO I=1,Narg
    CALL GETARG(I,firstchars)
    IF (firstchars .EQ. '--') THEN
      Nopt = Nopt + 1
    ELSE
      Nparam = Nparam + 1
    END IF
  END DO
  
  Arguments%Noptions = Nopt
  IF (ALLOCATED(Arguments%options)) DEALLOCATE(Arguments%options)
  ALLOCATE(Arguments%options(1:Nopt))
  
  Arguments%Nparameters = Nparam
  IF (ALLOCATED(Arguments%parameters)) DEALLOCATE(Arguments%parameters)
  ALLOCATE(Arguments%parameters(1:Nparam))
  IF (ALLOCATED(Arguments%values)) DEALLOCATE(Arguments%values)
  ALLOCATE(Arguments%values(1:Nparam))
  
  ! Divide arguments up
  Iopt   = 0
  Iparam = 0
  DO I=1,Narg
    CALL GETARG(I,firstchars)
    IF (firstchars .EQ. '--') THEN
      Iopt = Iopt + 1
      CALL GETARG(I,Arguments%options(Iopt))
    ELSE
      Iparam = Iparam + 1
      CALL GETARG(I,Arguments%parameters(Iparam))
    END IF
  END DO
  
  ! Try to parse parameters to floating point or logical -> floating
  ! point (T=1,F=0).  Set to -HUGE(1d0) if cannot be parsed.
  DO I=1,Nparam
    READ(UNIT=Arguments%parameters(I),FMT=*,IOSTAT=ios) x
    IF (ios .EQ. 0) THEN
      Arguments%values(I) = x
    ELSE
      READ(UNIT=Arguments%parameters(I),FMT=*,IOSTAT=ios) L
      IF (ios .EQ. 0) THEN
        IF (L) THEN
          Arguments%values(I) = 1d0
        ELSE
          Arguments%values(I) = 0d0
        END IF
      ELSE
        Arguments%values(I) = -HUGE(1d0)
      END IF
    END IF
  END DO
  
END SUBROUTINE


! ----------------------------------------------------------------------
! Initializes the verbosity level using command line arguments or
! default values.
! 
! Possible options:
!   --verbosity=<value>  ! Level of verbosity (default is 1)
!   --verbose            ! Equivalent to --verbosity=2
!   --quiet              ! Equivalent to --verbosity=0
! 
SUBROUTINE InitVerbosity()
  IMPLICIT NONE
  INTEGER :: verb
  
  IF (GetLongArgInteger('verbosity',verb)) THEN
    VerbosityLevel = verb
  ELSE IF (GetLongArg('verbose')) THEN
    VerbosityLevel = 2
  ELSE IF (GetLongArg('quiet')) THEN
    VerbosityLevel = 0
  ELSE
    VerbosityLevel = 1
  END IF
  
END SUBROUTINE


! ----------------------------------------------------------------------
! Reads the p-value or CL from command line arguments or sets them to
! a default value [p=0.1 or 90% CL].  We use p+CL = 1 and return only
! the p-value (as the logarithm of its value).
! 
! Possible options:
!   --p-value=<val>         ! The p-value
!   --log-p-value=<val>     ! The logarithm of the p-value
!   --p-value-sigma=<val>   ! The p-value corresponding to the given number of s.d.'s
!                           ! in the normal distribution
!  --confidence-level=<val> ! The confidence level (1-p)
!  --confidence-level-sigma=<val>
!                           ! Equivalent to --p-value-sigma
! 
! Output argument:
!   lnp             The logarithm of the p-value (CL = 1-p)
! 
SUBROUTINE ReadLogPValue(lnp)
  IMPLICIT NONE
  REAL*8, INTENT(OUT) :: lnp
  LOGICAL :: calc_nsigma
  REAL*8 :: x,y,p,nsigma
  
  calc_nsigma = .FALSE.
  
  ! Default
  lnp = LOG(0.1d0)
  
  ! Process arguments
  IF (GetLongArgReal('confidence-level',x)) THEN
    lnp = LOG(MAX(1-x,TINY(1d0)))
  ELSE IF (GetLongArgReal('p-value',x)) THEN
    lnp = LOG(MAX(x,TINY(1d0)))
  ELSE IF (GetLongArgReal('log-p-value',x)) THEN
    lnp = x
  ELSE IF (GetLongArgReal('p-value-sigma',x)) THEN
    calc_nsigma = .TRUE.
    nsigma = x
  ELSE IF (GetLongArgReal('confidence-level-sigma',x)) THEN
    calc_nsigma = .TRUE.
    nsigma = x
  END IF
  
  ! calculate p-value in terms of normal distribution at nsigma s.d.'s.
  ! In that case:
  !    p = erfc(nsigma/\sqrt{2})
  IF (calc_nsigma) THEN
    IF (x .LT. 25d0) THEN
      lnp = LOG(ERFC(x/SQRT2))
    ELSE
      ! For large nsigma, use asymptotic expansion of erfc.
      ! Unnecessarily calculated to near full double precision....
      y = 1d0 / x**2
      lnp = -0.5d0*x**2 - LOG(SQRTPI*x/SQRT2) + LOGp1(-y*(1-3*y*(1-5*y*(1-7*y*(1-9*y*(1-11*y))))))
    END IF
  END IF
  
END SUBROUTINE



!=======================================================================
! OUTPUT ROUTINES
!=======================================================================

!-----------------------------------------------------------------------
! Prints the given number of empty comment lines (just the comment
! prefix).  Utility function.
! 
SUBROUTINE WriteEmptyCommentLines(N)
  IMPLICIT NONE
  INTEGER, INTENT(IN) :: N
  INTEGER :: I
  DO I = 1,N
    WRITE(*,'(A)') COMMENT_PREFIX
  END DO
END SUBROUTINE


!-----------------------------------------------------------------------
! Write command header.
! Outputs the command used and date.
! 
! Optional input arguments:
!   extra_lines     Blank lines (albeit with prefix) to add after
!                   output
! 
SUBROUTINE WriteCommandHeader(extra_lines)
  IMPLICIT NONE
  INTEGER, INTENT(IN), OPTIONAL :: extra_lines
  CHARACTER*1024 :: cmd
  CHARACTER*10 :: date,time
  CHARACTER*100 :: datetime
  
  ! Command
  CALL GetFullCommand(cmd)
  WRITE(*,'(A)') COMMENT_PREFIX &
      //  'Command: ' // TRIM(cmd)
  
  ! Date & time
  CALL DATE_AND_TIME(date,time)
  datetime = 'Generated on '                                            &
      // date(1:4) // '-' // date(5:6) // '-' // date(7:8)  // ' at '   &
      // time(1:2) // ':' // time(3:4) // ':' // time(5:10)
  WRITE(*,'(A)') COMMENT_PREFIX &
      // TRIM(datetime)
  
  ! Version
  !WRITE(*,'(A)') COMMENT_PREFIX &
  !    // 'DDCalc0 version: ' // TRIM(VERSION_STRING)
  !WRITE(*,'(A)') COMMENT_PREFIX &
  !    // 'See/cite C. Savage et al., arxiv:14MM.XXXX.'
  WRITE(*,'(A)') COMMENT_PREFIX &
      // 'DDCalc0 version ' // TRIM(VERSION_STRING) // '.  See/cite C. Savage et al., arxiv:14MM.XXXX.'
  
  WRITE(*,'(A)') COMMENT_PREFIX
  
  IF (PRESENT(extra_lines)) CALL WriteEmptyCommentLines(extra_lines)
  
END SUBROUTINE


!-----------------------------------------------------------------------
! Prints to standard output information regarding the WIMP,
! notably the mass and couplings.
! 
! Optional input arguments:
!   extra_lines     Blank lines (albeit with prefix) to add after
!                   output
! 
SUBROUTINE WriteWIMPHeader(extra_lines)
  IMPLICIT NONE
  INTEGER, INTENT(IN), OPTIONAL :: extra_lines
  
  !WRITE(*,'(A)') COMMENT_LINE
  
  ! Mass
  WRITE(*,'(A,F11.4)') COMMENT_PREFIX &
      // 'WIMP mass [GeV]                       =',WIMP%m
  WRITE(*,'(A)') COMMENT_PREFIX
  
  ! Couplings
  WRITE(*,'(A,2(2X,A12))') COMMENT_PREFIX &
      // 'WIMP-nucleon couplings:  ','  G [GeV^-2]','  sigma [pb]'
  WRITE(*,'(A,2(2X,1PG12.4))') COMMENT_PREFIX &
      // '  proton SI              ',WIMP%GpSI,GpToSigmapSI(WIMP%m,WIMP%GpSI)
  WRITE(*,'(A,2(2X,1PG12.4))') COMMENT_PREFIX &
      // '  neutron SI             ',WIMP%GnSI,GnToSigmanSI(WIMP%m,WIMP%GnSI)
  WRITE(*,'(A,2(2X,1PG12.4))') COMMENT_PREFIX &
      // '  proton SD              ',WIMP%GpSD,GpToSigmapSD(WIMP%m,WIMP%GpSD)
  WRITE(*,'(A,2(2X,1PG12.4))') COMMENT_PREFIX &
      // '  neutron SD             ',WIMP%GnSD,GnToSigmanSD(WIMP%m,WIMP%GnSD)
  WRITE(*,'(A)') COMMENT_PREFIX
  
  IF (PRESENT(extra_lines)) CALL WriteEmptyCommentLines(extra_lines)
  
END SUBROUTINE


!-----------------------------------------------------------------------
! Prints to standard output information regarding the dark matter halo,
! notably the density and velocity distribution.
! 
! Optional input arguments:
!   extra_lines     Blank lines (albeit with prefix) to add after
!                   output
! 
SUBROUTINE WriteHaloHeader(extra_lines)
  IMPLICIT NONE
  INTEGER, INTENT(IN), OPTIONAL :: extra_lines
  
  !WRITE(*,'(A)') COMMENT_LINE
  
  ! Solar motion
  WRITE(*,'(A)') COMMENT_PREFIX &
      // 'Sun''s velocity in the Galactic rest frame [km/s] in Galactic'
  WRITE(*,'(A)') COMMENT_PREFIX &
      // 'coordinates (U,V,W), where U is anti-radial (towards the'
  WRITE(*,'(A)') COMMENT_PREFIX &
      // 'Galactic center), V is in the direction of the disk rotation,'
  WRITE(*,'(A)') COMMENT_PREFIX &
      // 'and W is vertical (out of the disk plane):'
  WRITE(*,'(A,3(1X,F8.2),3X,A)') COMMENT_PREFIX &
      // '  ',Halo%vsun
  WRITE(*,'(A)') COMMENT_PREFIX
  
  ! Density
  WRITE(*,'(A,F11.4)') COMMENT_PREFIX &
      // 'Local dark matter density [GeV/cm^3]  =',Halo%rho
  WRITE(*,'(A)') COMMENT_PREFIX
  
  ! Halo velocity distribution
  IF (.NOT. Halo%tabulated) THEN
    ! Maxwell-Boltzmann parameters
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'SHM-like velocity distribution (Maxwell-Boltzmann with finite cutoff):'
    WRITE(*,'(A,3(1X,F8.2),3X,A)') COMMENT_PREFIX &
        // '  Bulk motion (galactic frame) [km/s] =',Halo%vbulk
    WRITE(*,'(A,F9.2)') COMMENT_PREFIX &
        // '  Most probable speed (v0) [km/s]     =',Halo%v0
    IF (Halo%vesc .GE. 1e6) THEN
      WRITE(*,'(A)') COMMENT_PREFIX &
        // '  Escape speed (vesc) [km/s]          = (infinite)'
    ELSE
      WRITE(*,'(A,F9.2)') COMMENT_PREFIX &
        // '  Escape speed (vesc) [km/s]          =',Halo%vesc
    END IF
  ELSE IF (Halo%eta_file .NE. '') THEN
    ! Tabulated from file
    !WRITE(*,'(A)') COMMENT_PREFIX &
    !    // 'The mean inverse speed was provided in tabulated form in the following'
    !WRITE(*,'(A)') COMMENT_PREFIX &
    !    // 'file:'
    !WRITE(*,'(A)') COMMENT_PREFIX &
    !    // '    ' // TRIM(Halo%eta_file)
    WRITE(*,'(A,A)') COMMENT_PREFIX &
        // 'Mean inverse speed tabulation file    = ',TRIM(Halo%eta_file)
  ELSE
    ! User provided tabulation
    ! This really shouldn't occur here because there is no way
    ! to provide the tabulation this way in program modes!
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'The mean inverse speed was provided in tabulated form by the user.'
  END IF
  WRITE(*,'(A)') COMMENT_PREFIX
  
  IF (PRESENT(extra_lines)) CALL WriteEmptyCommentLines(extra_lines)
  
END SUBROUTINE


!-----------------------------------------------------------------------
! Prints to standard output information regarding the detector.
! 
! Optional input arguments:
!   extra_lines     Blank lines (albeit with prefix) to add after
!                   output
! 
SUBROUTINE WriteDetectorHeader(extra_lines)
  IMPLICIT NONE
  INTEGER, INTENT(IN), OPTIONAL :: extra_lines
  
  !WRITE(*,'(A)') COMMENT_LINE
  
  ! Exposure, observed events, expected background events
  WRITE(*,'(A,1PG12.4)') COMMENT_PREFIX &
      // 'Detector exposure [kg day]            =',DefaultDetector%exposure
  IF (DefaultDetector%Nevents .GE. 0) THEN
    WRITE(*,'(A,I6)') COMMENT_PREFIX &
      // 'Observed events                       =',DefaultDetector%Nevents
  END IF
  WRITE(*,'(A,F11.4)') COMMENT_PREFIX &
      // 'Average expected background events    =',DefaultDetector%MuBackground
  WRITE(*,'(A)') COMMENT_PREFIX
  
  ! Detector isotopes
  WRITE(*,'(A,I6)') COMMENT_PREFIX &
      // 'Isotopes                              =',DefaultDetector%Niso
  WRITE(*,'(A,99(3X,I4,2X))') COMMENT_PREFIX &
      // '  Atomic number Z       ',DefaultDetector%Ziso
  WRITE(*,'(A,99(3X,I4,2X))') COMMENT_PREFIX &
      // '  Atomic mass number A  ',DefaultDetector%Aiso
  WRITE(*,'(A,99(1X,F8.5))') COMMENT_PREFIX &
      // '  Mass fraction         ',DefaultDetector%fiso
  WRITE(*,'(A)') COMMENT_PREFIX
  
  ! Efficiencies and intervals/bins
  WRITE(*,'(A,A)') COMMENT_PREFIX &
      // 'Efficiency file                       = ',TRIM(DefaultDetector%eff_file)
  IF (DefaultDetector%Neff .GT. 0) THEN
    WRITE(*,'(A,I6)') COMMENT_PREFIX &
      // 'Number of bins/sub-intervals          =',DefaultDetector%Neff
  END IF
  WRITE(*,'(A)') COMMENT_PREFIX
  
  IF (PRESENT(extra_lines)) CALL WriteEmptyCommentLines(extra_lines)
  
END SUBROUTINE


!-----------------------------------------------------------------------
! Write the interactive-mode instruction header.
! 
! Optional input arguments:
!   extra_lines     Blank lines (albeit with prefix) to add after
!                   output
! 
SUBROUTINE WriteInteractiveHeader(extra_lines)
  IMPLICIT NONE
  INTEGER, INTENT(IN), OPTIONAL :: extra_lines
  
  WRITE(*,'(A)') COMMENT_PREFIX &
      // 'Enter WIMP parameters in one of the following formats:'
  WRITE(*,'(A)') COMMENT_PREFIX &
      // '  $>  m'
  WRITE(*,'(A)') COMMENT_PREFIX &
      // '  $>  m sigmaSI'
  WRITE(*,'(A)') COMMENT_PREFIX &
      // '  $>  m sigmaSI sigmaSD'
  WRITE(*,'(A)') COMMENT_PREFIX &
      // '  $>  m sigmaSI sigmapSD sigmanSD'
  WRITE(*,'(A)') COMMENT_PREFIX &
      // '  $>  m sigmapSI sigmanSI sigmapSD sigmanSD'
  WRITE(*,'(A)') COMMENT_PREFIX &
      // 'where m is the WIMP mass [GeV] and sigma is a WIMP-nucleon cross-section'
  WRITE(*,'(A)') COMMENT_PREFIX &
      // '[pb].  In the first case, all cross-sections are set to 1 pb.  In the'
  WRITE(*,'(A)') COMMENT_PREFIX &
      // 'second case, SD couplings are set to zero.  Negative cross-sections may'
  WRITE(*,'(A)') COMMENT_PREFIX &
      // 'be given to indicate the corresponding coupling is negative.  A blank'
  WRITE(*,'(A)') COMMENT_PREFIX &
      // 'line will terminate the program (as will an invalid format).'
  !WRITE(*,'(A)') COMMENT_PREFIX &
  !    // '  $>  m GpSI GnSI GpSD GnSD'
  !WRITE(*,'(A)') COMMENT_PREFIX &
  !    // 'where m is the WIMP mass [GeV], sigma is a WIMP-nucleon cross-section'
  !WRITE(*,'(A)') COMMENT_PREFIX &
  !    // '[pb], and G is a WIMP-nucleon coupling [GeV^-2].  In the first case,'
  !WRITE(*,'(A)') COMMENT_PREFIX &
  !    // 'all cross-sections are set to 1 pb.  In the second case, SD couplings'
  !WRITE(*,'(A)') COMMENT_PREFIX &
  !    // 'are set to zero.  Negative cross-sections may be given to indicate'
  !WRITE(*,'(A)') COMMENT_PREFIX &
  !    // 'the corresponding coupling is negative.  A blank line will terminate'
  !WRITE(*,'(A)') COMMENT_PREFIX &
  !    // 'the program (as will an invalid format).'
  !WRITE(*,'(A)') COMMENT_PREFIX &
  !    // ''
  
  IF (PRESENT(extra_lines)) CALL WriteEmptyCommentLines(extra_lines)
  
END SUBROUTINE


!-----------------------------------------------------------------------
! Write log-likelihood header.
! 
! Optional input arguments:
!   extra_lines     Blank lines (albeit with prefix) to add after
!                   output
! 
SUBROUTINE WriteLogLikelihoodHeader(extra_lines)
  IMPLICIT NONE
  INTEGER, INTENT(IN), OPTIONAL :: extra_lines
  
  WRITE(*,'(A)') COMMENT_PREFIX &
      // 'The log-likelihood for the given parameters is given below,'
  WRITE(*,'(A)') COMMENT_PREFIX &
      // 'defined as L = P(N|s+b) where P is the Poisson distribution of'
  WRITE(*,'(A)') COMMENT_PREFIX &
      // 'N observed events given an expected signal s and background b.'
  
  IF (PRESENT(extra_lines)) CALL WriteEmptyCommentLines(extra_lines)
  
END SUBROUTINE


!-----------------------------------------------------------------------
! Write log p-value header.
! 
! Optional input arguments:
!   extra_lines     Blank lines (albeit with prefix) to add after
!                   output
! 
SUBROUTINE WriteLogPValueHeader(extra_lines)
  IMPLICIT NONE
  INTEGER, INTENT(IN), OPTIONAL :: extra_lines
  
  IF (DefaultDetector%Neff .EQ. DefaultDetector%Nevents+1) THEN
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'The log of the p-value for the given parameters is given below,'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'calculated using the maximum gap method.  See:'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '  S. Yellin, PRD 66, 032005 (2002) [physics/0203002]'
  ELSE
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'The log of the p-value for the given parameters are given below,'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'calculated using a Poisson distribution on the number of events.'
  END IF
  
  IF (PRESENT(extra_lines)) CALL WriteEmptyCommentLines(extra_lines)
  
END SUBROUTINE


!-----------------------------------------------------------------------
! Prints to standard output a header containing information regarding
! the events and likelihoods data to follow.
! 
! Optional input arguments:
!   extra_lines     Blank lines (albeit with prefix) to add after
!                   output
! 
SUBROUTINE WriteEventsAndLikelihoodsHeader(extra_lines)
  IMPLICIT NONE
  INTEGER, INTENT(IN), OPTIONAL :: extra_lines
  
  !WRITE(*,'(A)') COMMENT_LINE
  
  IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'The columns below contain the following data:'
  END IF
  
  IF (VerbosityLevel .GE. 3) THEN
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '  m            WIMP mass [GeV].'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '  GpSI,GnSI,GpSD,GnSD'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '               WIMP-nucleon spin-independent and spin-dependent couplings [GeV^2].'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '  sigmapSI,sigmanSI,sigmapSD,sigmanSD'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '               WIMP-nucleon SI and SD scattering cross-sections [pb].'
  END IF
  
  IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '  observed     The observed number of events.'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '  background   Average expected background events.'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '  signal(SI)   Average expected spin-independent signal events.'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '  signal(SD)   Average expected spin-dependent signal events.'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '  log(L)       Log-likelihood using the Poisson distribution (signal+background).'
    IF (DefaultDetector%Neff .EQ. DefaultDetector%Nevents+1) THEN
      WRITE(*,'(A)') COMMENT_PREFIX &
        // '  log(p)       Log of the p-value determined using the maximum gap method;'
      WRITE(*,'(A)') COMMENT_PREFIX &
        // '               see Yellin, Phys. Rev. D 66, 032005 (2002) [physics/0203002].'
    ELSE
      WRITE(*,'(A)') COMMENT_PREFIX &
        // '  log(p)       Log of the p-value determined using the Poisson distribution'
      WRITE(*,'(A)') COMMENT_PREFIX &
        // '               (signal only: no background subtraction).'
    END IF
  END IF
  
  IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(A)') COMMENT_PREFIX
  END IF
  
  IF (PRESENT(extra_lines)) CALL WriteEmptyCommentLines(extra_lines)
  
END SUBROUTINE


!-----------------------------------------------------------------------
! Prints to standard output a column header for the events and
! likelihoods data to follow.
! 
SUBROUTINE WriteEventsAndLikelihoodsColumnHeader()
  IMPLICIT NONE
  
  SELECT CASE (VerbosityLevel)
  CASE (:0)
    CONTINUE
  CASE (1:2)
    WRITE(*,'(A,1(1X,A8),3(1X,A11),2(1X,A11))') COMMENT_PREFIX,         &
        'observed','background ','signal(SI) ','signal(SD) ',           &
        '  log(L)   ','  log(p)   '
  CASE (3:)
    WRITE(*,'(A,1(1X,A10),4(1X,A10),4(1X,A10),1(1X,A8),3(1X,A11),2(1X,A11))') &
        COMMENT_PREFIX,'    mass  ',                                    &
        '   GpSI   ','   GnSI   ','   GpSD   ','   GnSD   ',            &
        ' sigmapSI ',' sigmanSI ',' sigmapSD ',' sigmanSD ',            &
        'observed','background ','signal(SI) ','signal(SD) ',           &
        '  log(L)   ','  log(p)   '
  END SELECT
  
END SUBROUTINE


!-----------------------------------------------------------------------
! Prints to standard output the events and likelihoods data for the
! current WIMP mass and couplings.
! 
SUBROUTINE WriteEventsAndLikelihoodsData()
  IMPLICIT NONE
  REAL*8 :: lnLike
  REAL*8 :: lnp
  
  ! Get log-likelihood and p-value
  lnLike = LogLikelihood()
  lnp    = LogPValue()
  
  ! Columns to print depend on the verbosity level.
  ! For data, only absolute value of verbosity is used.
  
  SELECT CASE (ABS(VerbosityLevel))
  CASE (:2)
    WRITE(*,'(A,1(2X,I5,2X),3(1X,1PG11.4),2(1X,1PG11.4))')              &
        DATA_PREFIX,                                                    &
        DefaultDetector%Nevents,DefaultDetector%MuBackground,           &
        DefaultDetector%MuSignalSI(0),DefaultDetector%MuSignalSD(0),    &
        lnLike,lnp
  CASE (3:)
    WRITE(*,'(A,1(1X,F10.3),4(1X,1PG10.3),4(1X,1PG10.3),1(2X,I5,2X),3(1X,1PG11.4),2(1X,1PG11.4))') &
        DATA_PREFIX,                                                    &
        WIMP%m,WIMP%GpSI,WIMP%GnSI,WIMP%GpSD,WIMP%GnSD,                 &
        GpToSigmapSI(WIMP%m,WIMP%GpSI),GnToSigmanSI(WIMP%m,WIMP%GnSI),  &
        GpToSigmapSD(WIMP%m,WIMP%GpSD),GnToSigmanSD(WIMP%m,WIMP%GnSD),  &
        DefaultDetector%Nevents,DefaultDetector%MuBackground,           &
        DefaultDetector%MuSignalSI(0),DefaultDetector%MuSignalSD(0),    &
        lnLike,lnp
  END SELECT
  
END SUBROUTINE


!-----------------------------------------------------------------------
! Prints to standard output a header containing information regarding
! the events and likelihoods data to follow.
! 
! Optional input arguments:
!   extra_lines     Blank lines (albeit with prefix) to add after
!                   output
! 
SUBROUTINE WriteSpectrumHeader(extra_lines)
  IMPLICIT NONE
  INTEGER, INTENT(IN), OPTIONAL :: extra_lines
  
  !WRITE(*,'(A)') COMMENT_LINE
  
  IF (VerbosityLevel .GE. 4) THEN
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'The table below gives differential rate components at reference'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'couplings that yield WIMP-nucleon scattering cross-sections of 1 pb.'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'Separate columns are given for the spectrum contribution arising from'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'the proton-proton [dRdEpp0], neutron-neutron [dRdEnn0], and proton-'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'neutron [dRdEpn0] components of the cross-section/coupling formula.'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'This allows rates for arbitrary couplings to be constructed as follows:'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '    dRdE = (sigmap/[pb])*dRdEpp0 + (sigman/[pb])*dRdEnn0'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '              +/- (sqrt{sigmap*sigman}/[pb])*dRdEpn0'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'where sigmap and sigman are the WIMP-nucleon scattering cross-sections'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'in pb.  The sign of the cross-term should be the same as the sign of'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'the product of couplings Gp*Gn.'
    !WRITE(*,'(A)') COMMENT_PREFIX &
    !    // ''
    WRITE(*,'(A)') COMMENT_PREFIX
  END IF
  
  IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'The columns below contain the following data:'
  END IF
  
  IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '  E            Recoil energy [keV].'
  END IF
  
  SELECT CASE (VerbosityLevel)
  CASE (2)
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '  dRdE         Differential recoil spectrum [cpd/kg/keV].'
  CASE (3)
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '  dRdE         Differential recoil spectrum [cpd/kg/keV].'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '               One column for each of the total, SI, and SD spectra.'
  CASE (4:)
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '  dRdEpp0,dRdEpn0,dRdEnn0'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '               Differential recoil spectrum components [cpd/kg/keV],'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '               given separately for SI and SD interactions.'
  END SELECT
  
  IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(A)') COMMENT_PREFIX
  END IF
  
  IF (PRESENT(extra_lines)) CALL WriteEmptyCommentLines(extra_lines)
  
END SUBROUTINE


!-----------------------------------------------------------------------
! Prints to standard output a column header for the tabulated
! differential rate spectrum to follow.
! 
SUBROUTINE WriteSpectrumColumnHeader()
  IMPLICIT NONE
  
  ! Columns to print depend on the verbosity level.
  ! For data, only absolute value of verbosity is used.
  
  SELECT CASE (ABS(VerbosityLevel))
  CASE (1)
    WRITE(*,'(A,1(1X,A10))',ADVANCE='NO') COMMENT_PREFIX,'  E [keV] '
    WRITE(*,'(1(1X,A12))',ADVANCE='NO')                                 &
        ' dR/dE [dru]'
  CASE (2)
    ! Differential rate
    WRITE(*,'(A,1(1X,A10))',ADVANCE='NO') COMMENT_PREFIX,'    E     '
    WRITE(*,'(1(1X,A12))',ADVANCE='NO')                                 &
        '   dR/dE    '
  CASE (3)
    ! Combined, si, sd
    WRITE(*,'(A,1(1X,A10))',ADVANCE='NO') COMMENT_PREFIX,'    E     '
    WRITE(*,'(3(1X,A12))',ADVANCE='NO')                                 &
        '   dR/dE    ',' dR/dE(SI)  ',' dR/dE(SD)  '
  CASE (4:)
    ! Reference rate components
    WRITE(*,'(A,1(1X,A10))',ADVANCE='NO') COMMENT_PREFIX,'    E     '
    WRITE(*,'(6(1X,A12))',ADVANCE='NO')                                 &
        'dRdEpp0(SI) ','dRdEpn0(SI) ','dRdEnn0(SI) ',                   &
        'dRdEpp0(SD) ','dRdEpn0(SD) ','dRdEnn0(SD) '
  END SELECT
  IF (VerbosityLevel .GE. 1) WRITE(*,'(A)') ''
  
END SUBROUTINE


!-----------------------------------------------------------------------
! Prints to standard output the tabulated differential rate spectrum
! for the current WIMP mass and couplings.
! 
SUBROUTINE WriteSpectrumData()
  IMPLICIT NONE
  INTEGER :: KE
  
  ! Columns to print depend on the verbosity level.
  ! For data, only absolute value of verbosity is used.
  
  DO KE = 1,DefaultDetector%NE
    WRITE(*,'(A,1(1X,F10.4))',ADVANCE='NO') DATA_PREFIX,                &
        DefaultDetector%E(KE)
    SELECT CASE (ABS(VerbosityLevel))
    CASE (:2)
      ! Differential rate
      WRITE(*,'(1(1X,1PG12.5))',ADVANCE='NO')                           &
          CoerceExponent(DefaultDetector%dRdE(KE),2,5)
    CASE (3)
      ! Combined, si, sd
      WRITE(*,'(3(1X,1PG12.5))',ADVANCE='NO')                           &
          CoerceExponent(DefaultDetector%dRdE(KE),2,5),                 &
          CoerceExponent(DefaultDetector%dRdEsi(KE),2,5),               &
          CoerceExponent(DefaultDetector%dRdEsd(KE),2,5)
    CASE (4:)
      ! Reference rate components
      WRITE(*,'(6(1X,1PG12.5))',ADVANCE='NO')                           &
          CoerceExponent(DefaultDetector%dRdEsi0(+1,KE),2,5),           &
          CoerceExponent(DefaultDetector%dRdEsi0( 0,KE),2,5),           &
          CoerceExponent(DefaultDetector%dRdEsi0(-1,KE),2,5),           &
          CoerceExponent(DefaultDetector%dRdEsd0(+1,KE),2,5),           &
          CoerceExponent(DefaultDetector%dRdEsd0( 0,KE),2,5),           &
          CoerceExponent(DefaultDetector%dRdEsd0(-1,KE),2,5)
    END SELECT
    WRITE(*,'(A)') ''
  END DO
  
END SUBROUTINE


!-----------------------------------------------------------------------
! Prints to standard output a header containing information regarding
! the events table (tabulated by mass) to follow.
! 
! Optional input arguments:
!   extra_lines     Blank lines (albeit with prefix) to add after
!                   output
! 
SUBROUTINE WriteEventsByMassHeader(extra_lines)
  IMPLICIT NONE
  INTEGER, INTENT(IN), OPTIONAL :: extra_lines
  REAL*8 :: sigmapSI,sigmanSI,sigmapSD,sigmanSD
  
  !WRITE(*,'(A)') COMMENT_LINE
  
  ! Will write out the fixed WIMP-nucleon cross-sections.
  ! Negative for negative couplings.
  CALL GetWIMP(sigmapSI=sigmapSI,sigmanSI=sigmanSI,sigmapSD=sigmapSD,sigmanSD=sigmanSD)
  IF (WIMP%GpSI .LT. 0d0) sigmapSI = -ABS(sigmapSI)
  IF (WIMP%GnSI .LT. 0d0) sigmanSI = -ABS(sigmanSI)
  IF (WIMP%GpSD .LT. 0d0) sigmapSD = -ABS(sigmapSD)
  IF (WIMP%GnSD .LT. 0d0) sigmanSD = -ABS(sigmanSD)
  
  ! Description and fixed cross-sections
  IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'The table below gives the expected spin-independent (SI) and spin-'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'dependent (SD) interaction events, tabulated by WIMP mass, for fixed'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'WIMP-nucleon cross-sections.  The fixed cross-sections are [pb]:'
    WRITE(*,'(A,1(2X,1PG12.4))') COMMENT_PREFIX &
        // '  sigmapSI (proton SI)  =',CoerceExponent(sigmapSI,2,4)
    WRITE(*,'(A,1(2X,1PG12.4))') COMMENT_PREFIX &
        // '  sigmanSI (neutron SI) =',CoerceExponent(sigmanSI,2,4)
    WRITE(*,'(A,1(2X,1PG12.4))') COMMENT_PREFIX &
        // '  sigmapSD (proton SD)  =',CoerceExponent(sigmapSD,2,4)
    WRITE(*,'(A,1(2X,1PG12.4))') COMMENT_PREFIX &
        // '  sigmanSD (neutron SD) =',CoerceExponent(sigmanSD,2,4)
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'A negative cross-section means the corresponding WIMP-nucleon coupling'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'will be taken to be negative.'
    !WRITE(*,'(A)') COMMENT_PREFIX &
    !    // ''
    WRITE(*,'(A)') COMMENT_PREFIX
  END IF
  
  IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'The columns below contain the following data:'
  END IF
  
  IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '  mass         WIMP mass [GeV].'
  END IF
  
  IF (VerbosityLevel .GE. 3) THEN
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '  G            WIMP-nucleon couplings for spin-independent (SI) and'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '               spin-dependent (SD) interactions [GeV^-2].'
  END IF
  
  IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '  events(SI)   Average expected spin-independent events.'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '  events(SD)   Average expected spin-dependent events.'
  END IF
  
  IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(A)') COMMENT_PREFIX
  END IF
  
  IF (PRESENT(extra_lines)) CALL WriteEmptyCommentLines(extra_lines)
  
END SUBROUTINE


!-----------------------------------------------------------------------
! Prints to standard output a column header for the table of expected
! events tabulated by mass.
! 
SUBROUTINE WriteEventsByMassColumnHeader()
  IMPLICIT NONE
  INTEGER :: Keff
  
  ! Columns to print depend on the verbosity level.
  ! For data, only absolute value of verbosity is used.
  
  ! Multiple lines in some cases
  IF (VerbosityLevel .GE. 3) THEN
    ! Mass column
    WRITE(*,'(A,1(1X,A10))',ADVANCE='NO') COMMENT_PREFIX,''
    ! Coupling columns
    IF (VerbosityLevel .GE. 4) THEN
      WRITE(*,'(4(1X,A10))',ADVANCE='NO') '','','',''
    END IF
    ! Events for full range
    WRITE(*,'(1X,A1)',ADVANCE='NO') ' '
    WRITE(*,'(1(1X,A23))',ADVANCE='NO')                                 &
        '----- full range ------'
    ! Events for sub-intervals
    DO Keff = 1,DefaultDetector%Neff
      WRITE(*,'(1X,A1)',ADVANCE='NO') '|'
      WRITE(*,'(1(1X,A14,I3,A6))',ADVANCE='NO')                         &
          '----- interval',Keff,' -----'
    END DO
    WRITE(*,'(A)') ''
  END IF
  
  ! Main column header line below
  
  ! Mass column
  IF (VerbosityLevel .EQ. 1) THEN
    WRITE(*,'(A,1(1X,A10))',ADVANCE='NO') COMMENT_PREFIX,'mass [GeV]'
  ELSE IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(A,1(1X,A10))',ADVANCE='NO') COMMENT_PREFIX,'    mass  '
  END IF
  
  ! Coupling columns
  IF (VerbosityLevel .GE. 4) THEN
    WRITE(*,'(4(1X,A10))',ADVANCE='NO')                                 &
        '   GpSI   ','   GnSI   ','   GpSD   ','   GnSD   '
  END IF
  
  ! Events for full range
  IF (VerbosityLevel .GE. 3) WRITE(*,'(1X,A1)',ADVANCE='NO') ' '
  IF (VerbosityLevel .GE. 1) THEN
    WRITE(*,'(2(1X,A11))',ADVANCE='NO') ' events(SI)',' events(SD)'
  END IF
  
  ! Events for sub-intervals
  IF (VerbosityLevel .GE. 3) THEN
    DO Keff = 1,DefaultDetector%Neff
      WRITE(*,'(1X,A1)',ADVANCE='NO') ' '
      WRITE(*,'(2(1X,A11))',ADVANCE='NO') ' events(SI)',' events(SD)'
    END DO
  END IF
  
  IF (VerbosityLevel .GE. 1) WRITE(*,'(A)') ''
  
END SUBROUTINE


!-----------------------------------------------------------------------
! Prints to standard output the mass and expected events for the
! current WIMP.  Used for tabulation of events by WIMP mass.
! 
SUBROUTINE WriteEventsByMassData()
  IMPLICIT NONE
  INTEGER :: Keff
  
  ! Columns to print depend on the verbosity level.
  ! For data, only absolute value of verbosity is used.
  
  ! Mass
  WRITE(*,'(A,1(1X,F10.4))',ADVANCE='NO') DATA_PREFIX,                  &
      CoerceNumber(WIMP%m,10,4)
  
  ! Couplings
  IF (ABS(VerbosityLevel) .GE. 4) THEN
    WRITE(*,'(4(1X,1PG10.3))',ADVANCE='NO')                             &
        CoerceExponent(WIMP%GpSI,2,3),CoerceExponent(WIMP%GnSI,2,3),    &
        CoerceExponent(WIMP%GpSD,2,3),CoerceExponent(WIMP%GnSD,2,3)
  END IF
  
  ! Events (full range)
  IF (ABS(VerbosityLevel) .GE. 3) WRITE(*,'(1X,A1)',ADVANCE='NO') ' '
  WRITE(*,'(2(1X,1PG11.4))',ADVANCE='NO')                               &
      CoerceExponent(DefaultDetector%MuSignalSI(0),2,4),                &
      CoerceExponent(DefaultDetector%MuSignalSD(0),2,4)
  
  ! Events for sub-intervals
  IF (ABS(VerbosityLevel) .GE. 3) THEN
    DO Keff = 1,DefaultDetector%Neff
      WRITE(*,'(1X,A1)',ADVANCE='NO') ' '
      WRITE(*,'(2(1X,1PG11.4))',ADVANCE='NO')                           &
          CoerceExponent(DefaultDetector%MuSignalSI(Keff),2,4),         &
          CoerceExponent(DefaultDetector%MuSignalSD(Keff),2,4)
    END DO
  END IF
  
  WRITE(*,'(A)') ''
  
END SUBROUTINE


!-----------------------------------------------------------------------
! Prints to standard output a header containing information regarding
! the SI cross-section constraints table (tabulated by mass) to follow.
! 
! Required input arguments:
!   lnp             The logarithm of the p-value to use for the
!                   constraint CL.
!   thetaG          The angle of (Gp,Gn) that is fixed for each
!                   constraint determination.
!   s1,s2           The range of allowed signal expectation values.
! Optional input arguments:
!   extra_lines     Blank lines (albeit with prefix) to add after
!                   output
! 
SUBROUTINE WriteConstraintsSIHeader(lnp,thetaG,s1,s2,extra_lines)
  IMPLICIT NONE
  REAL*8, INTENT(IN) :: lnp,thetaG,s1,s2
  INTEGER, INTENT(IN), OPTIONAL :: extra_lines
  
  !WRITE(*,'(A)') COMMENT_LINE
  
  ! Description
  IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'The table below gives the range of allowed spin-independent (SI) cross-'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'sections.  The allowed cross-sections are those that predict a mean'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'signal compatible with the observed number of events and estimated'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'background.  The confidence interval (CI) for the mean signal at the'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'given confidence level CL is determined using a Poisson distribution'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'with Feldman-Cousins ordering; see Feldman & Cousins, Phys. Rev. D 57,'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '3873 (1998) [physics/9711021].'
    WRITE(*,'(A,1(2X,1PG12.4))') COMMENT_PREFIX &
        // '  1-CL                  =',CoerceExponent(EXP(lnp),2,4)
    WRITE(*,'(A,1(2X,1PG12.4),2X,A,1(2X,1PG12.4))') COMMENT_PREFIX &
        // '  signal events CI      =',CoerceExponent(s1,2,4),'-',CoerceExponent(s2,2,4)
    WRITE(*,'(A)') COMMENT_PREFIX
    
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'The constraints are determined using a fixed ratio of the two WIMP-nucleon'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'couplings Gp & Gn, described by the angle theta s.t. tan(theta) = Gn/Gp.'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'The conventional isospin-invariant case Gn=Gp corresponds to theta=PI/4.'
    WRITE(*,'(A,1(2X,F9.5))') COMMENT_PREFIX &
        // '  theta/PI              =',CoerceNumber(thetaG/PI,10,5)
    WRITE(*,'(A)') COMMENT_PREFIX
  END IF
  
  IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'The columns below contain the following data:'
  END IF
  
  IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '  mass         WIMP mass [GeV].'
  END IF
  
  IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '  sigmapSI     The WIMP-proton spin-independent cross-section lower and'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '               upper limits [pb].'
  END IF
  
  IF (VerbosityLevel .GE. 3) THEN
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '  sigmanSI     The WIMP-neutron spin-independent cross-section lower and'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '               upper limits [pb].'
  END IF
  
  IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(A)') COMMENT_PREFIX
  END IF
  
  IF (PRESENT(extra_lines)) CALL WriteEmptyCommentLines(extra_lines)
  
END SUBROUTINE


!-----------------------------------------------------------------------
! Prints to standard output a column header for the table of SI cross-
! section constraints tabulated by mass.
! 
SUBROUTINE WriteConstraintsSIColumnHeader()
  IMPLICIT NONE
  
  ! Columns to print depend on the verbosity level.
  ! For data, only absolute value of verbosity is used.
  
  ! Mass column
  IF (VerbosityLevel .EQ. 1) THEN
    WRITE(*,'(A,1(1X,A10))',ADVANCE='NO') COMMENT_PREFIX,'mass [GeV]'
  ELSE IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(A,1(1X,A10))',ADVANCE='NO') COMMENT_PREFIX,'    mass  '
  END IF
  
  ! WIMP-proton cross-section columns
  IF (VerbosityLevel .EQ. 1) THEN
    WRITE(*,'(1(1X,A25))',ADVANCE='NO') ' --sigmapSI range [pb]-- '
  ELSE IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(1(1X,A25))',ADVANCE='NO') ' --- sigmapSI range ---- '
  END IF
  
  ! WIMP-neutron cross-section column
  IF (VerbosityLevel .GE. 3) THEN
    WRITE(*,'(1(1X,A25))',ADVANCE='NO') ' --- sigmanSI range ---- '
  END IF
  
  IF (VerbosityLevel .GE. 1) WRITE(*,'(A)') ''
  
END SUBROUTINE


!-----------------------------------------------------------------------
! Prints to standard output the mass and SI cross-section limit for the
! current WIMP.  Used for tabulation of limits by WIMP mass.
! 
! Input argument:
!   s1,s2       The range of allowed signal expectation values.
! 
SUBROUTINE WriteConstraintsSIData(s1,s2)
  IMPLICIT NONE
  REAL*8, INTENT(IN) :: s1,s2
  REAL*8 :: mu,x1,x2,sigmapSI,sigmanSI
  
  ! Need scale factors x s.t. sigma -> x*sigma gives desired
  ! number of events.
  CALL GetRates(signal_si=mu)
  ! Empty set case
  IF (s2 .EQ. 0d0) THEN
    x1 = 0d0
    x2 = 0d0
  ! General case
  ELSE IF (mu .GT. 0d0) THEN
    x1 = s1/mu
    x2 = s2/mu
  ! No events case (at any scale)
  ELSE
    x1 = 0d0
    x2 = HUGE(1d0)
  END IF
  
  ! Cross-sections (multiply by x for limit)
  CALL GetWIMP(sigmapSI=sigmapSI,sigmanSI=sigmanSI)
  
  ! Columns to print depend on the verbosity level.
  ! For data, only absolute value of verbosity is used.
  
  ! Mass
  WRITE(*,'(A,1(1X,F10.4))',ADVANCE='NO') DATA_PREFIX,                  &
      CoerceNumber(WIMP%m,10,4)
  
  ! WIMP-proton cross-section
  WRITE(*,'(2(1X,1PG12.5))',ADVANCE='NO')                               &
      CoerceExponent(x1*sigmapSI,2,5),CoerceExponent(x2*sigmapSI,2,5)
  
  ! WIMP-neutron cross-section
  IF (ABS(VerbosityLevel) .GE. 3) THEN
    WRITE(*,'(2(1X,1PG12.5))',ADVANCE='NO')                             &
        CoerceExponent(x1*sigmanSI,2,5),CoerceExponent(x2*sigmanSI,2,5)
  END IF
  
  WRITE(*,'(A)') ''
  
END SUBROUTINE


!-----------------------------------------------------------------------
! Prints to standard output a header containing information regarding
! the SD cross-section constraints table (tabulated by mass) to follow.
! 
! Required input arguments:
!   lnp             The logarithm of the p-value to use for the
!                   constraint CL.
!   thetaG          The angle of (Gp,Gn) that is fixed for each
!                   constraint determination.
!   s1,s2           The range of allowed signal expectation values.
! Optional input arguments:
!   extra_lines     Blank lines (albeit with prefix) to add after
!                   output
! 
SUBROUTINE WriteConstraintsSDHeader(lnp,thetaG,s1,s2,extra_lines)
  IMPLICIT NONE
  REAL*8, INTENT(IN) :: lnp,thetaG,s1,s2
  INTEGER, INTENT(IN), OPTIONAL :: extra_lines
  
  !WRITE(*,'(A)') COMMENT_LINE
  
  ! Description
  IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'The table below gives the range of allowed spin-dependent (SD) cross-'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'sections.  The allowed cross-sections are those that predict a mean'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'signal compatible with the observed number of events and estimated'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'background.  The confidence interval (CI) for the mean signal at the'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'given confidence level CL is determined using a Poisson distribution'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'with Feldman-Cousins ordering; see Feldman & Cousins, Phys. Rev. D 57,'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '3873 (1998) [physics/9711021].'
    WRITE(*,'(A,1(2X,1PG12.4))') COMMENT_PREFIX &
        // '  1-CL                  =',CoerceExponent(EXP(lnp),2,4)
    WRITE(*,'(A,1(2X,1PG12.4),2X,A,1(2X,1PG12.4))') COMMENT_PREFIX &
        // '  signal events CI      =',CoerceExponent(s1,2,4),'-',CoerceExponent(s2,2,4)
    WRITE(*,'(A)') COMMENT_PREFIX
    
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'The constraints are determined using a fixed ratio of the two WIMP-nucleon'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'couplings Gp & Gn, described by the angle theta s.t. tan(theta) = Gn/Gp.'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'The isospin-invariant case Gn=Gp corresponds to theta=PI/4, proton-only'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'coupling is theta=0, and neutron-only coupling is theta=PI/2.'
    WRITE(*,'(A,1(2X,F9.5))') COMMENT_PREFIX &
        // '  theta/PI              =',CoerceNumber(thetaG/PI,10,5)
    WRITE(*,'(A)') COMMENT_PREFIX
  END IF
  
  IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'The columns below contain the following data:'
  END IF
  
  IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '  mass         WIMP mass [GeV].'
  END IF
  
  IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '  sigmapSD     The WIMP-proton spin-dependent cross-section lower and'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '               upper limits [pb].'
  END IF
  
  IF (VerbosityLevel .GE. 3) THEN
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '  sigmanSD     The WIMP-neutron spin-dependent cross-section lower and'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '               upper limits [pb].'
  END IF
  
  IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(A)') COMMENT_PREFIX
  END IF
  
  IF (PRESENT(extra_lines)) CALL WriteEmptyCommentLines(extra_lines)
  
END SUBROUTINE


!-----------------------------------------------------------------------
! Prints to standard output a column header for the table of SD cross-
! section constraints tabulated by mass.
! 
SUBROUTINE WriteConstraintsSDColumnHeader()
  IMPLICIT NONE
  
  ! Columns to print depend on the verbosity level.
  ! For data, only absolute value of verbosity is used.
  
  ! Mass column
  IF (VerbosityLevel .EQ. 1) THEN
    WRITE(*,'(A,1(1X,A10))',ADVANCE='NO') COMMENT_PREFIX,'mass [GeV]'
  ELSE IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(A,1(1X,A10))',ADVANCE='NO') COMMENT_PREFIX,'    mass  '
  END IF
  
  ! WIMP-proton cross-section columns
  IF (VerbosityLevel .EQ. 1) THEN
    WRITE(*,'(1(1X,A25))',ADVANCE='NO') ' --sigmapSD range [pb]-- '
  ELSE IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(1(1X,A25))',ADVANCE='NO') ' --- sigmapSD range ---- '
  END IF
  
  ! WIMP-neutron cross-section column
  IF (VerbosityLevel .GE. 3) THEN
    WRITE(*,'(1(1X,A25))',ADVANCE='NO') ' --- sigmanSD range ---- '
  END IF
  
  IF (VerbosityLevel .GE. 1) WRITE(*,'(A)') ''
  
END SUBROUTINE


!-----------------------------------------------------------------------
! Prints to standard output the mass and SD cross-section limit for the
! current WIMP.  Used for tabulation of limits by WIMP mass.
! 
! Input argument:
!   s1,s2       The range of allowed signal expectation values.
! 
SUBROUTINE WriteConstraintsSDData(s1,s2)
  IMPLICIT NONE
  REAL*8, INTENT(IN) :: s1,s2
  REAL*8 :: mu,x1,x2,sigmapSD,sigmanSD
  
  ! Need scale factors x s.t. sigma -> x*sigma gives desired
  ! number of events.
  CALL GetRates(signal_sd=mu)
  ! Empty set case
  IF (s2 .EQ. 0d0) THEN
    x1 = 0d0
    x2 = 0d0
  ! General case
  ELSE IF (mu .GT. 0d0) THEN
    x1 = s1/mu
    x2 = s2/mu
  ! No events case (at any scale)
  ELSE
    x1 = 0d0
    x2 = HUGE(1d0)
  END IF
  
  ! Cross-sections (multiply by x for limit)
  CALL GetWIMP(sigmapSD=sigmapSD,sigmanSD=sigmanSD)
  
  ! Columns to print depend on the verbosity level.
  ! For data, only absolute value of verbosity is used.
  
  ! Mass
  WRITE(*,'(A,1(1X,F10.4))',ADVANCE='NO') DATA_PREFIX,                  &
      CoerceNumber(WIMP%m,10,4)
  
  ! WIMP-proton cross-section
  WRITE(*,'(2(1X,1PG12.5))',ADVANCE='NO')                               &
      CoerceExponent(x1*sigmapSD,2,5),CoerceExponent(x2*sigmapSD,2,5)
  
  ! WIMP-neutron cross-section
  IF (ABS(VerbosityLevel) .GE. 3) THEN
    WRITE(*,'(2(1X,1PG12.5))',ADVANCE='NO')                             &
        CoerceExponent(x1*sigmanSD,2,5),CoerceExponent(x2*sigmanSD,2,5)
  END IF
  
  WRITE(*,'(A)') ''
  
END SUBROUTINE


!-----------------------------------------------------------------------
! Prints to standard output a header containing information regarding
! the SI cross-section limits table (tabulated by mass) to follow.
! 
! Required input arguments:
!   lnp             The logarithm of the p-value to use for the limit
!                   CL.
!   thetaG          The angle of (Gp,Gn) that is fixed for each limit
!                   determination.
! Optional input arguments:
!   extra_lines     Blank lines (albeit with prefix) to add after
!                   output
! 
SUBROUTINE WriteLimitsSIHeader(lnp,thetaG,extra_lines)
  IMPLICIT NONE
  REAL*8, INTENT(IN) :: lnp,thetaG
  INTEGER, INTENT(IN), OPTIONAL :: extra_lines
  
  !WRITE(*,'(A)') COMMENT_LINE
  
  ! Description
  IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'The table below gives the upper limit on spin-independent (SI) cross-'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'section(s) that are not excluded.  Cross-sections are excluded if their'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'p-value is smaller than the given p-value, where the p-value is'
    IF (DefaultDetector%Neff .EQ. DefaultDetector%Nevents+1) THEN
      WRITE(*,'(A)') COMMENT_PREFIX &
        // 'determined using the maximum gap method; see Yellin, Phys. Rev. D 66,'
      WRITE(*,'(A)') COMMENT_PREFIX &
        // '032005 (2002) [physics/0203002].'
    ELSE
      WRITE(*,'(A)') COMMENT_PREFIX &
        // 'determined using the Poisson distribution (signal only: no background'
      WRITE(*,'(A)') COMMENT_PREFIX &
        // ').'
    END IF
    WRITE(*,'(A,1(2X,1PG12.4))') COMMENT_PREFIX &
        // '  p-value               =',CoerceExponent(EXP(lnp),2,4)
    WRITE(*,'(A)') COMMENT_PREFIX
    
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'The limit is determined using a fixed ratio of the two WIMP-nucleon'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'couplings Gp & Gn, described by the angle theta s.t. tan(theta) = Gn/Gp.'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'The conventional isospin-invariant case Gn=Gp corresponds to theta=PI/4.'
    WRITE(*,'(A,1(2X,F9.5))') COMMENT_PREFIX &
        // '  theta/PI              =',CoerceNumber(thetaG/PI,10,5)
    WRITE(*,'(A)') COMMENT_PREFIX
  END IF
  
  IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'The columns below contain the following data:'
  END IF
  
  IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '  mass         WIMP mass [GeV].'
  END IF
  
  IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '  sigmapSI     The WIMP-proton spin-independent cross-section upper limit [pb].'
  END IF
  
  IF (VerbosityLevel .GE. 3) THEN
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '  sigmanSI     The WIMP-neutron spin-independent cross-section upper limit [pb].'
  END IF
  
  IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(A)') COMMENT_PREFIX
  END IF
  
  IF (PRESENT(extra_lines)) CALL WriteEmptyCommentLines(extra_lines)
  
END SUBROUTINE


!-----------------------------------------------------------------------
! Prints to standard output a column header for the table of SI cross-
! section limits tabulated by mass.
! 
SUBROUTINE WriteLimitsSIColumnHeader()
  IMPLICIT NONE
  
  ! Columns to print depend on the verbosity level.
  ! For data, only absolute value of verbosity is used.
  
  ! Mass column
  IF (VerbosityLevel .EQ. 1) THEN
    WRITE(*,'(A,1(1X,A10))',ADVANCE='NO') COMMENT_PREFIX,'mass [GeV]'
  ELSE IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(A,1(1X,A10))',ADVANCE='NO') COMMENT_PREFIX,'    mass  '
  END IF
  
  ! WIMP-proton cross-section column
  IF (VerbosityLevel .EQ. 1) THEN
    WRITE(*,'(1(1X,A12))',ADVANCE='NO') 'sigmapSI[pb]'
  ELSE IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(1(1X,A12))',ADVANCE='NO') '  sigmapSI  '
  END IF
  
  ! WIMP-neutron cross-section column
  IF (VerbosityLevel .GE. 3) THEN
    WRITE(*,'(1(1X,A12))',ADVANCE='NO') '  sigmanSI  '
  END IF
  
  IF (VerbosityLevel .GE. 1) WRITE(*,'(A)') ''
  
END SUBROUTINE


!-----------------------------------------------------------------------
! Prints to standard output the mass and SI cross-section limit for the
! current WIMP.  Used for tabulation of limits by WIMP mass.
! 
! Input argument:
!   lnp         Logarithm of the p-value for the limit CL
! 
SUBROUTINE WriteLimitsSIData(lnp)
  IMPLICIT NONE
  REAL*8, INTENT(IN) :: lnp
  REAL*8 :: x,sigmapSI,sigmanSI
  
  ! Need scale factor
  x = ScaleToPValue(lnp=lnp)
  
  ! Cross-sections (multiply by x for limit)
  CALL GetWIMP(sigmapSI=sigmapSI,sigmanSI=sigmanSI)
  
  ! Columns to print depend on the verbosity level.
  ! For data, only absolute value of verbosity is used.
  
  ! Mass
  WRITE(*,'(A,1(1X,F10.4))',ADVANCE='NO') DATA_PREFIX,                  &
      CoerceNumber(WIMP%m,10,4)
  
  ! WIMP-proton cross-section
  WRITE(*,'(1(1X,1PG12.5))',ADVANCE='NO')                               &
      CoerceExponent(x*sigmapSI,2,5)
  
  ! WIMP-neutron cross-section
  IF (ABS(VerbosityLevel) .GE. 3) THEN
    WRITE(*,'(1(1X,1PG12.5))',ADVANCE='NO')                             &
        CoerceExponent(x*sigmanSI,2,5)
  END IF
  
  WRITE(*,'(A)') ''
  
END SUBROUTINE


!-----------------------------------------------------------------------
! Prints to standard output a header containing information regarding
! the SD cross-section limits table (tabulated by mass) to follow.
! 
! Required input arguments:
!   lnp             The logarithm of the p-value to use for the limit
!                   CL.
!   thetaG          The angle of (Gp,Gn) that is fixed for each limit
!                   determination.
! Optional input arguments:
!   extra_lines     Blank lines (albeit with prefix) to add after
!                   output
! 
SUBROUTINE WriteLimitsSDHeader(lnp,thetaG,extra_lines)
  IMPLICIT NONE
  REAL*8, INTENT(IN) :: lnp,thetaG
  INTEGER, INTENT(IN), OPTIONAL :: extra_lines
  
  !WRITE(*,'(A)') COMMENT_LINE
  
  ! Description
  IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'The table below gives the upper limit on spin-dependent (SD) cross-'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'section(s) that are not excluded.  Cross-sections are excluded if their'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'p-value is smaller than the given p-value, where the p-value is'
    IF (DefaultDetector%Neff .EQ. DefaultDetector%Nevents+1) THEN
      WRITE(*,'(A)') COMMENT_PREFIX &
        // 'determined using the maximum gap method; see Yellin, Phys. Rev. D 66,'
      WRITE(*,'(A)') COMMENT_PREFIX &
        // '032005 (2002) [physics/0203002].'
    ELSE
      WRITE(*,'(A)') COMMENT_PREFIX &
        // 'determined using the Poisson distribution (signal only: no background'
      WRITE(*,'(A)') COMMENT_PREFIX &
        // ').'
    END IF
    WRITE(*,'(A,1(2X,1PG12.4))') COMMENT_PREFIX &
        // '  p-value               =',CoerceExponent(EXP(lnp),2,4)
    WRITE(*,'(A)') COMMENT_PREFIX
    
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'The limit is determined using a fixed ratio of the two WIMP-nucleon'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'couplings Gp & Gn, described by the angle theta s.t. tan(theta) = Gn/Gp.'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'The isospin-invariant case Gn=Gp corresponds to theta=PI/4, proton-only'
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'coupling is theta=0, and neutron-only coupling is theta=PI/2.'
    WRITE(*,'(A,1(2X,F9.5))') COMMENT_PREFIX &
        // '  theta/PI              =',CoerceNumber(thetaG/PI,10,5)
    WRITE(*,'(A)') COMMENT_PREFIX
    
    !WRITE(*,'(A)') COMMENT_PREFIX &
    !    // 'NOTE: SPIN-DEPENDENT FORM FACTORS NOT IMPLEMENTED.'
    !WRITE(*,'(A)') COMMENT_PREFIX
  END IF
  
  IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(A)') COMMENT_PREFIX &
        // 'The columns below contain the following data:'
  END IF
  
  IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '  mass         WIMP mass [GeV].'
  END IF
  
  IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '  sigmapSD     The WIMP-proton spin-dependent cross-section upper limit [pb].'
  END IF
  
  IF (VerbosityLevel .GE. 3) THEN
    WRITE(*,'(A)') COMMENT_PREFIX &
        // '  sigmanSD     The WIMP-neutron spin-dependent cross-section upper limit [pb].'
  END IF
  
  IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(A)') COMMENT_PREFIX
  END IF
  
  IF (PRESENT(extra_lines)) CALL WriteEmptyCommentLines(extra_lines)
  
END SUBROUTINE


!-----------------------------------------------------------------------
! Prints to standard output a column header for the table of SD cross-
! section limits tabulated by mass.
! 
SUBROUTINE WriteLimitsSDColumnHeader()
  IMPLICIT NONE
  
  ! Columns to print depend on the verbosity level.
  ! For data, only absolute value of verbosity is used.
  
  ! Mass column
  IF (VerbosityLevel .EQ. 1) THEN
    WRITE(*,'(A,1(1X,A10))',ADVANCE='NO') COMMENT_PREFIX,'mass [GeV]'
  ELSE IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(A,1(1X,A10))',ADVANCE='NO') COMMENT_PREFIX,'    mass  '
  END IF
  
  ! WIMP-proton cross-section column
  IF (VerbosityLevel .EQ. 1) THEN
    WRITE(*,'(1(1X,A12))',ADVANCE='NO') 'sigmapSD[pb]'
  ELSE IF (VerbosityLevel .GE. 2) THEN
    WRITE(*,'(1(1X,A12))',ADVANCE='NO') '  sigmapSD  '
  END IF
  
  ! WIMP-neutron cross-section column
  IF (VerbosityLevel .GE. 3) THEN
    WRITE(*,'(1(1X,A12))',ADVANCE='NO') '  sigmanSD  '
  END IF
  
  IF (VerbosityLevel .GE. 1) WRITE(*,'(A)') ''
  
END SUBROUTINE


!-----------------------------------------------------------------------
! Prints to standard output the mass and SD cross-section limit for the
! current WIMP.  Used for tabulation of limits by WIMP mass.
! 
! Input argument:
!   lnp         Logarithm of the p-value for the limit CL
! 
SUBROUTINE WriteLimitsSDData(lnp)
  IMPLICIT NONE
  REAL*8, INTENT(IN) :: lnp
  REAL*8 :: x,sigmapSD,sigmanSD
  
  ! Need scale factor
  x = ScaleToPValue(lnp=lnp)
  
  ! Cross-sections (multiply by x for limit)
  CALL GetWIMP(sigmapSD=sigmapSD,sigmanSD=sigmanSD)
  
  ! Columns to print depend on the verbosity level.
  ! For data, only absolute value of verbosity is used.
  
  ! Mass
  WRITE(*,'(A,1(1X,F10.4))',ADVANCE='NO') DATA_PREFIX,                  &
      CoerceNumber(WIMP%m,10,4)
  
  ! WIMP-proton cross-section
  WRITE(*,'(1(1X,1PG12.5))',ADVANCE='NO')                               &
      CoerceExponent(x*sigmapSD,2,5)
  
  ! WIMP-neutron cross-section
  IF (ABS(VerbosityLevel) .GE. 3) THEN
    WRITE(*,'(1(1X,1PG12.5))',ADVANCE='NO')                             &
        CoerceExponent(x*sigmanSD,2,5)
  END IF
  
  WRITE(*,'(A)') ''
  
END SUBROUTINE



!=======================================================================
! WIMP ROUTINES
!=======================================================================

! ----------------------------------------------------------------------
! Get various WIMP quantities.
! 
! Optional output arguments:
!   m           WIMP mass [GeV].
!   GpSI        Spin-independent WIMP-proton coupling [GeV^-2].
!   GnSI        Spin-independent WIMP-neutron coupling [GeV^-2].
!   GpSD        Spin-dependent WIMP-proton coupling [GeV^-2].
!   GnSD        Spin-dependent WIMP-neutron coupling [GeV^-2].
!   GpSI0       Reference spin-independent WIMP-proton coupling [GeV^-2]
!               corresponding to \sigma_{SI,p} = 1 pb.
!   GnSI0       Reference spin-independent WIMP-neutron coupling [GeV^-2]
!               corresponding to \sigma_{SI,n} = 1 pb.
!   GpSD0       Reference spin-dependent WIMP-proton coupling [GeV^-2]
!               corresponding to \sigma_{SD,p} = 1 pb.
!   GnSD0       Reference spin-dependent WIMP-neutron coupling [GeV^-2]
!               corresponding to \sigma_{SD,n} = 1 pb.
!   fp          Spin-independent WIMP-proton coupling [GeV^-2].
!               Related by GpSI = 2 fp.
!   fn          Spin-independent WIMP-neutron coupling [GeV^-2].
!               Related by GnSI = 2 fn.
!   ap          Spin-dependent WIMP-proton coupling [unitless].
!               Related by GpSD = 2\sqrt{2} G_F ap.
!   an          Spin-dependent WIMP-neutron coupling [unitless].
!               Related by GnSD = 2\sqrt{2} G_F an.
!   sigmapSI    Spin-independent WIMP-proton cross-section [pb].
!   sigmanSI    Spin-independent WIMP-neutron cross-section [pb].
!   sigmapSD    Spin-dependent WIMP-proton cross-section [pb].
!   sigmanSD    Spin-dependent WIMP-neutron cross-section [pb].
! 
SUBROUTINE GetWIMP(m,GpSI,GnSI,GpSD,GnSD,GpSI0,GnSI0,GpSD0,GnSD0,       &
                   fp,fn,ap,an,sigmapSI,sigmanSI,sigmapSD,sigmanSD)
  IMPLICIT NONE
  REAL*8, INTENT(OUT), OPTIONAL :: m,GpSI,GnSI,GpSD,GnSD,GpSI0,GnSI0,GpSD0,GnSD0, &
           fp,fn,ap,an,sigmapSI,sigmanSI,sigmapSD,sigmanSD
  IF (PRESENT(m))     m     = WIMP%m
  IF (PRESENT(GpSI))  GpSI  = WIMP%GpSI
  IF (PRESENT(GnSI))  GnSI  = WIMP%GnSI
  IF (PRESENT(GpSD))  GpSD  = WIMP%GpSD
  IF (PRESENT(GnSD))  GnSD  = WIMP%GnSD
  IF (PRESENT(GpSI0)) GpSI0 = WIMP%GpSI0
  IF (PRESENT(GnSI0)) GnSI0 = WIMP%GnSI0
  IF (PRESENT(GpSD0)) GpSD0 = WIMP%GpSD0
  IF (PRESENT(GnSD0)) GnSD0 = WIMP%GnSD0
  IF (PRESENT(fp))    fp    = GToF(WIMP%GpSI)
  IF (PRESENT(fn))    fn    = GToF(WIMP%GnSI)
  IF (PRESENT(ap))    ap    = GToA(WIMP%GpSD)
  IF (PRESENT(an))    an    = GToA(WIMP%GnSD)
  IF (PRESENT(sigmapSI)) sigmapSI = GpToSigmapSI(WIMP%m,WIMP%GpSI)
  IF (PRESENT(sigmanSI)) sigmanSI = GnToSigmanSI(WIMP%m,WIMP%GnSI)
  IF (PRESENT(sigmapSD)) sigmapSD = GpToSigmapSD(WIMP%m,WIMP%GpSD)
  IF (PRESENT(sigmanSD)) sigmanSD = GnToSigmanSD(WIMP%m,WIMP%GnSD)
END SUBROUTINE


! ----------------------------------------------------------------------
! Set various WIMP quantities.
! 
! Optional input arguments:
!   m           WIMP mass [GeV].
!   GpSI        Spin-independent WIMP-proton coupling [GeV^-2].
!   GnSI        Spin-independent WIMP-neutron coupling [GeV^-2].
!   GpSD        Spin-dependent WIMP-proton coupling [GeV^-2].
!   GnSD        Spin-dependent WIMP-neutron coupling [GeV^-2].
!   fp          Spin-independent WIMP-proton coupling [GeV^-2].
!               Related by GpSI = 2 fp.
!   fn          Spin-independent WIMP-neutron coupling [GeV^-2].
!               Related by GnSI = 2 fn.
!   ap          Spin-dependent WIMP-proton coupling [unitless].
!               Related by GpSD = 2\sqrt{2} G_F ap.
!   an          Spin-dependent WIMP-neutron coupling [unitless].
!               Related by GnSD = 2\sqrt{2} G_F an.
! Optional cross-section arguments (give negative value to set
! corresponding coupling negative):
!   sigmapSI    Spin-independent WIMP-proton cross-section [pb].
!   sigmanSI    Spin-independent WIMP-neutron cross-section [pb].
!   sigmapSD    Spin-dependent WIMP-proton cross-section [pb].
!   sigmanSD    Spin-dependent WIMP-neutron cross-section [pb].
!   sigmaSI     Sets both sigmapSI and sigmanSI to the given value [pb].
!   sigmaSD     Sets both sigmapSD and sigmanSD to the given value [pb].
! 
SUBROUTINE SetWIMP(m,GpSI,GnSI,GpSD,GnSD,fp,fn,ap,an,                   &
                   sigmapSI,sigmanSI,sigmapSD,sigmanSD,sigmaSI,sigmaSD)
  IMPLICIT NONE
  REAL*8, INTENT(IN), OPTIONAL :: m,GpSI,GnSI,GpSD,GnSD,fp,fn,ap,an,    &
           sigmapSI,sigmanSI,sigmapSD,sigmanSD,sigmaSI,sigmaSD
  IF (PRESENT(m)) THEN
     WIMP%m = MAX(m,SQRT(TINY(1d0)))
     WIMP%GpSI0 = SigmapSIToGp(WIMP%m,1d0)
     WIMP%GnSI0 = SigmanSIToGn(WIMP%m,1d0)
     WIMP%GpSD0 = SigmapSDToGp(WIMP%m,1d0)
     WIMP%GnSD0 = SigmanSDToGn(WIMP%m,1d0)
  END IF
  IF (PRESENT(GpSI))  WIMP%GpSI = GpSI
  IF (PRESENT(GnSI))  WIMP%GnSI = GnSI
  IF (PRESENT(GpSD))  WIMP%GpSD = GpSD
  IF (PRESENT(GnSD))  WIMP%GnSD = GnSD
  IF (PRESENT(fp))    WIMP%GpSI = FToG(fp)
  IF (PRESENT(fn))    WIMP%GnSI = FToG(fn)
  IF (PRESENT(ap))    WIMP%GpSD = AToG(ap)
  IF (PRESENT(an))    WIMP%GnSD = AToG(an)
  IF (PRESENT(sigmapSI)) WIMP%GpSI = SigmapSIToGp(WIMP%m,sigmapSI)
  IF (PRESENT(sigmanSI)) WIMP%GnSI = SigmanSIToGn(WIMP%m,sigmanSI)
  IF (PRESENT(sigmapSD)) WIMP%GpSD = SigmapSDToGp(WIMP%m,sigmapSD)
  IF (PRESENT(sigmanSD)) WIMP%GnSD = SigmanSDToGn(WIMP%m,sigmanSD)
  IF (PRESENT(sigmaSI)) THEN
    WIMP%GpSI = SigmapSIToGp(WIMP%m,sigmaSI)
    WIMP%GnSI = SigmanSIToGn(WIMP%m,sigmaSI)
  END IF
  IF (PRESENT(sigmaSD)) THEN
    WIMP%GpSD = SigmapSDToGp(WIMP%m,sigmaSD)
    WIMP%GnSD = SigmanSDToGn(WIMP%m,sigmaSD)
  END IF
END SUBROUTINE


!-----------------------------------------------------------------------
! Initializes WIMP.
! Simply sets some default values for the WIMP parameters.
! 
SUBROUTINE InitWIMP()
  IMPLICIT NONE
  
  ! Default mass of 100 GeV
  CALL SetWIMP(m=100d0)
  
  ! Default cross-sections of 1 pb.
  WIMP%GpSI = WIMP%GpSI0
  WIMP%GnSI = WIMP%GnSI0
  WIMP%GpSD = WIMP%GpSD0
  WIMP%GnSD = WIMP%GnSD0
  
END SUBROUTINE


!-----------------------------------------------------------------------
! Initializes WIMP from command-line parameters.
! 
! Possible options:
!   --m=<value>          ! WIMP mass [GeV]
!   --GpSI=<value>       ! Spin-independent WIMP-proton coupling [GeV^-2].
!   --GnSI=<value>       ! Spin-independent WIMP-neutron coupling [GeV^-2].
!   --GpSD=<value>       ! Spin-dependent WIMP-proton coupling [GeV^-2].
!   --GnSD=<value>       ! Spin-dependent WIMP-neutron coupling [GeV^-2].
!   --fp=<value>         ! Spin-independent WIMP-proton coupling [GeV^-2].
!                        ! Related by GpSI = 2 fp.
!   --fn=<value>         ! Spin-independent WIMP-neutron coupling [GeV^-2].
!                        ! Related by GnSI = 2 fn.
!   --ap=<value>         ! Spin-dependent WIMP-proton coupling [unitless].
!                        ! Related by GpSD = 2\sqrt{2} G_F ap.
!   --an=<value>         ! Spin-dependent WIMP-neutron coupling [unitless].
!                        ! Related by GnSD = 2\sqrt{2} G_F an.
! Cross-section options may be given as negative values to indicate the
! corresponding coupling should be negative:
!   --sigmapSI=<value>   ! Spin-independent WIMP-proton cross-section [pb].
!   --sigmanSI=<value>   ! Spin-independent WIMP-neutron cross-section [pb].
!   --sigmapSD=<value>   ! Spin-dependent WIMP-proton cross-section [pb].
!   --sigmanSD=<value>   ! Spin-dependent WIMP-neutron cross-section [pb].
!   --sigmaSI=<value>    ! Sets both sigmapSI and sigmanSI to the given value [pb].
!   --sigmaSD=<value>    ! Sets both sigmapSD and sigmanSD to the given value [pb].
! 
SUBROUTINE InitWIMPCL()
  IMPLICIT NONE
  LOGICAL :: status
  REAL*8 :: x
  
  ! Process mass: default value of 100 GeV
  CALL SetWIMP(m=100d0)
  IF (GetLongArgReal('m',x)) CALL SetWIMP(m=x)
  IF (Arguments%Nparameters .GE. 1) THEN
    x = Arguments%values(1)
    IF (x .GT. 0d0) CALL SetWIMP(m=x)
  END IF
  
  ! Process couplings: defaults of 1 pb.
  WIMP%GpSI = WIMP%GpSI0
  WIMP%GnSI = WIMP%GnSI0
  WIMP%GpSD = WIMP%GpSD0
  WIMP%GnSD = WIMP%GnSD0
  IF (GetLongArgReal('GpSI',x)) CALL SetWIMP(GpSI=x)
  IF (GetLongArgReal('GnSI',x)) CALL SetWIMP(GnSI=x)
  IF (GetLongArgReal('GpSD',x)) CALL SetWIMP(GpSD=x)
  IF (GetLongArgReal('GnSD',x)) CALL SetWIMP(GnSD=x)
  IF (GetLongArgReal('fp',x))   CALL SetWIMP(fp=x)
  IF (GetLongArgReal('fn',x))   CALL SetWIMP(fn=x)
  IF (GetLongArgReal('ap',x))   CALL SetWIMP(ap=x)
  IF (GetLongArgReal('an',x))   CALL SetWIMP(an=x)
  IF (GetLongArgReal('sigmapSI',x)) CALL SetWIMP(sigmapSI=x)
  IF (GetLongArgReal('sigmanSI',x)) CALL SetWIMP(sigmanSI=x)
  IF (GetLongArgReal('sigmapSD',x)) CALL SetWIMP(sigmapSD=x)
  IF (GetLongArgReal('sigmanSD',x)) CALL SetWIMP(sigmanSD=x)
  IF (GetLongArgReal('sigmaSI',x))  CALL SetWIMP(sigmaSI=x)
  IF (GetLongArgReal('sigmaSD',x))  CALL SetWIMP(sigmaSD=x)
  
  ! Process command-line arguments (if more than just mass)
  IF (Arguments%Nparameters .GE. 1) THEN
    status = ParseWIMPParameters(Arguments%Nparameters,Arguments%values)
  END IF
  
END SUBROUTINE


!-----------------------------------------------------------------------
! Routine to read a line containing WIMP parameters from standard input
! (if line not given explicitly), parse it, and set WIMP parameters
! accordingly.  Returns true if a non-empty line was found and was
! parsable (false if no line found (EOF), line was empty, or line
! could not be parsed).
! 
! The following forms are allowed:
!   m
!   m sigmaSI
!   m sigmaSI sigmaSD
!   m sigmaSI sigmanSD sigmanSD
!   m sigmapSI sigmanSI sigmanSD sigmanSD
!   (formerly:  m GpSI GnSI GpSD GnSD)
! In the first case, all couplings are set to 1 pb, while in the second
! case, the SD couplings are set to zero.
! 
! Optional input argument:
!   line        String containing WIMP parameters.  If not given,
!               a line is taken from standard output.
! 
FUNCTION ParseWIMPInput(line) RESULT(status)
  IMPLICIT NONE
  LOGICAL :: status
  CHARACTER(LEN=*), INTENT(IN), OPTIONAL :: line
  CHARACTER(LEN=1024) :: line0
  INTEGER :: ios,Np
  REAL*8 :: params(6)
  
  status = .FALSE.
  
  ! Use given line or read from standard input
  IF (PRESENT(line)) THEN
    line0 = line
  ELSE
    READ(*,'(A)',IOSTAT=ios) line0
    IF (ios .NE. 0) RETURN
  END IF
  
  ! Check if empty string
  IF (TRIM(line0) .EQ. '') RETURN
  
  ! Determine number of parameters and read them in
  Np = MIN(NumberOfFields(line0),6)
  IF (Np .LE. 0) RETURN
  READ(line0,*,IOSTAT=ios) params(1:Np)
  IF (ios .NE. 0) RETURN
  
  ! Now parse parameters
  status = ParseWIMPParameters(Np,params)
  
END FUNCTION


!-----------------------------------------------------------------------
! Routine to take an array containing WIMP parameters and set the
! internal WIMP parameters to that.  The meaning of the given
! parameters depends upon the number of parameters and is the same
! as expected for the commandline.  Returns false if an invalid
! number of parameters or invalid value is found.
! 
! The following array lengths and parameters are allowed:
!   N=1:  m
!   N=2:  m sigmaSI
!   N=3:  m sigmaSI sigmaSD
!   N=4:  m sigmaSI sigmanSD sigmanSD
!   N=5:  m sigmapSI sigmanSI sigmanSD sigmanSD
!         (formerly:  m GpSI GnSI GpSD GnSD)
! In the first case, all couplings are set to 1 pb, while in the second
! case, the SD couplings are set to zero.
! 
! Input arguments:
!   N           Number of parameters
!   p           Array of parameters of size [1:N]
! 
FUNCTION ParseWIMPParameters(N,p) RESULT(status)
  IMPLICIT NONE
  LOGICAL :: status
  INTEGER, INTENT(IN) :: N
  REAL*8, INTENT(IN) :: p(N)
  
  ! Check for bad cases (e.g. non-positive mass)
  status = .FALSE.
  IF (N .LT. 1) RETURN
  IF (p(1) .LE. 0d0) RETURN
  
  status = .TRUE.
  
  ! Meaning of parameters depends on number of parameters,
  ! but first parameter is always mass
  CALL SetWIMP(m=p(1))
  SELECT CASE (N)
  CASE (1)
    ! Form: m
    ! Set WIMP couplings to 1 pb
    WIMP%GpSI = WIMP%GpSI0
    WIMP%GnSI = WIMP%GnSI0
    WIMP%GpSD = WIMP%GpSD0
    WIMP%GnSD = WIMP%GnSD0
  CASE (2)
    ! Form: m sigmaSI
    ! Set SD couplings to zero
    CALL SetWIMP(sigmaSI=p(2),sigmaSD=0d0)
  CASE (3)
    ! Form: m sigmaSI sigmaSD
    CALL SetWIMP(sigmaSI=p(2),sigmaSD=p(3))
  CASE (4)
    ! Form: m sigmaSI sigmapSD sigmanSD
    CALL SetWIMP(sigmaSI=p(2),sigmapSD=p(3),sigmanSD=p(4))
  CASE (5)
    !! Form: m sigmapSI sigmanSI sigmapSD sigmanSD
    CALL SetWIMP(sigmapSI=p(2),sigmanSI=p(3),sigmapSD=p(4),sigmanSD=p(5))
    ! Form: m GpSI GnSI GpSD GnSD
    !CALL SetWIMP(GpSI=p(2),GnSI=p(3),GpSD=p(4),GnSD=p(5))
  CASE (6:)
    status = .FALSE.
  END SELECT
  
END FUNCTION



!=======================================================================
! COUPLING CONVERSION ROUTINES
!=======================================================================

!-----------------------------------------------------------------------
! Converts spin-independent coupling f to G, related by:
!   Gp = 2*fp    Gn = 2*fn
! 
! Input arguments:
!   f           SI coupling [GeV^-2]
! Returns in [GeV^-2].
! 
ELEMENTAL FUNCTION FToG(f) RESULT(G)
  IMPLICIT NONE
  REAL*8 :: G
  REAL*8, INTENT(IN) :: f
  REAL*8, PARAMETER :: F_SCALE = 2d0
  G = f * F_SCALE
END FUNCTION


!-----------------------------------------------------------------------
! Converts spin-independent coupling G to f, related by:
!   Gp = 2*fp    Gn = 2*fn
! 
! Input arguments:
!   G           SI coupling [GeV^-2]
! Returns in [GeV^-2].
! 
ELEMENTAL FUNCTION GToF(G) RESULT(f)
  IMPLICIT NONE
  REAL*8 :: f
  REAL*8, INTENT(IN) :: G
  REAL*8, PARAMETER :: F_SCALE = 2d0
  f = G / F_SCALE
END FUNCTION


!-----------------------------------------------------------------------
! Converts spin-dependent coupling a to G, related by:
!   Gp = 2*sqrt{2}*G_F*ap    Gn = 2*sqrt{2}*G_F*an
! 
! Input arguments:
!   a           SD coupling [unitless]
! Returns in [GeV^-2].
! 
ELEMENTAL FUNCTION AToG(a) RESULT(G)
  IMPLICIT NONE
  REAL*8 :: G
  REAL*8, INTENT(IN) :: a
  REAL*8, PARAMETER :: A_SCALE = SQRT(2d0)*2d0*FERMI_COUPLING_CONSTANT
  G = a * A_SCALE
END FUNCTION


!-----------------------------------------------------------------------
! Converts spin-dependent coupling G to a, related by:
!   Gp = 2*sqrt{2}*G_F*ap    Gn = 2*sqrt{2}*G_F*an
! 
! Input arguments:
!   G           SD coupling [GeV^-2]
! Returns in [unitless].
! 
ELEMENTAL FUNCTION GToA(G) RESULT(a)
  IMPLICIT NONE
  REAL*8 :: a
  REAL*8, INTENT(IN) :: G
  REAL*8, PARAMETER :: A_SCALE = SQRT(2d0)*2d0*FERMI_COUPLING_CONSTANT
  a = G / A_SCALE
END FUNCTION



!=======================================================================
! CROSS-SECTION/COUPLING CONVERSION ROUTINES
!=======================================================================

!-----------------------------------------------------------------------
! Converts WIMP-proton spin-independent cross-section to G, related by:
!   sigmapSI = \mu^2/\pi Gp^2
! where \mu is WIMP-proton reduced mass.
! 
! Input arguments:
!   m           WIMP mass [GeV]
!   sigma       SI cross-section [pb].  A negative value means
!               the coupling should be set negative.
! Returns in [GeV^-2].
! 
ELEMENTAL FUNCTION SigmapSIToGp(m,sigma) RESULT(G)
  IMPLICIT NONE
  REAL*8 :: G
  REAL*8, INTENT(IN) :: m,sigma
  REAL*8, PARAMETER :: Mp = PROTON_MASS
  ! Factor of 1d-10 takes pb to fm^2
  IF (sigma .GE. 0d0) THEN
    G = SQRTPI * (m+Mp)/(m*Mp) * SQRT(1d-10*sigma) / HBARC
  ELSE
    G = - SQRTPI * (m+Mp)/(m*Mp) * SQRT(-1d-10*sigma) / HBARC
  END IF
END FUNCTION


!-----------------------------------------------------------------------
! Converts WIMP-neutron spin-independent cross-section to G, related by:
!   sigmanSI = \mu^2/\pi Gn^2
! where \mu is WIMP-neutron reduced mass.
! 
! Input arguments:
!   m           WIMP mass [GeV]
!   sigma       SI cross-section [pb].  A negative value means
!               the coupling should be set negative.
! Returns in [GeV^-2].
! 
ELEMENTAL FUNCTION SigmanSIToGn(m,sigma) RESULT(G)
  IMPLICIT NONE
  REAL*8 :: G
  REAL*8, INTENT(IN) :: m,sigma
  REAL*8, PARAMETER :: Mn = NEUTRON_MASS
  ! Factor of 1d-10 takes pb to fm^2
  IF (sigma .GE. 0d0) THEN
    G = SQRTPI * (m+Mn)/(m*Mn) * SQRT(1d-10*sigma) / HBARC
  ELSE
    G = - SQRTPI * (m+Mn)/(m*Mn) * SQRT(-1d-10*sigma) / HBARC
  END IF
END FUNCTION


!-----------------------------------------------------------------------
! Converts G to WIMP-proton spin-independent cross-section, related by:
!   sigmapSI = \mu^2/\pi Gp^2
! where \mu is WIMP-proton reduced mass.
! 
! Input arguments:
!   m           WIMP mass [GeV]
!   G           SI coupling [GeV^-2]
! Returns in [pb].
! 
ELEMENTAL FUNCTION GpToSigmapSI(m,G) RESULT(sigma)
  IMPLICIT NONE
  REAL*8 :: sigma
  REAL*8, INTENT(IN) :: m,G
  REAL*8, PARAMETER :: Mp = PROTON_MASS
  ! Factor of 1d10 takes fm^2 to pb
  sigma = 1d10 * INVPI * ((m*Mp/(m+Mp))*G*HBARC)**2
END FUNCTION


!-----------------------------------------------------------------------
! Converts G to WIMP-neutron spin-independent cross-section, related by:
!   sigmanSI = \mu^2/\pi Gn^2
! where \mu is WIMP-neutron reduced mass.
! 
! Input arguments:
!   m           WIMP mass [GeV]
!   G           SI coupling [GeV^-2]
! Returns in [pb].
! 
ELEMENTAL FUNCTION GnToSigmanSI(m,G) RESULT(sigma)
  IMPLICIT NONE
  REAL*8 :: sigma
  REAL*8, INTENT(IN) :: m,G
  REAL*8, PARAMETER :: Mn = NEUTRON_MASS
  ! Factor of 1d10 takes fm^2 to pb
  sigma = 1d10 * INVPI * ((m*Mn/(m+Mn))*G*HBARC)**2
END FUNCTION


!-----------------------------------------------------------------------
! Converts WIMP-proton spin-dependent cross-section to G, related by:
!   sigmapSD = 3\mu^2/\pi Gp^2
! where \mu is WIMP-proton reduced mass.
! 
! Input arguments:
!   m           WIMP mass [GeV]
!   sigma       SD cross-section [pb].  A negative value means
!               the coupling should be set negative.
! Returns in [GeV^-2].
! 
ELEMENTAL FUNCTION SigmapSDToGp(m,sigma) RESULT(G)
  IMPLICIT NONE
  REAL*8 :: G
  REAL*8, INTENT(IN) :: m,sigma
  REAL*8, PARAMETER :: Mp = PROTON_MASS
  ! Factor of 1d-10 takes pb to fm^2
  IF (sigma .GE. 0d0) THEN
    G = SQRTPI/SQRT3 * (m+Mp)/(m*Mp) * SQRT(1d-10*sigma) / HBARC
  ELSE
    G = - SQRTPI/SQRT3 * (m+Mp)/(m*Mp) * SQRT(-1d-10*sigma) / HBARC
  END IF
END FUNCTION


!-----------------------------------------------------------------------
! Converts WIMP-neutron spin-dependent cross-section to G, related by:
!   sigmanSD = 3\mu^2/\pi Gn^2
! where \mu is WIMP-neutron reduced mass.
! 
! Input arguments:
!   m           WIMP mass [GeV]
!   sigma       SD cross-section [pb].  A negative value means
!               the coupling should be set negative.
! Returns in [GeV^-2].
! 
ELEMENTAL FUNCTION SigmanSDToGn(m,sigma) RESULT(G)
  IMPLICIT NONE
  REAL*8 :: G
  REAL*8, INTENT(IN) :: m,sigma
  REAL*8, PARAMETER :: Mn = NEUTRON_MASS
  ! Factor of 1d-10 takes pb to fm^2
  IF (sigma .GE. 0d0) THEN
    G = SQRTPI/SQRT3 * (m+Mn)/(m*Mn) * SQRT(1d-10*sigma) / HBARC
  ELSE
    G = - SQRTPI/SQRT3 * (m+Mn)/(m*Mn) * SQRT(-1d-10*sigma) / HBARC
  END IF
END FUNCTION


!-----------------------------------------------------------------------
! Converts G to WIMP-proton spin-dependent cross-section, related by:
!   sigmapSD = 3\mu^2/\pi Gp^2
! where \mu is WIMP-proton reduced mass.
! 
! Input arguments:
!   m           WIMP mass [GeV]
!   G           SD coupling [GeV^-2]
! Returns in [pb].
! 
ELEMENTAL FUNCTION GpToSigmapSD(m,G) RESULT(sigma)
  IMPLICIT NONE
  REAL*8 :: sigma
  REAL*8, INTENT(IN) :: m,G
  REAL*8, PARAMETER :: Mp = PROTON_MASS
  ! Factor of 1d10 takes fm^2 to pb
  sigma = 1d10 * 3*INVPI * ((m*Mp/(m+Mp))*G*HBARC)**2
END FUNCTION


!-----------------------------------------------------------------------
! Converts G to WIMP-neutron spin-dependent cross-section, related by:
!   sigmanSD = 3\mu^2/\pi Gn^2
! where \mu is WIMP-neutron reduced mass.
! 
! Input arguments:
!   m           WIMP mass [GeV]
!   G           SD coupling [GeV^-2]
! Returns in [pb].
! 
ELEMENTAL FUNCTION GnToSigmanSD(m,G) RESULT(sigma)
  IMPLICIT NONE
  REAL*8 :: sigma
  REAL*8, INTENT(IN) :: m,G
  REAL*8, PARAMETER :: Mn = NEUTRON_MASS
  ! Factor of 1d10 takes fm^2 to pb
  sigma = 1d10 * 3*INVPI * ((m*Mn/(m+Mn))*G*HBARC)**2
END FUNCTION



!=======================================================================
! HALO ROUTINES
!=======================================================================

! ----------------------------------------------------------------------
! Get various halo quantities.
! 
! Optional output arguments regarding galactic motions:
!   vrot        Local galactic disk rotation speed [km/s].
!   vlsr        Local standard of rest velocity vector (array of size 3)
!               [km/s], defined relative to galactic rest frame.
!   vpec        Sun's peculiar velocity vector (array of size 3) [km/s],
!               defined relative to local standard of rest.
!   vsun        Sun's velocity vector (array of size 3) [km/s], defined
!               relative to galactic rest frame.
! Optional output arguments regarding dark matter density:
!   rho         Local dark matter density [GeV/cm^3].
! Optional output arguments regarding SHM distribution, a truncated
! Maxwell-Boltzmann ("MB"):
!   vbulk       Bulk velocity of dark matter (array of size 3) [km/s],
!               defined relative to galactic rest frame.
!   vobs        Observer/detector's speed (i.e. Sun's speed) [km/s],
!               defined relative to MB rest frame.
!   v0          Most probable speed [km/s] in the MB rest frame.
!   vesc        Galactic/population escape speed [km/s] in the MB rest
!               frame (_galactic_ escape speed only if MB has no bulk
!               motion relative to galactic rest frame).
! Optional output arguments regarding tabulated eta(vmin):
!   tabulated   Indicates if a tabulated eta(vmin) is being used
!   eta_file    The file tabulated eta were taken from
!   Nvmin       Number of tabulation points
!   vmin        Allocatable array of vmin [km/s]
!   eta         Allocatable array of mean inverse speeds at vmin [s/km]
! 
SUBROUTINE GetHalo(vrot,vlsr,vpec,vsun,rho,vbulk,vobs,v0,vesc,          &
                   tabulated,eta_file,Nvmin,vmin,eta)
  IMPLICIT NONE
  CHARACTER(LEN=*), INTENT(OUT), OPTIONAL :: eta_file
  LOGICAL, INTENT(OUT), OPTIONAL :: tabulated
  INTEGER, INTENT(OUT), OPTIONAL :: Nvmin
  REAL*8, INTENT(OUT), OPTIONAL :: vrot,vlsr(3),vpec(3),vsun(3),       &
                                   rho,vbulk(3),vobs,v0,vesc
  REAL*8, ALLOCATABLE, INTENT(OUT), OPTIONAL :: vmin(:),eta(:)
  
  IF (PRESENT(vrot))  vrot  = Halo%vrot
  IF (PRESENT(vlsr))  vlsr  = Halo%vlsr
  IF (PRESENT(vpec))  vpec  = Halo%vpec
  IF (PRESENT(vsun))  vsun  = Halo%vsun
  
  IF (PRESENT(rho))   rho   = Halo%rho
  
  IF (PRESENT(vbulk)) vbulk = Halo%vbulk
  IF (PRESENT(vobs))  vobs  = Halo%vobs
  IF (PRESENT(v0))    v0    = Halo%v0
  IF (PRESENT(vesc))  vesc  = Halo%vesc
  
  IF (PRESENT(tabulated)) tabulated = Halo%tabulated
  IF (PRESENT(eta_file)) eta_file = Halo%eta_file
  IF (PRESENT(Nvmin)) Nvmin = Halo%Nvmin
  IF (PRESENT(vmin)) THEN
    ALLOCATE(vmin(Halo%Nvmin))
    vmin = Halo%vmin
  END IF
  IF (PRESENT(eta)) THEN
    ALLOCATE(eta(Halo%Nvmin))
    eta = Halo%eta
  END IF
END SUBROUTINE


! ----------------------------------------------------------------------
! Set various halo quantities.
! 
! Optional input arguments regarding galactic motions:
!   vrot        Local galactic disk rotation speed [km/s].
!   vlsr        Local standard of rest velocity vector (array of size 3)
!               [km/s], defined relative to galactic rest frame.
!   vpec        Sun's peculiar velocity vector (array of size 3) [km/s],
!               defined relative to local standard of rest.
!   vsun        Sun's velocity vector (array of size 3) [km/s], defined
!               relative to galactic rest frame.
! Optional input arguments regarding dark matter density:
!   rho         Local dark matter density [GeV/cm^3].
! Optional input arguments regarding SHM distribution, a truncated
! Maxwell-Boltzmann ("MB"):
!   vbulk       Bulk velocity of dark matter (array of size 3) [km/s],
!               defined relative to galactic rest frame.
!   vobs        Observer/detector's speed (i.e. Sun's speed) [km/s],
!               defined relative to MB rest frame.
!   v0          Most probable speed [km/s] in the MB rest frame.
!   vesc        Galactic/population escape speed [km/s] in the MB rest
!               frame (_galactic_ escape speed only if MB has no bulk
!               motion relative to galactic rest frame).
! Optional tabulated eta(vmin) arguments.  Can be loaded from a given
! file or explicitly provided.  If provided, Nvmin, vmin, and eta must
! all be given to take effect.  When a tabulation is not provided, the
! mean inverse speed will be calculated explicitly (not tabulated!)
! using the SHM as described by the above parameters.
!   tabulated   Indicates if a tabulated eta(vmin) is to be used.  Implied
!               by the use of other tabulation arguments, but can be set
!               false to return to the SHM calculation after a tabulation
!               has been loaded.
!   eta_file    File from which tabulated eta(vmin) should be read;
!               default is to perform explicit calculations for the SHM
!               describe The file tabulated eta were taken from
!   eta_filename Sets the stored file name _without_ loading any data
!               from the file.
!   eta_file_K  The column number in the file to take eta from (default
!               is second)
!   Nvmin       Number of tabulation points
!   vmin        Array of size [1:Nvmin] containing tabulation vmin [km/s]
!   eta         Array of size [1:Nvmin] containing tabulated mean inverse
!               speeds at vmin [s/km]
! 
SUBROUTINE SetHalo(vrot,vlsr,vpec,vsun,vobs,rho,vbulk,v0,vesc,          &
                   tabulated,eta_file,eta_filename,eta_file_K,Nvmin,vmin,eta)
  IMPLICIT NONE
  CHARACTER(LEN=*), INTENT(IN), OPTIONAL :: eta_file,eta_filename
  LOGICAL, INTENT(IN), OPTIONAL :: tabulated
  INTEGER, INTENT(IN), OPTIONAL :: eta_file_K,Nvmin
  REAL*8, INTENT(IN), OPTIONAL :: vrot,vlsr(3),vpec(3),vsun(3),         &
                                  rho,vbulk(3),vobs,v0,vesc
  REAL*8, INTENT(IN), OPTIONAL :: vmin(:),eta(:)
  INTEGER :: K
  
  IF (PRESENT(vrot))  CALL SetDiskRotationSpeed(vrot)
  IF (PRESENT(vlsr))  CALL SetLocalStandardOfRest(vlsr)
  IF (PRESENT(vpec))  CALL SetSunPeculiarVelocity(vpec)
  IF (PRESENT(vsun))  CALL SetSunVelocity(vsun)
  
  IF (PRESENT(rho))   CALL SetLocalDensity(rho)
  
  IF (PRESENT(vbulk)) CALL SetBulkVelocity(vbulk)
  IF (PRESENT(vobs))  CALL SetObserverSpeed(vobs)
  IF (PRESENT(v0))    CALL SetMostProbableSpeed(v0)
  IF (PRESENT(vesc))  CALL SetEscapeSpeed(vesc)
  
  IF (PRESENT(tabulated)) THEN
    IF (Halo%Nvmin .GT. 0) Halo%tabulated = tabulated
  END IF
  IF (PRESENT(Nvmin) .AND. PRESENT(vmin) .AND. PRESENT(eta)) THEN
    IF (Nvmin .GT. 0) THEN
      IF (ALLOCATED(Halo%vmin)) DEALLOCATE(Halo%vmin)
      IF (ALLOCATED(Halo%eta))  DEALLOCATE(Halo%eta)
      Halo%Nvmin = Nvmin
      Halo%vmin  = vmin
      Halo%eta   = eta
      Halo%tabulated = .TRUE.
      Halo%eta_file  = ''
    END IF
  END IF
  IF (PRESENT(eta_file)) THEN
    IF (PRESENT(eta_file_K)) THEN
      K = eta_file_K
    ELSE
      K = 2
    END IF
    CALL LoadArrays(file=eta_file,N=Halo%Nvmin,N1=1,C1=Halo%vmin,       &
                    N2=K,C2=Halo%eta)
    Halo%tabulated = .TRUE.
    Halo%eta_file  = eta_file
  END IF
  
  IF (PRESENT(eta_filename)) Halo%eta_file = eta_filename
  
END SUBROUTINE


!-----------------------------------------------------------------------
! Initializes halo.
! Simply sets halo parameters to default values.
! 
SUBROUTINE InitHalo()
  IMPLICIT NONE
  
  Halo%vrot  = 235d0
  Halo%vlsr  = (/ 0d0, 235d0, 0d0 /)
  Halo%vpec  = (/ 11d0, 12d0, 7d0 /)
  Halo%vsun  = (/ 0d0, 235d0, 0d0 /) + (/ 11d0, 12d0, 7d0 /)
  
  Halo%rho   = 0.4d0
  
  Halo%vbulk = (/ 0d0, 0d0, 0d0 /)
  Halo%vobs  = SQRT(11d0**2 + (235d0+12d0)**2 + 7d0**2)
  Halo%v0    = 235d0
  Halo%vesc  = 550d0
  
  Halo%tabulated = .FALSE.
  Halo%eta_file  = ''
  Halo%Nvmin = 0
  IF (ALLOCATED(Halo%vmin)) DEALLOCATE(Halo%vmin)
  IF (ALLOCATED(Halo%eta))  DEALLOCATE(Halo%eta)
  
END SUBROUTINE


!-----------------------------------------------------------------------
! Initializes halo from command-line parameters.
! 
! Possible options regarding galactic motions:
!   --vrot=<value>       ! Local galactic disk rotation speed [km/s].
!   --vlsr=<x>,<y>,<z>   ! Local standard of rest velocity vector (array of size 3)
!                        ! [km/s], defined relative to galactic rest frame.
!   --vpec=<x>,<y>,<z>   ! Sun's peculiar velocity vector (array of size 3) [km/s],
!                        ! defined relative to local standard of rest.
!   --vsun=<x>,<y>,<z>   ! Sun's velocity vector (array of size 3) [km/s], defined
!                        ! relative to galactic rest frame.
! Possible options regarding dark matter density:
!   --rho=<value>        ! Local dark matter density [GeV/cm^3]
! Possible options regarding SHM distribution:
!   --vbulk=<x>,<y>,<z>  ! Bulk velocity of dark matter (array of size 3) [km/s],
!                        ! defined relative to galactic rest frame.
!   --vobs=<value>       ! Observer/detector's speed (i.e. Sun's speed) [km/s],
!                        ! defined relative to MB rest frame.
!   --v0=<value>         ! Most probable speed [km/s] in the galactic rest frame.
!   --vesc=<value>       ! Escape speed of the dark matter population [km/s] in
!                        ! its rest frame.
! Possible options for provided a tabulated eta(vmin) instead of using the above
! SHM distribution.
!   --eta-file=<file>    ! File from which tabulated mean inverse speed eta(vmin)
!                        ! should be read.  First column is vmin [km/s] and second
!                        ! column is eta [s/km].  Default behavior is to do explicit
!                        ! calculations for SHM.
!   --eta-file=<file>,<K>! Same as above, but take the Kth column for eta.
! 
SUBROUTINE InitHaloCL()
  IMPLICIT NONE
  CHARACTER(LEN=1024) :: eta_file
  INTEGER :: I,K,Nval,ios
  REAL*8 :: vrot,vobs,rho,v0,vesc
  REAL*8, ALLOCATABLE :: vlsr(:),vpec(:),vsun(:),vbulk(:)
  CHARACTER(LEN=:), DIMENSION(:), ALLOCATABLE :: aval
  
  CALL InitHalo()
  
  IF (GetLongArgReal('vrot',vrot)) CALL SetDiskRotationSpeed(vrot)
  IF (GetLongArgReals('vlsr',vlsr,I)) THEN
    IF (I .EQ. 3) THEN
      CALL SetLocalStandardOfRest(vlsr)
    ELSE
      WRITE(0,*) 'ERROR: Invalid --vlsr=<vx>,<vy>,<vz> parameter.'
      STOP
    END IF
  END IF
  IF (GetLongArgReals('vpec',vpec,I)) THEN
    IF (I .EQ. 3) THEN
      CALL SetSunPeculiarVelocity(vpec)
    ELSE
      WRITE(0,*) 'ERROR: Invalid --vpec=<vx>,<vy>,<vz> parameter.'
      STOP
    END IF
  END IF
  IF (GetLongArgReals('vsun',vsun,I)) THEN
    IF (I .EQ. 3) THEN
      CALL SetSunVelocity(vsun)
    ELSE
      WRITE(0,*) 'ERROR: Invalid --vsun=<vx>,<vy>,<vz> parameter.'
      STOP
    END IF
  END IF
  
  IF (GetLongArgReal('rho',rho))   CALL SetLocalDensity(rho)
  
  IF (GetLongArgReals('vbulk',vbulk,I)) THEN
    IF (I .EQ. 3) THEN
      CALL SetBulkVelocity(vbulk)
    ELSE
      WRITE(0,*) 'ERROR: Invalid --vbulk=<vx>,<vy>,<vz> parameter.'
      STOP
    END IF
  END IF
  IF (GetLongArgReal('vobs',vobs)) CALL SetObserverSpeed(vobs)
  IF (GetLongArgReal('v0',v0))     CALL SetMostProbableSpeed(v0)
  IF (GetLongArgReal('vesc',vesc)) CALL SetEscapeSpeed(vesc)
  
  !IF (GetLongArgString('eta-file',eta_file)) CALL SetHalo(eta_file=eta_file)
  IF (GetLongArgStrings('eta-file',1024,aval,Nval)) THEN
    IF (Nval .GE. 2) THEN
      READ(aval(2),*,IOSTAT=ios) K
      IF (ios .NE. 0) K = 2
    ELSE
      K = 2
    END IF
    CALL SetHalo(eta_file=aval(1),eta_file_K=K)
  END IF
  
END SUBROUTINE


! ----------------------------------------------------------------------
! Get/set local galactic disk rotation speed [km/s].
! Modifies the Local Standard of Rest (LSR) and Sun's velocity relative
! to halo rest frame as well as the most probable speed of the velocity
! distribution (v0 = vrot).  The observer speed is updated.
! 
PURE FUNCTION GetDiskRotationSpeed() RESULT(vrot)
  IMPLICIT NONE
  REAL*8 :: vrot
  vrot = Halo%vrot
END FUNCTION

SUBROUTINE SetDiskRotationSpeed(vrot)
  IMPLICIT NONE
  REAL*8, INTENT(IN) :: vrot
  Halo%vrot = vrot
  Halo%vlsr = (/ 0d0, Halo%vrot, 0d0 /)
  Halo%vsun = Halo%vlsr + Halo%vpec
  Halo%vobs = SQRT(SUM((Halo%vsun - Halo%vbulk)**2))
  Halo%v0   = Halo%vrot
END SUBROUTINE


! ----------------------------------------------------------------------
! Get/set Local Standard Of Rest velocity vector [km/s], defined
! relative to galactic rest frame.  Usually assumed to be (0,vrot,0),
! where vrot is disk rotation speed.  Modifies Sun's velocity relative
! to halo rest frame.  The disk rotation speed and the most probable
! speed of the velocity distribution are set to the y component of this
! velocity vector.  The observer speed is updated.
! 
PURE FUNCTION GetLocalStandardOfRest() RESULT(vlsr)
  IMPLICIT NONE
  REAL*8 :: vlsr(3)
  vlsr = Halo%vlsr
END FUNCTION

SUBROUTINE SetLocalStandardOfRest(vlsr)
  IMPLICIT NONE
  REAL*8, INTENT(IN) :: vlsr(3)
  Halo%vlsr = vlsr
  Halo%vsun = Halo%vlsr + Halo%vpec
  Halo%vrot = vlsr(2)
  Halo%vobs = SQRT(SUM((Halo%vsun - Halo%vbulk)**2))
  Halo%v0   = ABS(Halo%vrot)
END SUBROUTINE


! ----------------------------------------------------------------------
! Get/set Sun's peculiar velocity vector [km/s], defined relative to
! local standard of rest.  Modifies Sun's velocity relative to halo
! rest frame.  The observer speed is updated.
! 
PURE FUNCTION GetSunPeculiarVelocity() RESULT(vpec)
  IMPLICIT NONE
  REAL*8 :: vpec(3)
  vpec = Halo%vpec
END FUNCTION

SUBROUTINE SetSunPeculiarVelocity(vpec)
  IMPLICIT NONE
  REAL*8, INTENT(IN) :: vpec(3)
  Halo%vpec = vpec
  Halo%vsun = Halo%vlsr + Halo%vpec
  Halo%vobs = SQRT(SUM((Halo%vsun - Halo%vbulk)**2))
END SUBROUTINE


! ----------------------------------------------------------------------
! Get/set Sun's velocity vector [km/s], defined relative to galactic
! rest frame.  Normally taken to be vlsr + vpec, i.e. the sum of the
! local standard of rest and the Sun's peculiar velocity.  The preferred
! way to set speeds is modifying the disk rotation speed or Sun's
! peculiar velocity, not by setting this velocity directly, as the
! contributing velocities become ill-defined.  If the Sun's velocity is
! set here, the routine will attempt to assign a rotation speed vrot
! and local standard of rest vlsr = (0,vrot,0) that matches the given
! velocity vector, using the current value of the peculiar velocity; if
! not possible, the peculiar motion is set to zero first.  The most
! probable speed of the velocity distribution is updated to the
! resulting vrot and the observer speed is updated.
! 
PURE FUNCTION GetSunVelocity() RESULT(vsun)
  IMPLICIT NONE
  REAL*8 :: vsun(3)
  vsun = Halo%vsun
END FUNCTION

SUBROUTINE SetSunVelocity(vsun)
  IMPLICIT NONE
  REAL*8, INTENT(IN) :: vsun(3)
  REAL*8 :: vrot2
  Halo%vsun = vsun
  vrot2 = SUM((Halo%vsun - Halo%vpec)**2)
  IF (vrot2 .GE. 0d0) THEN
    Halo%vrot = SQRT(vrot2)
    Halo%vlsr = (/ 0d0, Halo%vrot, 0d0 /)
  ELSE
    Halo%vpec = 0d0
    Halo%vrot = SQRT(SUM(Halo%vsun**2))
    Halo%vlsr = (/ 0d0, Halo%vrot, 0d0 /)
  END IF
  Halo%v0 = ABS(Halo%vrot)
  Halo%vobs = SQRT(SUM((Halo%vsun - Halo%vbulk)**2))
END SUBROUTINE


! ----------------------------------------------------------------------
! Get/set local halo density [GeV/cm^3].
! 
PURE FUNCTION GetLocalDensity() RESULT(rho)
  IMPLICIT NONE
  REAL*8 :: rho
  rho = Halo%rho
END FUNCTION

SUBROUTINE SetLocalDensity(rho)
  IMPLICIT NONE
  REAL*8, INTENT(IN) :: rho
  Halo%rho = MAX(rho,0d0)
END SUBROUTINE


! ----------------------------------------------------------------------
! Get/set the dark matter population's bulk velocity vector [km/s],
! defined relative to the galactic rest frame.  Modifies the observer
! speed.
! 
PURE FUNCTION GetBulkVelocity() RESULT(vbulk)
  IMPLICIT NONE
  REAL*8 :: vbulk(3)
  vbulk = Halo%vbulk
END FUNCTION

SUBROUTINE SetBulkVelocity(vbulk)
  IMPLICIT NONE
  REAL*8, INTENT(IN) :: vbulk(3)
  Halo%vbulk = vbulk
  Halo%vobs = SQRT(SUM((Halo%vsun - Halo%vbulk)**2))
END SUBROUTINE


! ----------------------------------------------------------------------
! Get/set observer/detector's speed (i.e. Sun's speed) [km/s], defined
! relative to Maxwell-Boltzmann population rest frame.  Normally taken
! to be |vlsr + vpec - vMB|, i.e. the sum of the local standard of rest
! and the Sun's peculiar velocity less the bulk velocity of the dark
! matter population.  The preferred way to set speeds is modifying the
! disk rotation speed, Sun's peculiar velocity, or the bulk dark matter
! motion, not by setting this speed directly, as the various
! velocities become ill-defined.  If the observer's speed is set here,
! the routine will set the bulk motion of the DM to zero (relative to
! the galactic rest frame) and attempt to assign a rotation speed vrot
! and local standard of rest vlsr = (0,vrot,0) that matches the given
! speed, using the current value of the peculiar velocity; if not
! possible, the peculiar motion is set to zero first.  The most
! probable speed of the velocity distribution is updated to the
! resulting vrot.
! 
PURE FUNCTION GetObserverSpeed() RESULT(vobs)
  IMPLICIT NONE
  REAL*8 :: vobs
  vobs = Halo%vobs
END FUNCTION

SUBROUTINE SetObserverSpeed(vobs)
  IMPLICIT NONE
  REAL*8, INTENT(IN) :: vobs
  REAL*8 :: vy2
  Halo%vobs  = MAX(vobs,0d0)
  Halo%vbulk = (/ 0d0, 0d0, 0d0 /)
  vy2 = Halo%vobs**2 - Halo%vpec(1)**2 - Halo%vpec(3)**2
  IF (vy2 .GE. 0d0) THEN
    Halo%vrot = SQRT(vy2) - Halo%vpec(2)
    Halo%vlsr = (/ 0d0, Halo%vrot, 0d0 /)
    Halo%vsun = Halo%vlsr + Halo%vpec
  ELSE
    Halo%vpec = 0d0
    Halo%vrot = Halo%vobs
    Halo%vlsr = (/ 0d0, Halo%vrot, 0d0 /)
    Halo%vsun = Halo%vlsr + Halo%vpec
  END IF
  Halo%v0 = ABS(Halo%vrot)
END SUBROUTINE


! ----------------------------------------------------------------------
! Get/set most probable speed v0 [km/s] in the dark matter population's
! rest frame. Related to other speeds characterizing velocity
! distribution by:
!     vrms = sqrt(3/2) v0    [rms velocity]
!     vmp  = v0              [most probably velocity]
!     vave = sqrt(4/pi) v0   [mean velocity]
! 
PURE FUNCTION GetMostProbableSpeed() RESULT(v0)
  IMPLICIT NONE
  REAL*8 :: v0
  v0 = Halo%v0
END FUNCTION

SUBROUTINE SetMostProbableSpeed(v0)
  IMPLICIT NONE
  REAL*8, INTENT(IN) :: v0
  Halo%v0 = v0
END SUBROUTINE


! ----------------------------------------------------------------------
! Get/set dark matter population escape speed [km/s].  In the case of
! the SHM with no bulk motion relative to the galactic rest frame, this
! is the galactic escape speed.
! 
PURE FUNCTION GetEscapeSpeed() RESULT(vesc)
  IMPLICIT NONE
  REAL*8 :: vesc
  vesc = Halo%vesc
END FUNCTION

SUBROUTINE SetEscapeSpeed(vesc)
  IMPLICIT NONE
  REAL*8, INTENT(IN) :: vesc
  Halo%vesc = MAX(vesc,0d0)
END SUBROUTINE


! ----------------------------------------------------------------------
! INTERFACE NAME: EToVmin
! Calculate the minimum velocity for producing a recoil of energy E,
! given by vmin = sqrt{M E/(2\mu^2)} [km/s].
! 
! This is the scalar version (single mass and energy).
! 
! Input arguments:
!   E           Recoil energy [keV]
!   m           WIMP mass [GeV]
!   Miso        Isotope mass [GeV]
! 
ELEMENTAL FUNCTION EToVmin0(E,m,Miso) RESULT(vmin)
  IMPLICIT NONE
  REAL*8 :: vmin
  REAL*8, INTENT(IN) :: E,m,Miso
  REAL*8 :: mu
  REAL*8, PARAMETER :: c = 1d-3*SPEED_OF_LIGHT  ! Speed of light in km/s
  mu = Miso*m / (Miso + m)
  vmin = c * SQRT(1d-6*Miso*E/(2*mu**2))
END FUNCTION


! ----------------------------------------------------------------------
! INTERFACE NAME: EToVmin
! Calculate the minimum velocity for producing a recoil of energy E,
! given by vmin = sqrt{M E/(2\mu^2)} [km/s].  Returns as array of
! size [1:N].
! 
! This is the 1D array version (single mass and array of energies).
! 
! Input arguments:
!   N           Number of recoil energies
!   E           Array of recoil energies [keV]
!   m           WIMP mass [GeV]
!   Miso        Isotope mass [GeV]
! 
PURE FUNCTION EToVmin1(N,E,m,Miso) RESULT(vmin)
  IMPLICIT NONE
  REAL*8 :: vmin(N)
  INTEGER, INTENT(IN) :: N
  REAL*8, INTENT(IN) :: E(N),m,Miso
  REAL*8 :: mu
  REAL*8, PARAMETER :: c = 1d-3*SPEED_OF_LIGHT  ! Speed of light in km/s
  mu = Miso*m / (Miso + m)
  vmin = c * SQRT(1d-6*Miso*E/(2*mu**2))
END FUNCTION


! ----------------------------------------------------------------------
! INTERFACE NAME: EToVmin
! Calculate the minimum velocity for producing a recoil of energy E,
! given by vmin = sqrt{M E/(2\mu^2)} [km/s].  Returns as array of
! size [1:N,1:Niso].
! 
! This is the 2D array version (multiple masses and array of energies).
! 
! Input arguments:
!   N           Number of recoil energies
!   E           Array of recoil energies [keV]
!   m           WIMP mass [GeV]
!   Niso        Number of isotopes
!   Miso        Array of isotope masses [GeV]
! 
PURE FUNCTION EToVmin2(N,E,m,Niso,Miso) RESULT(vmin)
  IMPLICIT NONE
  REAL*8 :: vmin(N,Niso)
  INTEGER, INTENT(IN) :: N,Niso
  REAL*8, INTENT(IN) :: E(N),m,Miso(Niso)
  INTEGER :: I
  REAL*8 :: mu(N)
  REAL*8, PARAMETER :: c = 1d-3*SPEED_OF_LIGHT  ! Speed of light in km/s
  mu = Miso*m / (Miso + m)
  DO I = 1,Niso
    vmin(:,I) = c * SQRT(1d-6*Miso(I)*E/(2*mu(I)**2))
  END DO
END FUNCTION


!-----------------------------------------------------------------------
! INTERFACE NAME: MeanInverseSpeed
! Calculates the mean inverse speed (eta) [s/km] for the given vmin,
! with eta define as:
!     eta(vmin) = \int_{|v|>vmin} d^3v 1/|v| f(v)
! 
! This is the scalar version (single vmin).
! 
! Input arguments:
!   vmin        The minimum speed in the eta integral [km/s]
! 
ELEMENTAL FUNCTION MeanInverseSpeed0(vmin) RESULT(eta)
  IMPLICIT NONE
  REAL*8 :: eta
  REAL*8, INTENT(IN) :: vmin
  REAL*8 :: v0,vobs,vesc,x,y,z,Nesc
  
  ! If have tabulation, use it
  IF (Halo%tabulated) THEN
    eta = MeanInverseSpeedT(vmin)
    RETURN
  END IF
  
  ! Easier to use variable names
  v0   = Halo%v0
  vobs = Halo%vobs
  vesc = Halo%vesc
  
  ! Special case: no dispersion
  ! Distribution is delta function
  IF (v0 .EQ. 0) THEN
    IF (vobs .EQ. 0d0) THEN
      eta = 0d0
    ELSE
      IF (vmin .LE. vobs) THEN
        eta = 1d0 / vobs
      ELSE
        eta = 0d0
      END IF
    END IF
    RETURN
  END IF
  
  x    = vmin / v0
  y    = vobs / v0
  z    = vesc / v0
  Nesc = ERF(z) - 2*INVSQRTPI*z*EXP(-z**2)
  
  ! Special case: no relative motion by observer
  !   eta = 2/(sqrt(pi) Nesc v0) [e^{-x^2} - e^{-z^2}]
  ! Note: EXP2(a,b) = e^b - e^a
  IF (y .EQ. 0d0) THEN
    IF (x .LE. z) THEN
      eta = 2*INVSQRTPI/(Nesc*v0) * EXP2(-z**2,-x**2)
    ELSE
      eta = 0d0
    END IF
    RETURN
  END IF
  
  ! Special case: no finite cutoff (vesc is effectively infinite)
  IF (z .GT. 25d0) THEN
    eta = ERF2(x-y,x+y) / (2*vobs)
    RETURN
  END IF
  
  ! General case.
  ! See e.g. Savage, Freese & Gondolo, PRD 74, 043531 (2006)
  ! [astrop-ph/0607121]; use arxiv version as PRD version has type-
  ! setting issues in the formula.
  ! Note: ERF2(a,b) = ERF(b) - ERF(a)
  IF (x .LE. ABS(y-z)) THEN
    IF (y .LT. z) THEN
      eta = 1d0 / (2*Nesc*vobs) * (ERF2(x-y,x+y) - 4*INVSQRTPI*y*EXP(-z**2))
    ELSE
      eta = 1d0 / vobs
    END IF
  ELSE IF (x .LE. y+z) THEN
    eta = 1d0 / (2*Nesc*vobs) * (ERF2(x-y,z) - 2*INVSQRTPI*(z+y-x)*EXP(-z**2))
  ELSE
    eta = 0d0
  END IF
  
END FUNCTION


!-----------------------------------------------------------------------
! INTERFACE NAME: MeanInverseSpeed
! Calculates the mean inverse speed (eta) [s/km] for the given 1D
! array of vmin, with eta define as:
!     eta(vmin) = \int_{|v|>vmin} d^3v 1/|v| f(v)
! Returns as array of size [1:N].
! 
! This is the 1D array version (1D array of vmin).
! 
! Input arguments:
!   N           Number of vmin
!   vmin        The minimum speed in the eta integral [km/s].
!               Array of size [1:N].
! 
PURE FUNCTION MeanInverseSpeed1(N,vmin) RESULT(eta)
  IMPLICIT NONE
  REAL*8 :: eta(N)
  INTEGER, INTENT(IN) :: N
  REAL*8, INTENT(IN) :: vmin(N)
  REAL*8 :: v0,vobs,vesc,x(N),y,z,Nesc
  
  ! If have tabulation, use it
  IF (Halo%tabulated) THEN
    eta = MeanInverseSpeedT(vmin)
    RETURN
  END IF
  
  ! Easier to use variable names
  v0   = Halo%v0
  vobs = Halo%vobs
  vesc = Halo%vesc
  
  ! Special case: no dispersion
  ! Distribution is delta function
  IF (v0 .EQ. 0) THEN
    IF (vobs .EQ. 0d0) THEN
      eta = 0d0
    ELSE
      WHERE (vmin .LE. vobs)
        eta = 1d0 / vobs
      ELSE WHERE
        eta = 0d0
      END WHERE
    END IF
    RETURN
  END IF
  
  x    = vmin / v0
  y    = vobs / v0
  z    = vesc / v0
  Nesc = ERF(z) - 2*INVSQRTPI*z*EXP(-z**2)
  
  ! Special case: no relative motion by observer
  !   eta = 2/(sqrt(pi) Nesc v0) [e^{-x^2} - e^{-z^2}]
  ! Note: EXP2(a,b) = e^b - e^a
  IF (y .EQ. 0d0) THEN
    WHERE (x .LE. z)
      eta = 2*INVSQRTPI/(Nesc*v0) * EXP2(-z**2,-x**2)
    ELSE WHERE
      eta = 0d0
    END WHERE
    RETURN
  END IF
  
  ! Special case: no finite cutoff (vesc is effectively infinite)
  IF (z .GT. 25d0) THEN
    eta = ERF2(x-y,x+y) / (2*vobs)
    RETURN
  END IF
  
  ! General case.
  ! See e.g. Savage, Freese & Gondolo, PRD 74, 043531 (2006)
  ! [astrop-ph/0607121]; use arxiv version as PRD version has type-
  ! setting issues in the formula.
  ! Note: ERF2(a,b) = ERF(b) - ERF(a)
  ! Separate y < z & y > z cases to make easier use of WHERE statements.
  IF (y .LT. z) THEN
    WHERE (x .LT. z-y)
      eta = 1d0 / (2*Nesc*vobs) * (ERF2(x-y,x+y) - 4*INVSQRTPI*y*EXP(-z**2))
    ELSE WHERE (x .LT. z+y)
      eta = 1d0 / (2*Nesc*vobs) * (ERF2(x-y,z) - 2*INVSQRTPI*(z+y-x)*EXP(-z**2))
    ELSE WHERE
      eta = 0d0
    END WHERE
  ELSE
    WHERE (x .LT. y-z)
      eta = 1d0 / vobs
    ELSE WHERE (x .LT. y+z)
      eta = 1d0 / (2*Nesc*vobs) * (ERF2(x-y,z) - 2*INVSQRTPI*(z+y-x)*EXP(-z**2))
    ELSE WHERE
      eta = 0d0
    END WHERE
  END IF
  
END FUNCTION


!-----------------------------------------------------------------------
! INTERFACE NAME: MeanInverseSpeed
! Calculates the mean inverse speed (eta) [s/km] for the given 2D
! array of vmin, with eta define as:
!     eta(vmin) = \int_{|v|>vmin} d^3v 1/|v| f(v)
! Returns as array of size [1:N1,1:N2].
! 
! This is the 2D array version (2D array of vmin).
! 
! Input arguments:
!   N1,N2       Size of vmin and eta arrays, i.e. [1:N1,1:N2]
!   vmin        The minimum speed in the eta integral [km/s].
!               Array of size [1:N].
! 
PURE FUNCTION MeanInverseSpeed2(N1,N2,vmin) RESULT(eta)
  IMPLICIT NONE
  REAL*8 :: eta(N1,N2)
  INTEGER, INTENT(IN) :: N1,N2
  REAL*8, INTENT(IN) :: vmin(N1,N2)
  REAL*8 :: v0,vobs,vesc,x(N1,N2),y,z,Nesc
  
  ! If have tabulation, use it
  IF (Halo%tabulated) THEN
    eta = MeanInverseSpeedT(vmin)
    RETURN
  END IF
  
  ! Easier to use variable names
  v0   = Halo%v0
  vobs = Halo%vobs
  vesc = Halo%vesc
  
  ! Special case: no dispersion
  ! Distribution is delta function
  IF (v0 .EQ. 0) THEN
    IF (vobs .EQ. 0d0) THEN
      eta = 0d0
    ELSE
      WHERE (vmin .LE. vobs)
        eta = 1d0 / vobs
      ELSE WHERE
        eta = 0d0
      END WHERE
    END IF
    RETURN
  END IF
  
  x    = vmin / v0
  y    = vobs / v0
  z    = vesc / v0
  Nesc = ERF(z) - 2*INVSQRTPI*z*EXP(-z**2)
  
  ! Special case: no relative motion by observer
  !   eta = 2/(sqrt(pi) Nesc v0) [e^{-x^2} - e^{-z^2}]
  ! Note: EXP2(a,b) = e^b - e^a
  IF (y .EQ. 0d0) THEN
    WHERE (x .LE. z)
      eta = 2*INVSQRTPI/(Nesc*v0) * EXP2(-z**2,-x**2)
    ELSE WHERE
      eta = 0d0
    END WHERE
    RETURN
  END IF
  
  ! Special case: no finite cutoff (vesc is effectively infinite)
  IF (z .GT. 25d0) THEN
    eta = ERF2(x-y,x+y) / (2*vobs)
    RETURN
  END IF
  
  ! General case.
  ! See e.g. Savage, Freese & Gondolo, PRD 74, 043531 (2006)
  ! [astrop-ph/0607121]; use arxiv version as PRD version has type-
  ! setting issues in the formula.
  ! Note: ERF2(a,b) = ERF(b) - ERF(a)
  ! Separate y < z & y > z cases to make easier use of WHERE statements.
  IF (y .LT. z) THEN
    WHERE (x .LT. z-y)
      eta = 1d0 / (2*Nesc*vobs) * (ERF2(x-y,x+y) - 4*INVSQRTPI*y*EXP(-z**2))
    ELSE WHERE (x .LT. z+y)
      eta = 1d0 / (2*Nesc*vobs) * (ERF2(x-y,z) - 2*INVSQRTPI*(z+y-x)*EXP(-z**2))
    ELSE WHERE
      eta = 0d0
    END WHERE
  ELSE
    WHERE (x .LT. y-z)
      eta = 1d0 / vobs
    ELSE WHERE (x .LT. y+z)
      eta = 1d0 / (2*Nesc*vobs) * (ERF2(x-y,z) - 2*INVSQRTPI*(z+y-x)*EXP(-z**2))
    ELSE WHERE
      eta = 0d0
    END WHERE
  END IF
  
END FUNCTION


!-----------------------------------------------------------------------
! Calculates the mean inverse speed (eta) [s/km] for the given vmin,
! with eta define as:
!     eta(vmin) = \int_{|v|>vmin} d^3v 1/|v| f(v)
! using the stored tabulation rather than the explicit calculation.
! 
! Input arguments:
!   vmin        The minimum speed in the eta integral [km/s]
! 
ELEMENTAL FUNCTION MeanInverseSpeedT(vmin) RESULT(eta)
  IMPLICIT NONE
  REAL*8 :: eta
  REAL*8, INTENT(IN) :: vmin
  INTEGER :: K
  REAL*8 :: f
  
  IF (.NOT. Halo%tabulated .OR. (Halo%Nvmin .LE. 0)) THEN
    eta = 0d0
    RETURN
  END IF
  
  K = BSearch(Halo%Nvmin,Halo%vmin,vmin)
  
  IF (K .LE. 0) THEN
    eta = Halo%eta(1)
  ELSE IF (K .GE. Halo%Nvmin) THEN
    IF (vmin .EQ. Halo%vmin(Halo%Nvmin)) THEN
      eta = Halo%eta(Halo%Nvmin)
    ELSE
      eta = 0d0
    END IF
  ELSE IF (Halo%vmin(K) .EQ. Halo%vmin(K+1)) THEN
    eta = Halo%eta(K)
  ELSE
    f = (vmin-Halo%vmin(K)) / (Halo%vmin(K+1)-Halo%vmin(K))
    eta = (1-f)*Halo%eta(K) + f*Halo%eta(K+1)
  END IF
  
END FUNCTION



!=======================================================================
! ISOTOPES/NUCLEAR ROUTINES (INCLUDING FORM FACTORS)
!=======================================================================

! ----------------------------------------------------------------------
! For the given element, fills in allocatable arrays containing isotopic
! atomic numbers (Z), atomic masses (A), mass fractions (f), and
! masses (M).  Only selected elements are available, with the arrays
! being set to zero length for the rest.
! 
! Implemented Z:
!   14  Silicon
!   18  Argon
!   32  Germanium
!   54  Xenon
! Special cases:
!   1153  Sodium Iodide (NaI)
! 
! Input argument:
!     Z          Atomic number of element
! Output arguments:
!     Niso       Number of isotopes
!     Ziso       Allocatable array (integer) of isotopes' atomic numbers (Z)
!     Aiso       Allocatable array (integer) of isotopes' atomic masses (A)
!     fiso       Allocatable array of isotopes' mass fractions
!     Miso       Allocatable array of isotopes' nuclear masses [GeV]
! 
SUBROUTINE IsotopeList(Z,Niso,Ziso,Aiso,fiso,Miso)
  IMPLICIT NONE
  INTEGER, INTENT(IN) :: Z
  INTEGER, INTENT(OUT) :: Niso
  INTEGER, ALLOCATABLE, INTENT(OUT) :: Ziso(:),Aiso(:)
  REAL*8, ALLOCATABLE, INTENT(OUT) :: fiso(:),Miso(:)
  
  ! Set Z, A & f.  Nuclear masses will be calculated later.
  SELECT CASE (Z)
  ! Silicon
  CASE (14)
    Niso = 3
    ALLOCATE(Ziso(Niso),Aiso(Niso),fiso(Niso),Miso(Niso))
    Ziso = (/14,14,14/)
    Aiso = (/28,29,30/)
    fiso = (/0.9187d0,0.0483d0,0.0329d0/)
  ! Argon
  CASE (18)
    Niso = 3
    ALLOCATE(Ziso(Niso),Aiso(Niso),fiso(Niso),Miso(Niso))
    Ziso = (/18,18,18/)
    Aiso = (/36,38,40/)
    fiso = (/0.0030d0,0.0006d0,0.9964d0/)
  ! Germanium
  CASE (32)
    Niso = 5
    ALLOCATE(Ziso(Niso),Aiso(Niso),fiso(Niso),Miso(Niso))
    Ziso = (/32,32,32,32,32/)
    Aiso = (/70,72,73,74,76/)
    fiso = (/0.1961d0,0.2704d0,0.0779d0,0.3738d0,0.0818d0/)
  ! Xenon
  CASE (54)
    Niso = 9
    ALLOCATE(Ziso(Niso),Aiso(Niso),fiso(Niso),Miso(Niso))
    Ziso = (/ 54, 54, 54, 54, 54, 54, 54, 54, 54/)
    Aiso = (/124,126,128,129,130,131,132,134,136/)
    fiso = (/0.0009d0,0.0009d0,0.0186d0,0.2592d0,0.0403d0,0.2117d0,0.2703d0,0.1064d0,0.0917d0/)
  ! Sodium Iodide (NaI)
  ! For mass fractions: M_Na = 22.9898u, M_I = 126.9045u
  CASE (1153)
    Niso = 2
    ALLOCATE(Ziso(Niso),Aiso(Niso),fiso(Niso),Miso(Niso))
    Ziso = (/11, 53/)
    Aiso = (/23,127/)
    fiso = (/0.1534d0,0.8466d0/)
  CASE DEFAULT
    Niso = 0
    ALLOCATE(Ziso(Niso),Aiso(Niso),fiso(Niso),Miso(Niso))
  END SELECT
  
  ! Set nuclear mass
  Miso = IsotopeMass(Ziso,Aiso)
  
END SUBROUTINE


! ----------------------------------------------------------------------
! Determines the mass of the isotope using the semi-empirical mass
! model.  A few low mass (Z <= 2) isotopes are given explicitly to
! avoid accuracy issues in the mass model in those cases.
! 
! Input arguments:
!     Z          Atomic number
!     A          Mass number
! Returns in [GeV].
! 
ELEMENTAL FUNCTION IsotopeMass(Z,A) RESULT(M)
  IMPLICIT NONE
  REAL*8 :: M
  INTEGER, INTENT(IN) :: Z,A
  REAL*8 :: A0,E
  ! 
  ! The semi-empirical mass formula will be used to calculate nuclear
  ! masses, but a few low-mass cases are given explicitly. [GeV]
  REAL*8, PARAMETER :: H2_MASS  = 1.8756129d0  ! Deuteron
  REAL*8, PARAMETER :: H3_MASS  = 2.8089210d0  ! Tritium ("triton")
  REAL*8, PARAMETER :: HE3_MASS = 2.8083915d0  ! Helium-3 ("helion")
  REAL*8, PARAMETER :: HE4_MASS = 3.7273792d0  ! Alpha
  ! 
  ! Semi-empirical mass formula constants
  !   m = Z*Mp + (A-Z)*Mn                 proton+neutron contributions
  !       - aV*A + aS*A^(2/3)             volume & surface terms
  !       + aC*Z^2/A^(1/3)                Coulomb term
  !       + aA*(A-2Z)^2/A                 Pauli (asymmetry) term
  !       + eps_{Z,A} * aP/A^(1/2)        pairing term
  ! where eps_{Z,A} is 0 if A is odd and (-1)^(Z+1) otherwise.
  ! The values below [GeV] are taken from Rohlf (1994).
  REAL*8, PARAMETER :: SEMF_AV = 0.01575d0
  REAL*8, PARAMETER :: SEMF_AS = 0.0178d0
  REAL*8, PARAMETER :: SEMF_AC = 0.000711d0
  REAL*8, PARAMETER :: SEMF_AA = 0.0237d0
  REAL*8, PARAMETER :: SEMF_AP = 0.01118d0

  ! Bad cases
  IF ((A .LE. 0) .OR. (Z .LT. 0)) THEN
    M = 0d0
    RETURN
  END IF
  
  ! Special cases: all natural and/or commonly used isotopes
  ! with Z <= 2 (i.e. nucleons, hydrogen, helium)
  IF (A .EQ. 1) THEN
    IF (Z .EQ. 0) THEN
      M = NEUTRON_MASS
      RETURN
    ELSE IF (Z .EQ. 1) THEN
      M = PROTON_MASS
      RETURN
    END IF
  ELSE IF ((A .EQ. 2) .AND. (Z .EQ. 1)) THEN
    M = H2_MASS
    RETURN
  ELSE IF (A .EQ. 3) THEN
    IF (Z .EQ. 1) THEN
      M = H3_MASS
      RETURN
    ELSE IF (Z .EQ. 2) THEN
      M = HE3_MASS
      RETURN
    END IF
  ELSE IF ((A .EQ. 4) .AND. (Z .EQ. 2)) THEN
    M = HE4_MASS
    RETURN
  END IF
  
  ! Generic semi-empirical mass formula calculation.
  ! Here, E is binding energy.
  A0 = A      ! type conversion
  E = SEMF_AV*A - SEMF_AS*A0**(2d0/3d0) - SEMF_AC*Z**2/A0**(1d0/3d0)    &
      - SEMF_AA*(A-2*Z)**2/A0
  
  ! Pairing term
  IF (MOD(A,2) .EQ. 0) THEN
    IF (MOD(Z,2) .EQ. 0) THEN
      E = E + SEMF_AP/A0**(1d0/2d0)
    ELSE
      E = E - SEMF_AP/A0**(1d0/2d0)
    END IF
  END IF
  
  M = Z*PROTON_MASS + (A-Z)*NEUTRON_MASS - E
  
END FUNCTION


! ----------------------------------------------------------------------
! Calculates the spin-independent weighted form factor Wsi for the given
! isotope at the given momentum transfer q, defined as:
!   Wsi(+1,:) = (1/pi) Z^2 F^2(:)        ! SI proton
!   Wsi( 0,:) = (1/pi) 2*Z*(A-Z) F^2(:)  ! SI crossterm
!   Wsi(-1,:) = (1/pi) (A-Z)^2 F^2(:)    ! SI neutron
! where F^2 is the standard form factor (':' represents momentum array
! q).  Uses the Helm form factor; see:
!   Lewin & Smith, Astropart. Phys. 6, 87 (1996)  [Eqn 4.7]
! 
! Required input arguments:
!     Z,A        The atomic number and mass number of the isotope.
!     N          Number of momentum q values.
!     q          Array of size [1:N] containing momentum values [GeV].
! Required output argument:
!     W          Array of size [-1,1,1:N] to be filled with weighted
!                form factor values [unitless].
! 
PURE SUBROUTINE CalcWSI(Z,A,N,q,W)
  IMPLICIT NONE
  INTEGER, INTENT(IN) :: Z,A,N
  REAL*8, INTENT(IN) :: q(1:N)
  REAL*8, INTENT(OUT) :: W(-1:1,1:N)
  INTEGER :: K
  REAL*8 :: c,rn,qrn,qs,F2,weights(-1:1)
  REAL*8, PARAMETER :: Arn = 0.52d0
  REAL*8, PARAMETER :: S   = 0.9d0
  REAL*8, PARAMETER :: C1  = 1.23d0
  REAL*8, PARAMETER :: C2  = -0.60d0
  
  weights(+1) = Z**2      / PI
  weights( 0) = 2*Z*(A-Z) / PI
  weights(-1) = (A-Z)**2  / PI
  
  c  = C1*A**(1d0/3d0) + C2
  rn = SQRT(c**2 + (7d0/3d0)*PI**2*Arn**2 - 5*S**2)
  
  ! Helm FF:  F^2(q) = 9 [j1(q rn)/(q rn)]^2 exp(-q s)
  DO K = 1, N
    qrn = q(K)*rn / HBARC
    qs  = q(K)*S / HBARC
    ! avoid numerical issues for small q by using a Taylor expansion
    IF (qrn .LE. 0.01d0) THEN
      F2 = (1 - qrn**2*((1d0/5d0) - qrn**2*(3d0/175d0))) * EXP(-qs**2)
    ELSE
      F2 = 9 * (SIN(qrn) - qrn*COS(qrn))**2 / qrn**6 * EXP(-qs**2)
    END IF
    W(:,K) = F2 * weights
  END DO
  
END SUBROUTINE


! ----------------------------------------------------------------------
! Calculates the spin-dependent weighted form factor Wsd for the given
! isotope at the given momentum transfer q, defined as:
!   Wsd(+1,:) = 4/(2J+1) Spp(q)          ! SD proton
!   Wsd( 0,:) = 4/(2J+1) Spn(q)          ! SD crossterm
!   Wsd(-1,:) = 4/(2J+1) Snn(q)          ! SD neutron
! where J is the nuclear spin and Spp/Spn/Snn are the spin structure
! functions.  For a comprehensive review of available spin structure
! functions, see:
!   Bednyakov & Simkovic, Phys. Part. Nucl. 37, S106 (2006)
!     [hep-ph/0608097]
! Note that, in the above review, the quantities "ap" & "an" are
! actually the quantities Gp & Gn as used here, not the quantities
! ap & an as used here or in much of the other direct detection
! literature.
! 
! Xenon form factors implemented by Andre Scaffidi.
! 
! NOTE: NO OTHER SD FORM FACTORS ARE CURRENTLY IMPLEMENTED.
!       THEY ARE SIMPLY SET TO ZERO.
! 
! Required input arguments:
!     Z,A        The atomic number and mass number of the isotope.
!     N          Number of momentum q values.
!     q          Array of size [1:N] containing momentum values [GeV].
! Required output argument:
!     W          Array of size [-1,1,1:N] to be filled with weighted
!                form factor values [unitless].
! 
PURE SUBROUTINE CalcWSD(Z,A,N,q,W)
  IMPLICIT NONE
  INTEGER, INTENT(IN) :: Z,A,N
  REAL*8, INTENT(IN) :: q(1:N)
  REAL*8, INTENT(OUT) :: W(-1:1,1:N)
  REAL*8 :: J,u,uk,expu,S00,S11,S01,Spp(1:N),Spn(1:N),Snn(1:N)
  INTEGER :: I,K
  
  ! Stucture constants as calculated by Menendez et al. [1208.1094] Table 1 for
  ! Xe 129. These were taken from the 1b+2b columns where possible
  REAL*8, PARAMETER :: B_XE129 = 2.2853d0 / HBARC  ! Harmonic oscillator length [GeV^-1]
  INTEGER, PARAMETER :: NC_XE129 = 9
  REAL*8, PARAMETER :: C00_XE129(0:9) = (/ 0.054731d0, -0.146897d0,     &
      0.182479d0,   -0.128112d0, 0.0539978d0, -0.0133335d0,             &
      0.00190579d0, -1.48373d-4, 5.11732d-6,  -2.06597d-8  /)
  REAL*8, PARAMETER :: C11_XE129(0:9) = (/  0.02933d0, -0.0905396d0,    &
      0.122783d0,   -0.0912046d0, 0.0401076d0, -0.010598d0,             &
      0.00168737d0, -1.56768d-4,  7.69202d-6,  -1.48874d-7 /)
  REAL*8, PARAMETER :: C01_XE129(0:9) = (/ -0.0796645d0, 0.231997d0,    &
      -0.304198d0,   0.222024d0, -0.096693d0, 0.0251835d0,              &
      -0.00392356d0, 3.53343d-4, -1.65058d-5, 2.88576d-7 /)
  
  ! Structure constants as calculated by Menendez et al. [1208.1094] Table 1 for
  ! Xe 131, again, 1b + 2b currents used
  REAL*8, PARAMETER :: B_XE131 = 2.2905d0 / HBARC  ! Harmonic oscillator length [GeV^-1]
  INTEGER, PARAMETER :: NC_XE131 = 9
  REAL*8, PARAMETER :: C00_XE131(0:9) = (/ 0.0417889d0, -0.111171d0,    &
      0.171966d0,   -0.133219d0, 0.0633805d0, -0.0178388d0,             &
      0.00282476d0, -2.31681d-4, 7.78223d-6,  -4.49287d-10 /)
  REAL*8, PARAMETER :: C11_XE131(0:9) = (/ 0.022446d0, -0.0733931d0,    &
      0.110509d0,   -0.0868752d0, 0.0405399d0, -0.0113544d0,            &
      0.00187572d0, -1.75285d-4,  8.40043d-6,  -1.53632d-7 /)
  REAL*8, PARAMETER :: C01_XE131(0:9) = (/ -0.0608808d0, 0.181473d0,    &
      -0.272533d0,  0.211776d0, -0.0985956d0, 0.027438d0,               &
      -0.0044424d0, 3.97619d-4, -1.74758d-5,  2.55979d-7 /)
  
  ! Initialization
  ! Most isotopes have zero spin (=> zero form factor)
  J   = 0d0
  Spp = 0d0
  Snn = 0d0
  Spn = 0d0
  W   = 0d0
  
  ! Xenon -------------------------------------
  IF (Z .EQ. 54) THEN
    ! Xenon 129 ----------------
    ! From Menendez et al. [1208.1094].
    ! Functional form is Sij(u) = e^{-u} \sum_{k=0} C_{ij,k} u^k
    ! where u = q^2 b^2 / 2.
    IF (A .EQ. 129) THEN
      ! Nuclear spin
      J=0.5d0
      ! Cycle over input momenta
      DO I=1,N
        u  = 0.5d0 * (q(I)*B_XE129)**2
        uk = 1
        S00 = 0d0
        S11 = 0d0
        S01 = 0d0
        ! Fits over 0 < u < 3 (probably).
        ! Conservatively set to zero outside this region.
        IF (u .LE. 3d0) THEN
          DO K=0,NC_XE129
            S00 = S00 + uk * C00_XE129(K)
            S11 = S11 + uk * C11_XE129(K)
            S01 = S01 + uk * C01_XE129(K)
            uk = u*uk
          END DO
          expu = EXP(-u)
          S00 = expu * S00
          S11 = expu * S11
          S01 = expu * S01
        END IF
        ! Basis transformation: a0=ap+an, a1=ap-an
        Spp(I) = S00 + S11 + S01
        Snn(I) = S00 + S11 - S01
        Spn(I) = S00 - S11
      END DO
      
    ! Xenon 131 ----------------
    ! From Menendez et al. [1208.1094].
    ! Functional form is Sij(u) = e^{-u} \sum_{k=0} C_{ij,k} u^k
    ! where u = q^2 b^2 / 2.
    ELSE IF (A .EQ. 131) THEN
      ! Nuclear spin
      J=1.5d0
      ! Cycle over input momenta
      DO I=1,N
        u  = 0.5d0 * (q(I)*B_XE131)**2
        uk = 1
        S00 = 0d0
        S11 = 0d0
        S01 = 0d0
        ! Fits over 0 < u < 3 (probably).
        ! Conservatively set to zero outside this region.
        IF (u .LE. 3d0) THEN
          DO K=0,NC_XE131
            S00 = S00 + uk * C00_XE131(K)
            S11 = S11 + uk * C11_XE131(K)
            S01 = S01 + uk * C01_XE131(K)
            uk = u*uk
          END DO
          expu = EXP(-u)
          S00 = expu * S00
          S11 = expu * S11
          S01 = expu * S01
        END IF
        ! Basis transformation: a0=ap+an, a1=ap-an
        Spp(I) = S00 + S11 + S01
        Snn(I) = S00 + S11 - S01
        Spn(I) = S00 - S11
      END DO
      
    ! --------------------------
    ! Other isotopes have zero spin
    ELSE
      W = 0d0
      RETURN
    END IF
  
  ! Zero spin or unimplemented isotope ---------
  ELSE
    W = 0d0
    RETURN
  ENDIF
  
  ! Weighted form factors
  W(+1,:) = (4d0 / (2d0*J + 1d0)) * Spp    ! SD proton
  W( 0,:) = (4d0 / (2d0*J + 1d0)) * Spn    ! SD crossterm
  W(-1,:) = (4d0 / (2d0*J + 1d0)) * Snn    ! SD neutron
  
END SUBROUTINE


! ----------------------------------------------------------------------
! INTERFACE NAME: EToQ
! Calculates the momentum transfer q [GeV] corresponding to a nuclear
! recoil energy E [keV] for the given isotope mass m [GeV].
! 
! This is the scalar version (single mass and energy).
! 
! Input arguments:
!   E           Recoil energy [keV]
!   Miso        Isotope mass [GeV]
! 
ELEMENTAL FUNCTION EToQ(E,Miso) RESULT(q)
  IMPLICIT NONE
  REAL*8 :: Q
  REAL*8, INTENT(IN) :: E,Miso
  ! Factor of 1d-6 to take keV -> GeV
  q = SQRT(2*Miso*(1d-6*E))
END FUNCTION



!=======================================================================
! DETECTOR SETUP ROUTINES
!=======================================================================

! ----------------------------------------------------------------------
! Get various detector quantities.
! Note this only allows access to fixed quantities, not quantities that
! are recalculated for each WIMP (see GetRates() for that).
! 
! Optional input argument:
!   D           The DetectorRateStruct structure to extract detector
!               quantities from.  If not given, a default structure
!               (internally stored) will be used.
! Optional output arguments:
!   mass        Detector fiducial mass [kg]
!   time        Detector exposure time [day]
!   exposure    Detector exposure [kg day]
!   Nevents     Number of observed events
!   background  Average expected background events
!   Niso        Number of isotopes
!   Ziso        Allocatable integer array to be filled with isotope
!               atomic numbers. Allocated to size [1:Niso].
!   Aiso        Allocatable integer array to be filled with isotope
!               atomic mass numbers. Allocated to size [1:Niso].
!   fiso        Allocatable real array to be filled with isotope
!               mass fractions. Allocated to size [1:Niso].
!   Miso        Allocatable real array to be filled with isotope
!               nuclear masses [GeV]. Allocated to size [1:Niso].
!   NE          Number of tabulated recoil energies E
!   E           Allocatable real array to be filled with recoil energies
!               [keV]. Allocated to size [1:NE].
!   eff_file    File from which efficiencies were read
!   Neff        Number of subintervals for which efficiencies are
!               available (0 for only total interval)
!   eff         Allocatable dimension=2 array containing efficiencies
!               as a function of recoil energy. Allocated to size
!               [1:NE,0:Neff], where the first index is over recoil
!               energies and the second index is over the sub-interval
!               number (0 for the total interval).
!   Wsi,Wsd     Allocatable dimension=3 array containing weighted form
!               factors for spin-independent (SI) and spin-dependent
!               (SD) couplings.  Allocated to size [-1:1,1:NE,1:Niso].
! 
SUBROUTINE GetDetector(D,mass,time,exposure,Nevents,background,         &
                       Niso,Ziso,Aiso,fiso,Miso,NE,E,Neff,eff_file,eff, &
                       Wsi,Wsd)
  IMPLICIT NONE
  TYPE(DetectorRateStruct), INTENT(IN), TARGET, OPTIONAL :: D
  CHARACTER(LEN=*), INTENT(OUT), OPTIONAL :: eff_file
  INTEGER, INTENT(OUT), OPTIONAL :: Nevents,Niso,NE,Neff
  INTEGER, ALLOCATABLE, INTENT(OUT), OPTIONAL :: Ziso(:),Aiso(:)
  REAL*8, INTENT(OUT), OPTIONAL :: mass,time,exposure,background
  REAL*8, ALLOCATABLE, INTENT(OUT), OPTIONAL :: fiso(:),Miso(:),E(:),   &
          eff(:,:),Wsi(:,:,:),Wsd(:,:,:)
  TYPE(DetectorRateStruct), POINTER :: DP
  
  ! Get pointer to detector structure to be used: either an explicitly
  ! provided structure or the default, internally-stored structure.
  IF (PRESENT(D)) THEN
    DP => D
  ELSE
    DP => DefaultDetector
  END IF
  
  ! Exposures
  IF (PRESENT(mass))     mass     = DP%mass
  IF (PRESENT(time))     time     = DP%time
  IF (PRESENT(exposure)) exposure = DP%exposure
  
  ! Observed events and expected background events
  IF (PRESENT(Nevents))    Nevents    = DP%Nevents
  IF (PRESENT(background)) background = DP%MuBackground
  
  ! Isotope data
  IF (PRESENT(Niso)) Niso = DP%Niso
  IF (PRESENT(Ziso)) THEN
    ALLOCATE(Ziso(DP%Niso))
    Ziso = DP%Ziso
  END IF
  IF (PRESENT(Aiso)) THEN
    ALLOCATE(Aiso(DP%Niso))
    Aiso = DP%Aiso
  END IF
  IF (PRESENT(fiso)) THEN
    ALLOCATE(fiso(DP%Niso))
    fiso = DP%fiso
  END IF
  IF (PRESENT(Miso)) THEN
    ALLOCATE(Miso(DP%Niso))
    Miso = DP%Miso
  END IF
  
  ! Recoil energies
  IF (PRESENT(NE)) NE = DP%NE
  IF (PRESENT(E)) THEN
    ALLOCATE(E(DP%NE))
    E = DP%E
  END IF
  
  ! Efficiencies
  IF (PRESENT(Neff))     Neff     = DP%Neff
  IF (PRESENT(eff_file)) eff_file = DP%eff_file
  IF (PRESENT(eff)) THEN
    ALLOCATE(eff(DP%NE,0:DP%Neff))
    eff = DP%eff
  END IF
  
  ! Weighted form factors
  IF (PRESENT(Wsi)) THEN
    ALLOCATE(Wsi(-1:1,DP%NE,0:DP%Niso))
    Wsi = DP%Wsi
  END IF
  IF (PRESENT(Wsd)) THEN
    ALLOCATE(Wsd(-1:1,DP%NE,0:DP%Niso))
    Wsd = DP%Wsd
  END IF
  
END SUBROUTINE


! ----------------------------------------------------------------------
! Set various detector quantities.  Making changes will reset all
! per-WIMP calculations like rates.
! Note this only allows access to fixed quantities, or quantities that
! need be calculated only once, not quantities that are recalculated
! for each WIMP (see SetRates() for that).
! 
! Optional input/output argument:
!   D           The DetectorRateStruct structure containing detector
!               data to be modified.  If not given, a default structure
!               (internally stored) will be used.
! Optional input arguments:
!   mass        Detector fiducial mass [kg]
!   time        Detector exposure time [day]
!   exposure    Detector exposure [kg day]
!   Nevents     Number of observed events
!   background  Average expected background events
! Optional isotope-related input arguments.  Niso must be given for
! any of the arrays to be used, regardless if the number has changed.
! If Niso changes, then all of the A/Z/f arrays must be specified for
! changes to take effect (otherwise these inputs are ignored).
!   Niso        Number of isotopes
!   Ziso        Integer array of size [1:Niso] containing atomic
!               numbers.
!   Aiso        Integer array of size [1:Niso] containing atomic
!               mass numbers.
!   fiso        Array of size [1:Niso] containing isotope mass
!               fractions.
! Pre-populated isotopic abundances.
!   Zelem       If Z = 14, 18, 32, or 54, sets the isotopic composition
!               to the given element.  Z = 1153 can be used for NaI.
! Optional recoil energy tabulation arguments.  Both are required for
! changes to take effect.  NOTE: THIS SETS ALL EFFICIENCIES TO ONE
! UNLESS THE EFFICIENCIES ARE ALSO SPECIFIED.  That is necessary as the
! energy tabulation must match that of the efficiency curve(s) for
! calculations to be performed.
!   NE          Number of tabulated recoil energies E
!   E           Array of size [1:NE] containing recoil energies [keV].
! Optional efficiency curve(s) arguments.  If specified by file, the
! recoil energy tabulation will be set to that of the file.  If eff is
! given, NE and Neff must also be given for it to take effect.
!   eff_file    File from which efficiencies shoud be read.
!   eff_filename Sets the stored file name _without_ loading any data
!               from the file.
!   intervals   Specify if sub-intervals should be loaded from the
!               efficiency file (if available); otherwise, only the
!               full interval is used for calculations (default: true).
!   Neff        Number of subintervals for which efficiencies are
!               available.  Set to zero to calculate only the total
!               interval.
!   eff         Array of size [1:NE,0:Neff] containing efficiencies
!               as a function of recoil energy, where the first index
!               is over recoil energies and the second index is over
!               the sub-interval number (0 for the total interval).
!   Emin        If given, sets all efficiencies below the given energy
!               [keV] to zero, removing all contributions from recoils
!               at lower energies.  Not reversible.
! Form factor arguments.  The NE and Niso arguments must also be given
! for changes to take effect.
!   Wsi,Wsd     Arrays of size [-1:1,1:NE,1:Niso] containing weighted
!               form factors for spin-independent (SI) and spin-
!               dependent (SD) couplings.
! 
SUBROUTINE SetDetector(D,mass,time,exposure,Nevents,background,         &
                       Niso,Ziso,Aiso,fiso,Zelem,                       &
                       NE,E,eff_file,eff_filename,intervals,            &
                       Neff,eff,Emin,Wsi,Wsd)
  IMPLICIT NONE
  TYPE(DetectorRateStruct), INTENT(INOUT), TARGET, OPTIONAL :: D
  CHARACTER(LEN=*), INTENT(IN), OPTIONAL :: eff_file,eff_filename
  LOGICAL, INTENT(IN), OPTIONAL :: intervals
  INTEGER, INTENT(IN), OPTIONAL :: Nevents,Niso,Zelem,NE,Neff
  !INTEGER, INTENT(IN), OPTIONAL :: Ziso(Niso),Aiso(Niso)
  INTEGER, INTENT(IN), OPTIONAL :: Ziso(:),Aiso(:)
  REAL*8, INTENT(IN), OPTIONAL :: mass,time,exposure,background,Emin
  !REAL*8, INTENT(IN), OPTIONAL :: fiso(Niso),E(NE),eff(NE,0:Neff),      &
  !        Wsi(-1:1,NE,Niso),Wsd(-1:1,NE,Niso)
  REAL*8, INTENT(IN), OPTIONAL :: fiso(:),E(:),eff(:,0:),               &
          Wsi(-1:,:,:),Wsd(-1:,:,:)
  TYPE(DetectorRateStruct), POINTER :: DP
  LOGICAL :: intervals0,iso_change,E_change,eff_change
  INTEGER :: Niso0,NE0,Neff0,KE,Kiso
  REAL*8 :: f
  
  ! Get pointer to detector structure to be used: either an explicitly
  ! provided structure or the default, internally-stored structure.
  IF (PRESENT(D)) THEN
    DP => D
  ELSE
    DP => DefaultDetector
  END IF
  
  ! Indicate if quantities changed, indicating need for
  ! array resizing and initialization
  iso_change = .FALSE.
  E_change   = .FALSE.
  eff_change = .FALSE.
  
  ! Initial array sizes
  Niso0 = DP%Niso
  NE0   = DP%NE
  Neff0 = DP%Neff
  
  ! Include sub-intervals?
  intervals0 = .TRUE.
  IF (PRESENT(intervals)) intervals0 = intervals
  
  ! Exposures
  IF (PRESENT(mass)) THEN
    DP%mass     = mass
    DP%exposure = DP%mass * DP%time
  END IF
  IF (PRESENT(time)) THEN
    DP%time     = time
    DP%exposure = DP%mass * DP%time
  END IF
  IF (PRESENT(exposure)) THEN
    DP%mass     = -1d0
    DP%time     = -1d0
    DP%exposure = exposure
  END IF
  
  ! Observed events and expected background events
  IF (PRESENT(Nevents))    DP%Nevents      = Nevents
  IF (PRESENT(background)) DP%MuBackground = background
  
  ! Isotope data
  IF (PRESENT(Niso)) THEN
    IF (Niso .EQ. DP%Niso) THEN
      IF (PRESENT(Ziso)) THEN
        DP%Ziso = Ziso(1:Niso)
        iso_change = .TRUE.
      END IF
      IF (PRESENT(Aiso)) THEN
        DP%Aiso = Aiso(1:Niso)
        iso_change = .TRUE.
      END IF
      IF (PRESENT(fiso)) THEN
        DP%fiso = fiso(1:Niso)
        iso_change = .TRUE.
      END IF
      IF (iso_change) THEN
        DP%Miso = IsotopeMass(DP%Ziso,DP%Aiso)
      END IF
    ELSE IF (PRESENT(Ziso) .AND. PRESENT(Aiso) .AND. PRESENT(fiso)) THEN
      IF (ALLOCATED(DP%Ziso)) DEALLOCATE(DP%Ziso)
      IF (ALLOCATED(DP%Aiso)) DEALLOCATE(DP%Aiso)
      IF (ALLOCATED(DP%fiso)) DEALLOCATE(DP%fiso)
      IF (ALLOCATED(DP%Miso)) DEALLOCATE(DP%Miso)
      ALLOCATE(DP%Ziso(Niso),DP%Aiso(Niso),DP%fiso(Niso),DP%Miso(Niso))
      DP%Niso = Niso
      DP%Ziso = Ziso(1:Niso)
      DP%Aiso = Aiso(1:Niso)
      DP%fiso = fiso(1:Niso)
      DP%Miso = IsotopeMass(DP%Ziso,DP%Aiso)
      iso_change = .TRUE.
    END IF
  END IF
  IF (PRESENT(Zelem)) THEN
    CALL IsotopeList(Zelem,DP%Niso,DP%Ziso,DP%Aiso,DP%fiso,DP%Miso)
    iso_change = .TRUE.
  END IF
  
  ! Energy and/or efficiencies
  IF (PRESENT(NE)) THEN
    IF (PRESENT(E)) THEN
      IF (ALLOCATED(DP%E) .AND. (NE .NE. DP%NE)) DEALLOCATE(DP%E)
      IF (.NOT. ALLOCATED(DP%E)) ALLOCATE(DP%E(NE))
      DP%NE = NE
      DP%E = E(1:NE)
      E_change = .TRUE.
    END IF
    IF (PRESENT(Neff) .AND. PRESENT(eff)) THEN
      DP%Neff = Neff
      IF (ALLOCATED(DP%eff)) DEALLOCATE(DP%eff)
      ALLOCATE(DP%eff(NE,0:Neff))
      DP%eff = eff(1:NE,0:Neff)
      eff_change = .TRUE.
    END IF
    ! Bad case: E changed, but eff was not (and will not be) updated.
    ! Set to 100% efficiency, without sub-intervals.  Useful for
    ! raw rate calculations.
    IF (E_change .AND. .NOT. eff_change .AND. .NOT. PRESENT(eff_file)) THEN
      IF (ALLOCATED(DP%eff)) DEALLOCATE(DP%eff)
      DP%Neff = 0
      ALLOCATE(DP%eff(NE,0:0))
      DP%eff = 1d0
    END IF
  END IF
  
  ! Load efficiencies from file
  IF (PRESENT(eff_file)) THEN
    IF (eff_file .NE. '') THEN
      DP%eff_file = eff_file
      CALL LoadEfficiencyFile(eff_file,DP%NE,DP%E,DP%Neff,DP%eff,       &
                              intervals0)
      E_change   = .TRUE.
      eff_change = .TRUE.
    END IF
  END IF
  
  ! Saved efficiency file name
  IF (PRESENT(eff_filename)) DP%eff_file = eff_filename
  
  ! Apply threshold cut.
  ! We move all E < Emin tabulation points to Emin and set the
  ! efficiency of the final one to an interpolated value to
  ! maintain the original shape over any partially cropped bin.
  IF (PRESENT(Emin)) THEN
    KE = BSearch(DP%NE,DP%E,Emin)
    IF (KE .GE. DP%NE) THEN
      DP%E   = Emin
      DP%eff = 0d0
      E_change   = .TRUE.
      eff_change = .TRUE.
    ELSE IF (KE .GE. 1) THEN
      IF (DP%E(KE) .LT. DP%E(KE+1)) THEN
        f = (Emin - DP%E(KE)) / (DP%E(KE+1) - DP%E(KE))
      ELSE
        f = 1d0
      END IF
      DP%E(:KE) = Emin
      DP%eff(KE,:) = (1d0-f)*DP%eff(KE,:) + f*DP%eff(KE+1,:)
      DP%eff(1:KE-1,:) = 0d0
      E_change   = .TRUE.
      eff_change = .TRUE.
    END IF
  END IF
  
  ! Weighted form factors (SI)
  IF (PRESENT(NE) .AND. PRESENT(Niso) .AND. PRESENT(Wsi)) THEN
    IF (ALLOCATED(DP%Wsi)) DEALLOCATE(DP%Wsi)
    ALLOCATE(DP%Wsi(-1:1,NE,Niso))
    DP%Wsi = Wsi(-1:1,1:NE,1:Niso)
  ELSE IF ((iso_change .OR. E_change) .AND. (DP%Niso .GE. 0)            &
           .AND. (DP%NE .GE. 0)) THEN
    IF (ALLOCATED(DP%Wsi)) DEALLOCATE(DP%Wsi)
    ALLOCATE(DP%Wsi(-1:1,DP%NE,DP%Niso))
    DO Kiso = 1,DP%Niso
      CALL CalcWSI(DP%Ziso(Kiso),DP%Aiso(Kiso),DP%NE,                   &
                   EToQ(DP%E,DP%Miso(Kiso)),DP%Wsi(:,:,Kiso))
    END DO
  END IF
  
  ! Weighted form factors (SD)
  IF (PRESENT(NE) .AND. PRESENT(Niso) .AND. PRESENT(Wsd)) THEN
    IF (ALLOCATED(DP%Wsd)) DEALLOCATE(DP%Wsd)
    ALLOCATE(DP%Wsd(-1:1,NE,Niso))
    DP%Wsd = Wsd(-1:1,1:NE,1:Niso)
  ELSE IF ((iso_change .OR. E_change) .AND. (DP%Niso .GE. 0)            &
           .AND. (DP%NE .GE. 0)) THEN
    IF (ALLOCATED(DP%Wsd)) DEALLOCATE(DP%Wsd)
    ALLOCATE(DP%Wsd(-1:1,DP%NE,DP%Niso))
    DO Kiso = 1,DP%Niso
      CALL CalcWSD(DP%Ziso(Kiso),DP%Aiso(Kiso),DP%NE,                   &
                   EToQ(DP%E,DP%Miso(Kiso)),DP%Wsd(:,:,Kiso))
    END DO
  END IF
  
  ! Resize halo velocity arrays if necessary
  IF ((iso_change .OR. E_change) .AND. (DP%Niso .GE. 0)                 &
      .AND. (DP%NE .GE. 0)) THEN
    IF (ALLOCATED(DP%vmin)) DEALLOCATE(DP%vmin)
    ALLOCATE(DP%vmin(DP%NE,DP%Niso))
    IF (ALLOCATED(DP%eta))  DEALLOCATE(DP%eta)
    ALLOCATE(DP%eta(DP%NE,DP%Niso))
  END IF
  
  ! Resize rate arrays if necessary
  IF ((E_change .OR. eff_change) .AND. (DP%NE .GE. 0)                   &
      .AND. (DP%Neff .GE. 0)) THEN
    IF (ALLOCATED(DP%dRdEsi0)) DEALLOCATE(DP%dRdEsi0)
    ALLOCATE(DP%dRdEsi0(-1:1,DP%NE))
    IF (ALLOCATED(DP%dRdEsd0)) DEALLOCATE(DP%dRdEsd0)
    ALLOCATE(DP%dRdEsd0(-1:1,DP%NE))
    IF (ALLOCATED(DP%Rsi0)) DEALLOCATE(DP%Rsi0)
    ALLOCATE(DP%Rsi0(-1:1,0:DP%Neff))
    IF (ALLOCATED(DP%Rsd0)) DEALLOCATE(DP%Rsd0)
    ALLOCATE(DP%Rsd0(-1:1,0:DP%Neff))
    IF (ALLOCATED(DP%dRdEsi)) DEALLOCATE(DP%dRdEsi)
    ALLOCATE(DP%dRdEsi(DP%NE))
    IF (ALLOCATED(DP%dRdEsd)) DEALLOCATE(DP%dRdEsd)
    ALLOCATE(DP%dRdEsd(DP%NE))
    IF (ALLOCATED(DP%dRdE)) DEALLOCATE(DP%dRdE)
    ALLOCATE(DP%dRdE(DP%NE))
    IF (ALLOCATED(DP%Rsi)) DEALLOCATE(DP%Rsi)
    ALLOCATE(DP%Rsi(0:DP%Neff))
    IF (ALLOCATED(DP%Rsd)) DEALLOCATE(DP%Rsd)
    ALLOCATE(DP%Rsd(0:DP%Neff))
    IF (ALLOCATED(DP%R)) DEALLOCATE(DP%R)
    ALLOCATE(DP%R(0:DP%Neff))
  END IF
  
  ! Resize event arrays if necessary
  IF (eff_change .AND. (DP%Neff .GE. 0)) THEN
    IF (ALLOCATED(DP%MuSignalSI0)) DEALLOCATE(DP%MuSignalSI0)
    ALLOCATE(DP%MuSignalSI0(-1:1,0:DP%Neff))
    IF (ALLOCATED(DP%MuSignalSD0)) DEALLOCATE(DP%MuSignalSD0)
    ALLOCATE(DP%MuSignalSD0(-1:1,0:DP%Neff))
    IF (ALLOCATED(DP%MuSignalSI)) DEALLOCATE(DP%MuSignalSI)
    ALLOCATE(DP%MuSignalSI(0:DP%Neff))
    IF (ALLOCATED(DP%MuSignalSD)) DEALLOCATE(DP%MuSignalSD)
    ALLOCATE(DP%MuSignalSD(0:DP%Neff))
    IF (ALLOCATED(DP%MuSignal)) DEALLOCATE(DP%MuSignal)
    ALLOCATE(DP%MuSignal(0:DP%Neff))
  END IF
  
  ! Set all calculable quantities to zero
  IF ((iso_change .OR. E_change .OR. eff_change)                        &
       .AND. (DP%Niso .GE. 0) .AND. (DP%NE .GE. 0)                      &
       .AND. (DP%Neff .GE. 0)) THEN
    DP%vmin        = 0d0
    DP%eta         = 0d0
    DP%dRdEsi0     = 0d0
    DP%dRdEsd0     = 0d0
    DP%Rsi0        = 0d0
    DP%Rsd0        = 0d0
    DP%dRdEsi      = 0d0
    DP%dRdEsd      = 0d0
    DP%dRdE        = 0d0
    DP%Rsi         = 0d0
    DP%Rsd         = 0d0
    DP%R           = 0d0
    DP%MuSignalSI0 = 0d0
    DP%MuSignalSD0 = 0d0
    DP%MuSignalSI  = 0d0
    DP%MuSignalSD  = 0d0
    DP%MuSignal    = 0d0
  END IF
  
END SUBROUTINE


!-----------------------------------------------------------------------
! Initializes the detector.
! Simply sets the detector to match that of the LUX 2013 analysis.
! If something other than this is desired, use the SetDetector()
! function after this.
! 
! Optional output argument:
!   D           The DetectorRateStruct structure to be initialized.
!               If not given, the default structure (internally stored)
!               will be initialized.
! Optional input argument:
!   intervals   Specify if sub-intervals should be loaded; otherwise,
!               only the full interval is used for calculations
!               (default: true).
! 
SUBROUTINE InitDetector(D,intervals)
  IMPLICIT NONE
  TYPE(DetectorRateStruct), INTENT(OUT), TARGET, OPTIONAL :: D
  LOGICAL, INTENT(IN), OPTIONAL :: intervals
  TYPE(DetectorRateStruct), POINTER :: DP
  LOGICAL :: intervals0
  
  ! Get pointer to detector structure to be used: either an explicitly
  ! provided structure or the default, internally-stored structure.
  IF (PRESENT(D)) THEN
    DP => D
  ELSE
    DP => DefaultDetector
  END IF
  
  ! Include sub-intervals?
  intervals0 = .TRUE.
  IF (PRESENT(intervals)) intervals0 = intervals
  
  ! Set to LUX 2013 analysis
  CALL LUX_2013_InitTo(DP,intervals0)
  
END SUBROUTINE


!-----------------------------------------------------------------------
! Initializes the detector setup from command-line parameters.
! 
! Possible options:
!   --mass=<value>       ! Detector fiducial mass [kg]
!   --time=<value>       ! Detector exposure time [day]
!   --exposure=<value>   ! Detector exposure [kg day]
!   --events=<N>         ! Number of observed events
!   --background=<value> ! Average expected background events
! Isotope-related options specifying the atomic number, atomic mass
! number, and mass fraction of the isotopes.  Must specify all three
! and they must have the same number of isotopes for changes to take
! effect:
!   --isotope-Z=<Z1>,<Z2>,<Z3>,...
!   --isotope-A=<A1>,<A2>,<A3>,...
!   --isotope-f=<f1>,<f2>,<f3>,...
! Shortcuts for specific materials by atomic number (using natural
! abundances):
!   --element-Z=<Z>
! Shortcuts for specific materials by name (using natural abundances):
!   --argon
!   --germanium
!   --sodium-iodide
!   --silicon
!   --xenon
! Efficiency related options:
!   --file=<file>        ! File from which tabulated efficiencies should be read.
!                        ! First column is E [keV], any following consecutive
!                        ! columns containing values above 1.0 are ignored, and
!                        ! the remaining columns are taken to be efficiency curves.
!                        ! First efficiency is for full range, remaining efficiencies
!                        ! are for sub-intervals.  If not given, a default is used.
!   --no-intervals       ! Disable use of sub-intervals, even if available in file.
!   --Emin=<value>       ! If given, sets all efficiencies below the given energy
!                        ! [keV] to zero, removing all contributions from recoils
!                        ! at lower energies.  Not reversible.
! 
! Optional output argument:
!   D           The DetectorRateStruct structure to be initialized.
!               If not given, the default structure (internally stored)
!               will be initialized.
! Optional input arguments:
!   eff_file    File from which efficiencies shoud be read.  If not
!               given or set to '', an internal default will be used
!               (the LUX 2013 result).  Non-empty value takes precedence
!               over --file option.
!   intervals   Specify if sub-intervals should be loaded from the
!               efficiency file (if available); otherwise, only the
!               full interval is used for calculations (default: true).
! 
SUBROUTINE InitDetectorCL(D,eff_file,intervals)
  IMPLICIT NONE
  TYPE(DetectorRateStruct), INTENT(OUT), TARGET, OPTIONAL :: D
  CHARACTER(LEN=*), INTENT(IN), OPTIONAL :: eff_file
  LOGICAL, INTENT(IN), OPTIONAL :: intervals
  TYPE(DetectorRateStruct), POINTER :: DP
  CHARACTER(LEN=1024) :: eff_file0
  LOGICAL :: intervals0
  INTEGER :: Nevents,Niso,Zelem,NE,Neff,N1,N2,N3
  INTEGER, ALLOCATABLE :: Ziso(:),Aiso(:)
  REAL*8 :: mass,time,exposure,background,Emin
  REAL*8, ALLOCATABLE :: fiso(:),E(:),eff(:,:)
  
  ! Get pointer to detector structure to be used: either an explicitly
  ! provided structure or the default, internally-stored structure.
  IF (PRESENT(D)) THEN
    DP => D
  ELSE
    DP => DefaultDetector
  END IF
  
  Niso = -1
  NE   = -1
  Neff = -1
  
  ! File name ('' for internal default)
  IF (.NOT. GetLongArgString('file',eff_file0)) eff_file0 = ''
  IF (PRESENT(eff_file)) THEN
    IF (TRIM(eff_file) .NE. '') eff_file0 = eff_file
  END IF
  
  ! Include sub-intervals?
  intervals0 = .NOT. GetLongArg('no-intervals')
  IF (PRESENT(intervals)) intervals0 = intervals
  
  ! Experiment-specific settings
  ! LUX 2013 result
  IF (GetLongArg('LUX-2013')) THEN
    CALL LUX_2013_InitTo(DP,intervals0)
  ! DARWIN proposal, xenon-based (as of 2014)
  ELSE IF (GetLongArg('DARWIN-Xe-2014')) THEN
    CALL DARWIN_Xe_2014_InitTo(DP,intervals0)
  ! DARWIN proposal, argon-based (as of 2014)
  ELSE IF (GetLongArg('DARWIN-Ar-2014')) THEN
    CALL DARWIN_Ar_2014_InitTo(DP,intervals0)
  ! Defaults (LUX 2013)
  ELSE
    CALL LUX_2013_InitTo(DP,intervals0)
    CALL SetDetector(eff_filename='[LUX 2013 (default)]')
  END IF
  
  ! If given, load efficiency file
  IF (TRIM(eff_file0) .NE. '') THEN
    CALL LoadEfficiencyFile(eff_file0,NE,E,Neff,eff,intervals0)
    CALL SetDetector(DP,eff_file=eff_file0,intervals=intervals0)
  END IF
  
  ! Update isotopes
  IF (GetLongArgIntegers('isotope-Z',Ziso,N1)                           &
      .AND. GetLongArgIntegers('isotope-A',Aiso,N2)                     &
      .AND. GetLongArgReals('isotope-f',fiso,N3)) THEN
    IF ((N1 .EQ. N2) .AND. (N1 .EQ. N3)) THEN
      Niso = N1
      CALL SetDetector(DP,Niso=Niso,Ziso=Ziso,Aiso=Aiso,fiso=fiso)
    END IF
  ELSE IF (GetLongArgInteger('element-Z',Zelem)) THEN
    CALL SetDetector(DP,Zelem=Zelem)
  ELSE IF (GetLongArg('argon')) THEN
    CALL SetDetector(DP,Zelem=18)
  ELSE IF (GetLongArg('germanium')) THEN
    CALL SetDetector(DP,Zelem=32)
  ELSE IF (GetLongArg('silicon')) THEN
    CALL SetDetector(DP,Zelem=14)
  ELSE IF (GetLongArg('xenon')) THEN
    CALL SetDetector(DP,Zelem=54)
  ELSE IF (GetLongArg('sodium-iodide')) THEN
    CALL SetDetector(DP,Zelem=1153)
  END IF
  
  ! Update exposures
  IF (GetLongArgReal('mass',mass))         CALL SetDetector(DP,mass=mass)
  IF (GetLongArgReal('time',time))         CALL SetDetector(DP,time=time)
  IF (GetLongArgReal('exposure',exposure)) CALL SetDetector(DP,exposure=exposure)
  
  ! Update observed events and expected background events
  IF (GetLongArgInteger('events',Nevents))     CALL SetDetector(DP,Nevents=Nevents)
  IF (GetLongArgReal('background',background)) CALL SetDetector(DP,background=background)
  
  ! Set an energy threshold
  IF (GetLongArgReal('Emin',Emin)) THEN
    CALL SetDetector(DP,Emin=Emin)
  END IF
  
END SUBROUTINE


! ----------------------------------------------------------------------
! Sets the efficiencies to those found in the given file, presumably
! an efficiency or efficiencies file generated by TPCMC.
! 
! Required input argument:
!     file       The file to load efficiencies from.
! Required output arguments:
!     NE         Number of recoil energy tabulation points loaded
!     E          Allocatable array to contain energies [keV].
!                Allocated to size [1:NE].
!     Neff       Number of sub-interval efficiencies loaded (0 if
!                intervals=false).
!     eff        Allocatable array to contain efficiency curves for
!                the total range and each sub-interval (if
!                intervals=true).  Allocated to size [1:NE,0:Neff].
! Required input argument:
!     intervals  Indicates if sub-intervals should be included.
! 
SUBROUTINE LoadEfficiencyFile(file,NE,E,Neff,eff,intervals)
  IMPLICIT NONE
  CHARACTER(LEN=*), INTENT(IN) :: file
  LOGICAL, INTENT(IN) :: intervals
  INTEGER, INTENT(OUT) :: NE,Neff
  REAL*8, ALLOCATABLE, INTENT(OUT) :: E(:),eff(:,:)
  LOGICAL :: status
  INTEGER :: Kcol,Keff,Nrow,Ncol,Nvalid
  REAL*8, ALLOCATABLE :: data(:,:)
  
  ! Load table from file
  CALL LoadTable(file=file,Nrow=Nrow,Ncol=Ncol,data=data,status=status)
  IF ((.NOT. status) .OR. (Ncol .LT. 2)) THEN
    WRITE(0,*) 'ERROR: Failed to load data from file ' // TRIM(file) // '.'
    STOP
  END IF
  
  ! Energies
  NE = Nrow
  ALLOCATE(E(NE))
  E = data(:,1)
  
  ! Find number of valid efficiency columns
  Nvalid = 0
  DO Kcol = 2,Ncol
    IF (ALL(data(:,Kcol) .GE. 0d0) .AND. ALL(data(:,Kcol) .LE. 1.00001d0)) Nvalid = Nvalid + 1
  END DO
  IF (Nvalid .LE. 0) THEN
    WRITE(0,*) 'ERROR: Failed to find valid data in file ' // TRIM(file) // '.'
    STOP
  END IF
  
  ! Determine number of efficiencies to use
  IF (intervals) THEN
    Neff = Nvalid
  ELSE
    Neff = 0
  END IF
  ALLOCATE(eff(NE,0:Neff))
  
  ! Now fill efficiencies
  Keff = 0
  DO Kcol = 2,Ncol
    IF (ALL(data(:,Kcol) .GE. 0d0) .AND. ALL(data(:,Kcol) .LE. 1.00001d0) &
        .AND. (Keff .LE. Neff)) THEN
      eff(:,Keff) = data(:,Kcol)
      Keff = Keff + 1
    END IF
  END DO
  
END SUBROUTINE



!=======================================================================
! RATE ROUTINES
!=======================================================================

! ----------------------------------------------------------------------
! Get various rate quantities.  These require CalcRates() to have been
! run first.
! 
! Optional input argument:
!   D           The DetectorRateStruct structure to extract rate
!               quantities from.  If not given, a default structure
!               (internally stored) will be used.
! Optional output arguments:
!   Nevents     Number of observed events
!   background  Average expected background events
!   signal      Average expected signal events
!   signal_si   Average expected spin-independent signal events
!   signal_sd   Average expected spin-dependent signal events
!   rate        Signal rate [cpd/kg]
!   rate_si     Spin-independent signal rate [cpd/kg]
!   rate_sd     Spin-dependent signal rate [cpd/kg]
! Optional sub-interval/bin arguments.  Arrays will be allocated to
! size [1:Nbins]:
!   Nbins       Number of bins/intervals
!   binsignal   Allocatable array to be filled with average expected
!               signal in each bin.  Allocated to size [1:Nbins].
!               The sum of all bins is equal to 'signal'.
!   binsignal_si,binsignal_sd
!               Same as 'binsignal' but for only spin-independent or
!               spin-dependent events, respectively.
!   binrate      Allocatable array to be filled with recoil rate in
!               each bin.  Allocated to size [1:Nbins].
!               The sum of all bins is equal to 'rate'.
!   binrate_si,binrate_sd
!               Same as 'binrate' but for only spin-independent or
!               spin-dependent events, respectively.
!   
SUBROUTINE GetRates(D,Nevents,background,signal,signal_si,signal_sd,    &
                    rate,rate_si,rate_sd,Nbins,binsignal,binsignal_si,  &
                    binsignal_sd,binrate,binrate_si,binrate_sd)
  IMPLICIT NONE
  TYPE(DetectorRateStruct), INTENT(IN), TARGET, OPTIONAL :: D
  INTEGER, INTENT(OUT), OPTIONAL :: Nevents,Nbins
  REAL*8, INTENT(OUT), OPTIONAL :: background,signal,signal_si,signal_sd,&
          rate,rate_si,rate_sd
  REAL*8, ALLOCATABLE, INTENT(OUT), OPTIONAL :: binsignal(:),           &
          binsignal_si(:),binsignal_sd(:),binrate(:),binrate_si(:),     &
          binrate_sd(:)
  TYPE(DetectorRateStruct), POINTER :: DP
  INTEGER :: Neff
  
  ! Get pointer to detector structure to be used: either an explicitly
  ! provided structure or the default, internally-stored structure.
  IF (PRESENT(D)) THEN
    DP => D
  ELSE
    DP => DefaultDetector
  END IF
  
  Neff = DP%Neff
  
  ! Observed events and expected background events
  IF (PRESENT(Nevents))    Nevents    = DP%Nevents
  IF (PRESENT(background)) background = DP%MuBackground
  
  ! Signal events
  IF (PRESENT(signal))    signal    = DP%MuSignal(0)
  IF (PRESENT(signal_si)) signal_si = DP%MuSignalSI(0)
  IF (PRESENT(signal_sd)) signal_sd = DP%MuSignalSD(0)
  
  ! Signal rates
  IF (PRESENT(rate))    rate    = DP%R(0)
  IF (PRESENT(rate_si)) rate_si = DP%Rsi(0)
  IF (PRESENT(rate_sd)) rate_sd = DP%Rsd(0)
  
  ! Bins
  IF (PRESENT(Nbins)) Nbins = Neff
  
  ! Signal events by bin
  IF (PRESENT(binsignal)) THEN
    ALLOCATE(binsignal(Neff))
    binsignal = DP%MuSignal(1:Neff)
  END IF
  IF (PRESENT(binsignal_si)) THEN
    ALLOCATE(binsignal_si(Neff))
    binsignal_si = DP%MuSignalSI(1:Neff)
  END IF
  IF (PRESENT(binsignal_sd)) THEN
    ALLOCATE(binsignal_sd(Neff))
    binsignal_sd = DP%MuSignalSD(1:Neff)
  END IF
  
  ! Signal rates by bin
  IF (PRESENT(binrate)) THEN
    ALLOCATE(binrate(Neff))
    binrate = DP%MuSignal(1:Neff)
  END IF
  IF (PRESENT(binrate_si)) THEN
    ALLOCATE(binrate_si(Neff))
    binrate_si = DP%MuSignalSI(1:Neff)
  END IF
  IF (PRESENT(binrate_sd)) THEN
    ALLOCATE(binrate_sd(Neff))
    binrate_sd = DP%MuSignalSD(1:Neff)
  END IF
  
END SUBROUTINE


! ----------------------------------------------------------------------
! Calculates various rate quantities using the current WIMP.
! This is the main routine intended for putting together the WIMP
! rate calculations and must be called any time the WIMP mass
! and/or couplings are modified.
! 
! Optional input/output argument:
!   D           The DetectorRateStruct structure containing detector
!               data, also where calculated rate data will be placed.
!               If not given, a default structure (internally stored)
!               will be used.
! 
SUBROUTINE CalcRates(D)
  IMPLICIT NONE
  TYPE(DetectorRateStruct), INTENT(INOUT), TARGET, OPTIONAL :: D
  TYPE(DetectorRateStruct), POINTER :: DP
  INTEGER :: Kiso,KE,Keff
  REAL*8 :: alphasi(-1:1),alphasd(-1:1)
  ! Constant used to convert units:
  !   s / (cm^3 km GeV^4)  -->  cpd/kg/keV
  ! Includes factor of hbar^2 c^4.
  REAL*8, PARAMETER :: TO_CPD_KG_KEV = 1.695e14
  
  !---------------------------------------------
  ! Differential rate is given by:
  !   dR/dE(E) = 1/(2m\mu^2) \sigma(q) \rho \eta(vmin)
  ! where q(E) = \sqrt{2ME}, vmin(E) = \sqrt{ME/2\mu^2}, and:
  !   \sigma(E) = \mu^2 (hbar c)^2 [W(1,E)*Gp^2 + W(0,E)*Gp*Gn + W(-1,E)*Gn^2]
  ! This gives:
  !   dR/dE(E) = 1/2m \rho \eta(E) [W(1)*Gp^2 + W(0)*Gp*Gn + W(-1)*Gn^2]
  ! The total differential rate is a mass-fraction weighted sum of
  ! dR/dE over different isotopes and is summed over spin-independent
  ! (SI) and spin-dependent (SD) contributions.
  ! 
  ! The weighted form factors W are defined as:
  !   Wsi(+1,E) = (1/pi) Z^2 F^2(E)        ! SI proton
  !   Wsi( 0,E) = (1/pi) 2*Z*(A-Z) F^2(E)  ! SI crossterm
  !   Wsi(-1,E) = (1/pi) (A-Z)^2 F^2(E)    ! SI neutron
  !   Wsd(+1,E) = 4/(2J+1) Spp(E)          ! SD proton
  !   Wsd( 0,E) = 4/(2J+1) Spn(E)          ! SD crossterm
  !   Wsd(-1,E) = 4/(2J+1) Snn(E)          ! SD neutron
  ! The above definitions give for the conventional SI and SD
  ! cross-sections:
  !   \sigma(E) = \mu^2 (hbar c)^2 [W(+1,E)*Gp^2 + W(0,E)*Gp*Gn + W(-1,E)*Gn^2]
  ! where Gp and Gn are the effective proton and neutron couplings
  ! in units of [GeV^-2] and \mu is the reduced mass.  In terms of
  ! more commonly used notation:
  !   SI (scalar):        G = 2f
  !   SD (axial-vector):  G = 2\sqrt{2} G_F a
  ! where G, f, and a have 'p' and 'n' subscripts.
  ! 
  ! Thes spin-independent and spin-dependent weighted form factors are
  ! tabulated over E for each of the isotopes, so the Wsi and Wsd arrays
  ! are of size of size [-1:1,1:NE,1:Niso] where NE is the number of
  ! tabulation points (energies) and Niso is the number of isotopes.
  ! As these factors are independent of the WIMP, they are calculated
  ! only once, during initialization (nothing needs to be done here).
  ! SD FORM FACTORS ARE ONLY IMPLEMENTED FOR XENON.  ALL OTHER SD FORM
  ! FACTORS ARE SET TO ZERO.
  ! 
  ! The mean inverse speed
  !   eta(E) = \int_{v>vmin} d^3v (1/v) f(v),
  ! with
  !   vmin(E) = \sqrt{M E/(2\mu^2)}
  ! the minimum WIMP speed capable of causing a nucleus to recoil with
  ! energy E, depends upon the WIMP mass, so it will need to be
  ! recalculated for every WIMP.  Both vmin and eta are tabulated over
  ! E for each of the isotopes, so the vmin and eta arrays are of size
  ! [1:NE,1:Niso].
  ! 
  ! When factoring in detector efficiencies and energy resolution, the
  ! total rate of observed events is given by:
  !   R = \int_0^\infty dE \phi(E) dR/dE(E)
  ! where \phi(E) is the efficiency/response factor defined as the
  ! fraction of events at a given energy E that will be both observed
  ! and fall within some particular S1 analysis range.  The actual
  ! observed spectrum is dR/dS1(S1), NOT the quantity \phi(E) dR/dE(E);
  ! the former cannot be directly calculated using \phi(E).  However,
  ! the definition of \phi(E) allows the integral of each spectrum to
  ! be related as we can also write the rate as:
  !   R = \int_{S1min}^{S1max} dS1 dR/dS1(S1)
  ! The \phi(E) are calculated for _specific_ ranges of S1.  As these
  ! efficiencies are independent of the WIMP, they do not need to be
  ! recalculated, which is good, since this code cannot calculate
  ! them.  Instead, the the efficiency curve(s) are loaded from a file
  ! (or uses the internal default) during the initialization stage.
  ! 
  ! The efficiency file should contain a tabulated efficiency for the
  ! full desired S1 range, but can optionally include additional
  ! tabulated efficiencies for sub-intervals of that full range.
  ! For example, LUX 2013 analyzed the S1 range [2,30] and observed one
  ! event at S1=3.1 below their nuclear recoil mean.  For the maximum
  ! gap method of determining exclusion constraints in the presence of
  ! unknown backgrounds, the expected rate in intervals between events
  ! is used, so the rate in the S1 ranges [2,3.1] and [3.1,20] must be
  ! calculated.  The default efficiencies are the \phi(E) efficiencies
  ! for these three ranges: the full range and the two sub-intervals.
  ! The tabulated efficiency array (called 'eff') is an array of size
  ! [1:NE,0:Neff] where Neff is the number of sub-intervals (0 if
  ! separate interval efficiencies are not to be used or were not
  ! available).  The second index is that of the sub-interval, with 0
  ! used for the total range.
  ! 
  ! To avoid having to deal with a variety of interpolation issues,
  ! everything is tabulated at the same energies E, given in the array
  ! E of size [1:NE].  The tabulation is taken from that of the
  ! provided efficiency file (or internal default) since efficiencies
  ! cannot be recalculated with this program (everything else can).
  ! 
  ! In the calculations below, we calculate the differential rates
  ! dR/dE(E) for each of the isotopes and each of the SI & SD couplings.
  ! Then we perform an efficiency-weighted integral over E and sum
  ! contributions together to get the total rates and expected events.
  ! The separate coupling spectra are saved to allow for possible
  ! further inspection.
  ! 
  ! To be clear, the following are required to determine the full rate:
  !   * Sum over SI & SD contributions (with a Gp^2, Gn^2, and Gp*Gn
  !     term for each)
  !   * Mass fraction weighted sum over isotope contributions
  !   * Integration over energy
  !   * All of the above possibly for multiple analysis intervals
  ! Though not necesssary, various intermediate quantities are kept
  ! to allow for possible further inspection (aside from individual
  ! isotope contributions, which are not kept).
  ! Note there is no required order for performing the sums and
  ! integrations; we take advantage of this to perform the SI/SD
  ! component sum _last_ so that the rate is very easily recalculated
  ! for different couplings.  Notably, the two cross-sections are
  ! treated as:
  !    \sigma(E) = (Gp/Gp0)^2 \sigmapp0(E) + (Gn/Gn0)^2 \sigmann0(E)
  !                + (Gp*Gn/Gp0*Gn0) \sigmapn0(E)
  ! where
  !    \sigmapp0(E) = \mu^2 (hbar c)^2 W(+1,E) * Gp0^2
  !    \sigmann0(E) = \mu^2 (hbar c)^2 W(-1,E) * Gn0^2
  !    \sigmapn0(E) = \mu^2 (hbar c)^2 W( 0,E) * Gp0*Gn0
  ! and Gp0 & Gn0 are reference couplings, which we take to be those
  ! that yield WIMP-nucleon cross-sections of 1 pb.  In that case,
  ! we can also write:
  !    \sigma(E) = (sigmap/[pb])*\sigmapp0(E) + (sigman/[pb])*\sigmann0(E)
  !                +/- \sqrt{(sigmap/[pb])(sigman/[pb])} \sigmapn0(E)
  ! with the sign of the cross-term equal to the sign of Gp*Gn.  The
  ! quantities below with names ending with 'si0' and 'sd0' have an
  ! index [-1:1] identical to that described for the W terms above,
  ! corresponds to the that quantity calculated for sigma(E) =
  ! sigmapp0(E) [+1], sigmann0(E) [-1], or sigmapn0(E) [0].
  !---------------------------------------------
  
  ! Get pointer to detector structure to be used: either an explicitly
  ! provided structure or the default, internally-stored structure.
  IF (PRESENT(D)) THEN
    DP => D
  ELSE
    DP => DefaultDetector
  END IF
  
  ! Update mean inverse speed.
  DP%vmin = EToVmin(DP%NE,DP%E,WIMP%m,DP%Niso,DP%Miso)
  DP%eta  = MeanInverseSpeed(DP%NE,DP%Niso,DP%vmin)
  
  ! Below, we will calculate reference rates for reference couplings
  ! GpSI0, GnSI0, GpSD0, & GnSD0 defined such that WIMP-nucleon
  ! cross-sections are 1 pb.  To convert between reference rates and
  ! actual rates, we multiply by alpha = G^2/G0^2.  Indices of alpha
  ! match that of the W arrays.
  alphasi(+1) = (WIMP%GpSI/WIMP%GpSI0)**2                      ! SI proton
  alphasi( 0) = (WIMP%GpSI/WIMP%GpSI0)*(WIMP%GnSI/WIMP%GnSI0)  ! SI crossterm
  alphasi(-1) = (WIMP%GnSI/WIMP%GnSI0)**2                      ! SI neutron
  alphasd(+1) = (WIMP%GpSD/WIMP%GpSD0)**2                      ! SD proton
  alphasd( 0) = (WIMP%GpSD/WIMP%GpSD0)*(WIMP%GnSD/WIMP%GnSD0)  ! SD crossterm
  alphasd(-1) = (WIMP%GnSD/WIMP%GnSD0)**2                      ! SD neutron
  
  ! Reference differential rates.
  ! Keep separate contributions from Gp^2, Gn^2, and Gp*Gn terms.
  DP%dRdEsi0 = 0d0
  DP%dRdEsd0 = 0d0
  ! Mass-fraction weighted sum over isotopes.
  DO Kiso = 1,DP%Niso
    ! SI proton term
    DP%dRdEsi0(+1,:) = DP%dRdEsi0(+1,:)                                 &
        +  DP%fiso(Kiso) / (2 * WIMP%m)                                 &
           * Halo%rho * DP%eta(:,Kiso)                                  &
           * DP%Wsi(+1,:,Kiso) * WIMP%GpSI0**2                          &
           * TO_CPD_KG_KEV
    ! SI cross term
    DP%dRdEsi0( 0,:) = DP%dRdEsi0( 0,:)                                 &
        +  DP%fiso(Kiso) / (2 * WIMP%m)                                 &
           * Halo%rho * DP%eta(:,Kiso)                                  &
           * DP%Wsi( 0,:,Kiso) * WIMP%GpSI0*WIMP%GnSI0                  &
           * TO_CPD_KG_KEV
    ! SI neutron term
    DP%dRdEsi0(-1,:) = DP%dRdEsi0(-1,:)                                 &
        +  DP%fiso(Kiso) / (2 * WIMP%m)                                 &
           * Halo%rho * DP%eta(:,Kiso)                                  &
           * DP%Wsi(-1,:,Kiso) * WIMP%GnSI0**2                          &
           * TO_CPD_KG_KEV
    ! SD proton term
    DP%dRdEsd0(+1,:) = DP%dRdEsd0(+1,:)                                 &
        +  DP%fiso(Kiso) / (2 * WIMP%m)                                 &
           * Halo%rho * DP%eta(:,Kiso)                                  &
           * DP%Wsd(+1,:,Kiso) * WIMP%GpSD0**2                          &
           * TO_CPD_KG_KEV
    ! SD cross term
    DP%dRdEsd0( 0,:) = DP%dRdEsd0( 0,:)                                 &
        +  DP%fiso(Kiso) / (2 * WIMP%m)                                 &
           * Halo%rho * DP%eta(:,Kiso)                                  &
           * DP%Wsd( 0,:,Kiso) * WIMP%GpSD0*WIMP%GnSD0                  &
           * TO_CPD_KG_KEV
    ! SD neutron term
    DP%dRdEsd0(-1,:) = DP%dRdEsd0(-1,:)                                 &
        +  DP%fiso(Kiso) / (2 * WIMP%m)                                 &
           * Halo%rho * DP%eta(:,Kiso)                                  &
           * DP%Wsd(-1,:,Kiso) * WIMP%GnSD0**2                          &
           * TO_CPD_KG_KEV
  END DO
  
  ! Integrate (efficiency-weighted) to find total rates.
  ! Uses a simple trapezoidal integration.
  DP%Rsi0 = 0d0
  DP%Rsd0 = 0d0
  ! Cycle over E bins and efficiency curves.
  DO KE = 1,DP%NE-1
    DO Keff = 0,DP%Neff
      DP%Rsi0(:,Keff) = DP%Rsi0(:,Keff)                                 &
          + 0.5d0 * (DP%E(KE+1) - DP%E(KE))                             &
            * (DP%eff(KE,Keff)*DP%dRdEsi0(:,KE)                         &
               + DP%eff(KE+1,Keff)*DP%dRdEsi0(:,KE+1))
      DP%Rsd0(:,Keff) = DP%Rsd0(:,Keff)                                 &
          + 0.5d0 * (DP%E(KE+1) - DP%E(KE))                             &
            * (DP%eff(KE,Keff)*DP%dRdEsd0(:,KE)                         &
               + DP%eff(KE+1,Keff)*DP%dRdEsd0(:,KE+1))
    END DO
  END DO
  
  ! Rates for actual couplings.  Here do separate SI and SD rates
  ! as well as total.
  DP%dRdEsi(:) =  alphasi(+1) * DP%dRdEsi0(+1,:)                        &
                + alphasi( 0) * DP%dRdEsi0( 0,:)                        &
                + alphasi(-1) * DP%dRdEsi0(-1,:)
  DP%dRdEsd(:) =  alphasd(+1) * DP%dRdEsd0(+1,:)                        &
                + alphasd( 0) * DP%dRdEsd0( 0,:)                        &
                + alphasd(-1) * DP%dRdEsd0(-1,:)
  DP%dRdE(:)   = DP%dRdEsi(:) + DP%dRdEsd(:)
  DP%Rsi(:) =  alphasi(+1) * DP%Rsi0(+1,:)                              &
             + alphasi( 0) * DP%Rsi0( 0,:)                              &
             + alphasi(-1) * DP%Rsi0(-1,:)
  DP%Rsd(:) =  alphasd(+1) * DP%Rsd0(+1,:)                              &
             + alphasd( 0) * DP%Rsd0( 0,:)                              &
             + alphasd(-1) * DP%Rsd0(-1,:)
  DP%R(:)   = DP%Rsi(:) + DP%Rsd(:)
  
  ! Average expected event (components at reference couplings)
  DP%MuSignalSI0(:,:) = DP%exposure * DP%Rsi0(:,:)
  DP%MuSignalSD0(:,:) = DP%exposure * DP%Rsd0(:,:)
  
  ! Average expected events.
  ! Could also achieve this by e.g. exposure * Rsi(:).
  DP%MuSignalSI(:) =  alphasi(+1) * DP%MuSignalSI0(+1,:)                &
                    + alphasi( 0) * DP%MuSignalSI0( 0,:)                &
                    + alphasi(-1) * DP%MuSignalSI0(-1,:)
  DP%MuSignalSD(:) =  alphasd(+1) * DP%MuSignalSD0(+1,:)                &
                    + alphasd( 0) * DP%MuSignalSD0( 0,:)                &
                    + alphasd(-1) * DP%MuSignalSD0(-1,:)
  DP%MuSignal(:)   = DP%MuSignalSI(:) + DP%MuSignalSD(:)
  
END SUBROUTINE



!=======================================================================
! LIKELIHOOD/P-VALUE ROUTINES
!=======================================================================

! ----------------------------------------------------------------------
! Calculates the log-likelihood for the current WIMP mass and couplings.
! Uses a Poisson distribution in the number of observed events N:
!    P(N|s+b)
! where s is the average expected signal and b is the average expected
! background.
! 
! Optional input argument:
!   D           The DetectorRateStruct structure containing detector
!               and rate date to calculate the likelihood for.  If
!               not given, a default structure (internally stored)
!               will be used.
! 
FUNCTION LogLikelihood(D) RESULT(lnlike)
  IMPLICIT NONE
  REAL*8 :: lnlike
  TYPE(DetectorRateStruct), INTENT(IN), TARGET, OPTIONAL :: D
  TYPE(DetectorRateStruct), POINTER :: DP
  INTEGER :: N
  REAL*8 :: b,s,mu
  
  ! Get pointer to detector structure to be used: either an explicitly
  ! provided structure or the default, internally-stored structure.
  IF (PRESENT(D)) THEN
    DP => D
  ELSE
    DP => DefaultDetector
  END IF
  
  ! Get observed events and expected events
  N = DP%Nevents
  b = DP%MuBackground
  s = DP%MuSignal(0)
  mu = s + b
  
  ! Poisson likelihood (routine handles special cases)
  lnlike = PoissonLogPDF(N,mu)
  
END FUNCTION


! ----------------------------------------------------------------------
! Calculates the log of the p-value for the current WIMP mass and
! couplings (NO BACKGROUND SUBTRACTION).  Uses the maximum gap method
! if efficiencies were provided in the efficiencies file for the
! intervals, otherwise uses a Poisson distribution in the number of
! observed events N:
!    P(N|s)
! where s is the average expected signal (background contributions are
! ignored).
! 
! Optional input argument:
!   D           The DetectorRateStruct structure containing detector
!               and rate date to calculate the p-value for.  If not
!               given, a default structure (internally stored) will
!               be used.
! 
FUNCTION LogPValue(D) RESULT(lnp)
  IMPLICIT NONE
  REAL*8 :: lnp
  TYPE(DetectorRateStruct), INTENT(IN), TARGET, OPTIONAL :: D
  TYPE(DetectorRateStruct), POINTER :: DP
  INTEGER :: N,I
  REAL*8 :: s,mu
  
  ! Get pointer to detector structure to be used: either an explicitly
  ! provided structure or the default, internally-stored structure.
  IF (PRESENT(D)) THEN
    DP => D
  ELSE
    DP => DefaultDetector
  END IF
  
  ! Get observed events and expected events
  N  = DP%Nevents
  mu = DP%MuSignal(0)
  
  ! Check if rates are available for each interval
  IF (DP%NEff .EQ. DP%Nevents+1) THEN
    lnp = LogMaximumGapP(mu,MAXVAL(DP%MuSignal(1:N+1)))
  ELSE
    lnp = LogPoissonP(N,mu)
  END IF
  
END FUNCTION


! ----------------------------------------------------------------------
! Calculates the factor x by which the cross-sections must be scaled
! (sigma -> x*sigma) to achieve the desired p-value (given as log(p)).
! See LogPValue() above for a description of the statistics.
! 
! Optional input arguments:
!   D           The DetectorRateStruct structure containing detector
!               and rate date to calculate the scaling for.  If not
!               given, a default structure (internally stored) will
!               be used.
!   lnp         The logarithm of the desired p-value.  Default is
!               p=0.1 (90% CL).
! 
FUNCTION ScaleToPValue(D,lnp) RESULT(x)
  IMPLICIT NONE
  REAL*8 :: x
  TYPE(DetectorRateStruct), INTENT(IN), TARGET, OPTIONAL :: D
  REAL*8, INTENT(IN), OPTIONAL :: lnp
  TYPE(DetectorRateStruct), POINTER :: DP
  INTEGER :: N
  REAL*8 :: lnp0,mu,f
  
  ! Get pointer to detector structure to be used: either an explicitly
  ! provided structure or the default, internally-stored structure.
  IF (PRESENT(D)) THEN
    DP => D
  ELSE
    DP => DefaultDetector
  END IF
  
  ! logarithm of p-value to use
  IF (PRESENT(lnp)) THEN
    lnp0 = lnp
  ELSE
    lnp0 = LOG(0.1d0)
  END IF
  
  ! Get observed events and expected events
  N  = DP%Nevents
  mu = DP%MuSignal(0)
  
  IF (mu .LE. 0d0) THEN
    x = HUGE(1d0)
    RETURN
  END IF
  
  IF (lnp0 .GE. 0d0) THEN
    x = 0d0
    RETURN
  END IF
  
  ! Use maximum gap if intervals available, otherwise Poisson
  IF (DP%NEff .EQ. DP%Nevents+1) THEN
    f = MAXVAL(DP%MuSignal(1:N+1)) / mu
    x = MaximumGapScaleToPValue(lnp0,mu,f)
  ELSE
    x = PoissonScaleToPValue(lnp0,N,mu)
  END IF
  
END FUNCTION


! ----------------------------------------------------------------------
! Calculates the factor x by which the signal must be scaled (s -> x*s)
! to achieve the desired p-value (given as log(p)) using the maximum
! gap method.
! 
! NOTE: Uses quickly implemented numerical methods.  Could be much
!       better optimized if the need arises.
! 
! Input arguments:
!   lnp         The logarithm of the desired p-value.
!   mu          The total expected number of events.
!   f           The largest fraction of events expected in any interval
!               (i.e. the "maximum" gap).
! 
PURE FUNCTION MaximumGapScaleToPValue(lnp,mu,f) RESULT(x)
  IMPLICIT NONE
  REAL*8 :: x
  REAL*8, INTENT(IN) :: lnp,mu,f
  REAL*8 :: x1,lnp1,x2,lnp2,xm,lnpm
  
  IF (mu .LE. 0d0) THEN
    x = HUGE(1d0)
    RETURN
  END IF
  
  ! Should not happen...
  IF (lnp .GE. 0d0) THEN
    x = 0d0
    RETURN
  END IF
  
  ! Should not happen...
  IF (f .LE. 0d0) THEN
    x = HUGE(1d0)
    RETURN
  END IF
  
  ! Special case
  IF (f .GE. 1d0) THEN
    x = -lnp / mu
    RETURN
  END IF
  
  ! Starting point
  x1   = 1d0 / mu
  lnp1 = LogMaximumGapP(x1*mu,x1*f*mu)
  ! Bracket
  IF (lnp1 .GT. lnp) THEN
    x2   = 2d0*x1
    lnp2 = LogMaximumGapP(x2*mu,x2*f*mu)
    DO WHILE (lnp2 .GE. lnp)
      x1   = x2
      lnp1 = lnp2
      x2   = 2d0*x2
      lnp2 = LogMaximumGapP(x2*mu,x2*f*mu)
    END DO
  ELSE
    DO WHILE (lnp1 .LE. lnp)
      x2   = x1
      lnp2 = lnp1
      x1   = 0.5d0*x1
      lnp1 = LogMaximumGapP(x1*mu,x1*f*mu)
    END DO
  END IF
  
  ! Bisection (geometric)
  DO WHILE ((ABS(lnp2-lnp1) .GT. 1d-5) .AND. (ABS(LOG(x2/x1)) .GT. 1d-5))
    xm   = SQRT(x1*x2)
    lnpm = LogMaximumGapP(xm*mu,xm*f*mu)
    IF (lnpm .GE. lnp) THEN
      x1   = xm
      lnp1 = lnpm
    ELSE
      x2   = xm
      lnp2 = lnpm
    END IF
  END DO
  x = 0.5d0*(x1+x2)
  
END FUNCTION


! ----------------------------------------------------------------------
! Calculates the factor x by which the signal must be scaled (s -> x*s)
! to achieve the desired p-value (given as log(p)) using a Poisson
! distribution.
! 
! NOTE: Uses quickly implemented numerical methods.  Could be much
!       better optimized if the need arises.
! 
! Input arguments:
!   lnp         The logarithm of the desired p-value.
!   N           Number of observed events.
!   mu          The total expected number of events.
! 
PURE FUNCTION PoissonScaleToPValue(lnp,N,mu) RESULT(x)
  IMPLICIT NONE
  REAL*8 :: x
  INTEGER, INTENT(IN) :: N
  REAL*8, INTENT(IN) :: lnp,mu
  REAL*8 :: x1,lnp1,x2,lnp2,xm,lnpm
  
  IF (mu .LE. 0d0) THEN
    x = HUGE(1d0)
    RETURN
  END IF
  
  ! Should not happen...
  IF (lnp .GE. 0d0) THEN
    x = 0d0
    RETURN
  END IF
  
  ! Analytic formula for N=0
  IF (N .EQ. 0) THEN
    x = -lnp / mu
    RETURN
  END IF
  
  ! Starting point
  x1   = N / mu
  lnp1 = LogPoissonP(N,x1*mu)
  
  ! Bracket
  IF (lnp1 .GT. lnp) THEN
    x2   = 2d0*x1
    lnp2 = LogPoissonP(N,x2*mu)
    DO WHILE (lnp2 .GE. lnp)
      x1   = x2
      lnp1 = lnp2
      x2   = 2d0*x2
      lnp2 = LogPoissonP(N,x2*mu)
    END DO
  ELSE
    DO WHILE (lnp1 .LE. lnp)
      x2   = x1
      lnp2 = lnp1
      x1   = 0.5d0*x1
      lnp1 = LogPoissonP(N,x1*mu)
    END DO
  END IF
  
  ! Bisection (geometric)
  DO WHILE ((ABS(lnp2-lnp1) .GT. 1d-5) .AND. (ABS(x2-x1) .GT. 1d-5))
    xm   = SQRT(x1*x2)
    lnpm = LogPoissonP(N,xm*mu)
    IF (lnpm .GE. lnp) THEN
      x1   = xm
      lnp1 = lnpm
    ELSE
      x2   = xm
      lnp2 = lnpm
    END IF
  END DO
  x = 0.5d0*(x1+x2)
  
END FUNCTION


! ----------------------------------------------------------------------
! Calculates the confidence interval [s1,s2] for the signal
! contribution s in a Poisson distribution with background expectation
! b and observed events N.  Calculated at the CL corresponding to the
! given p-value (p = 1-CL).  Uses the Feldman-Cousins method as
! described in:
!    G. Feldman & R. Cousins, Phys. Rev. D 57, 3873 (1998) [physics/9711021]
! 
! Input arguments:
!   lnp         The logarithm of the p-value (CL = 1-p)
!   N           Number of observed events
!   b           Mean number of expected background events
! Output arguments:
!   s1,s2       The signal contribution confidence interval [s1,s2]
! 
PURE SUBROUTINE FeldmanCousinsPoissonCI(lnp,N,b,s1,s2)
  IMPLICIT NONE
  INTEGER, INTENT(IN) :: N
  REAL*8, INTENT(IN) :: lnp,b
  REAL*8, INTENT(OUT) :: s1,s2
  REAL*8 :: step,sa,sb,x,lnsum1,lnsum2
  ! Specifies the desired precision for boundary searches.
  ! This should not be smaller than EPSILON(1d0)!
  REAL*8, PARAMETER :: RELATIVE_PRECISION = 100*EPSILON(1d0)
  
  ! It is possible to get a measure zero confidence interval,
  ! though that is only possible if the CL is low (61% or lower,
  ! safely below the 1-sigma CL = 68.3%).  The measure zero
  ! interval will occur if the following is true:
  !   \Sum_{k=N+1}^{floor(b)} P(k|b) > 1-p = CL
  ! in which case the interval is [0,0].  The l.h.s. is bounded from
  ! above by Q(floor(b),b) - P(0|b), the sum for N=0 (Q is the
  ! regularized incomplete gamma function) which has a global
  ! maximum of 0.611 at b=4.0.  Other maxima occur at integer values
  ! of b, with maxima only slowly changing (goes to 0.5 as b becomes
  ! large).  Catch this case here:
  IF (lnp .GT. LOG(0.38d0)) THEN
    CALL LogPoissonSums(N,b,lnsum1,x)
    CALL LogPoissonSums(INT(b+EPSILON(1d0)),b,lnsum2,x)
    IF (EXP(lnsum2)-EXP(lnsum1) .GE. (1-EXP(lnp))) THEN
      s1 = 0d0
      s2 = 0d0
      RETURN
    END IF
  END IF
  
  ! Get lower boundary
  IF (accept(N,b,0d0,lnp)) THEN
    ! This should include b > N
    s1 = 0d0
  ELSE
    ! This _should_ be an accepted point
    s1 = N - b
    ! Take s1 -> s1/2 until s1 no longer accepted
    DO WHILE (accept(N,b,s1,lnp))
      s1 = 0.5d0*s1
    END DO
    step = s1
    ! Boundary is now between [s1,s1+step]
    DO WHILE (step .GE. RELATIVE_PRECISION*s1)
      step = 0.5d0*step
      IF (.NOT. accept(N,b,s1+step,lnp)) s1 = s1 + step
    END DO
  END IF
  
  ! Get upper boundary
  ! Need at least one good starting point
  IF (N .GT. b) THEN
    ! This _should_ be an accepted point
    s2 = N - b
  ELSE
    s2 = 1d0
    DO WHILE (.NOT. accept(N,b,s2,lnp))
      s2 = 0.5d0*s2
    END DO
  END IF
  ! Take s2 -> 2*s2 until s2 no longer accepted
  DO WHILE (accept(N,b,s2,lnp))
    s2 = 2*s2
  END DO
  step = s2
  ! Boundary is now between [s2-step,s2]
  DO WHILE (step .GE. RELATIVE_PRECISION*s2)
    step = 0.5d0*step
    IF (.NOT. accept(N,b,s2-step,lnp)) s2 = s2 - step
  END DO
  
  
  CONTAINS
  
  ! ------------------------------------------
  ! Determines if the given N is within the acceptance
  ! region for a Poisson of mean b+s with given CL=1-p.
  PURE FUNCTION accept(N,b,s,lnp)
    IMPLICIT NONE
    LOGICAL :: accept
    INTEGER, INTENT(IN) :: N
    REAL*8, INTENT(IN) :: b,s,lnp
    INTEGER :: Ktail,K1,K2
    REAL*8 :: x,lnP1,lnP2,lnR1,lnR2,lnPsum
    
    accept = .TRUE.
    
    ! Below, we are going to determine the probability outside
    ! some interval [K1,K2] (Psum) by starting with an interval
    ! of the form [0,K2] and working our way inward until the
    ! probability exceeds the p-value or N falls outside the
    ! interval.  Note this means we are adding terms in order of
    !  _increasing_ R (the Feldman-Cousins ordering parameter).
    
    ! To avoid ambiguity, terms of equal R are either accepted
    ! or rejected from the interval as one.  To avoid under-
    ! coverage, we accept such a group if necessary to ensure the
    ! interval covers at least 1-p, leading to (conservative)
    ! overcoverage.  There is a case (s=0) where the R has a
    ! plateau over a range of K that must be handled carefully.
    ! The only other possibility of equal R is by coincidence for
    ! two terms on opposite sides of the Poisson peak; this case
    ! is handled appropriately.
    
    ! Special case: s = 0 with N <= b
    ! For the s = 0 case, R=1 for all K <= b with R < 1 for
    ! K > b.  By our construction, that means that [0,floor(b)]
    ! is _always_ part of the accepted interval.  We catch this
    ! case here because the later algorithms will not handle
    ! it correctly (the N > b case is handled correctly below).
    IF ((s .EQ. 0d0) .AND. (N .LE. b)) THEN
      accept = .TRUE.
      RETURN
    END IF
    
    ! Special case: N = b+s
    ! R is maximized at K=b+s, so this is the first accepted term.
    ! Though the code below handles this correctly, we avoid that
    ! possibly long calculation for what is a common case.
    IF (ABS(b+s-N) .LT. 0.4d0) THEN
      accept = .TRUE.
      RETURN
    END IF
    
    ! For K >= Ktail, ordering parameter R is smaller than
    ! for any K < Ktail and is asymptotically decreasing.
    Ktail = FClnRsearch0(b,s) + 1
    
    ! Special case: N is in the tail region.
    ! Just have to check if probability for K >= N exceeds p-value.
    IF (N .GE. Ktail) THEN
      CALL LogPoissonSums(N,b+s,x,lnPsum)
      accept = (lnPsum .GT. lnp)
      RETURN
    END IF
    
    ! Get area of tail region
    CALL LogPoissonSums(Ktail,b+s,x,lnPsum)
    
    ! Special case: acceptance region contains at least [0,Ntail-1].
    ! By our above check, N is in [0,Ntail-1]
    IF (lnPsum .GT. lnp) THEN
      accept = .TRUE.
      RETURN
    END IF
    
    ! Now we narrow the interval from both sides using the
    ! ordering parameter.  We narrow until N falls outside
    ! the interval or the interval no longer covers enough
    ! probability (i.e. area outside the interval exceeds
    ! p-value).  We treats terms of equal R as one in terms
    ! of acceptance: this is handled appropriately below
    ! except for the case where R(K) is flat, which only
    ! occurs for the s=0 case already handled above.
    !
    K1   = 0
    lnR1 = FClnR(K1,b,s)
    K2   = Ktail-1
    lnR2 = FClnR(K2,b,s)
    ! Psum is probability outside of [K1,K2]
    DO WHILE ((lnPsum .LT. lnp))
      ! See if N is no longer in the valid region
      IF ((N .LT. K1) .OR. (N .GT. K2)) THEN
        accept = .FALSE.
        RETURN
      END IF
      ! Find term with _smaller_ R value: add its probability
      ! to Psum and drop from interval.  Note special case where
      ! both terms have same R value (drop both).
      IF (lnR1 .LT. lnR2) THEN
        lnPsum = LOG_SUM(lnPsum,LogPoisson(K1,b+s))
        K1     = K1+1
        lnR1   = FClnR(K1,b,s)
      ELSE IF (lnR1 .GT. lnR2) THEN
        lnPsum = LOG_SUM(lnPsum,LogPoisson(K2,b+s))
        K2     = K2-1
        lnR2   = FClnR(K2,b,s)
      ELSE
        lnPsum = LOG_SUM(lnPsum,LogPoisson(K1,b+s))
        K1     = K1+1
        lnR1   = FClnR(K1,b,s)
        lnPsum = LOG_SUM(lnPsum,LogPoisson(K2,b+s))
        K2     = K2-1
        lnR2   = FClnR(K2,b,s)
      END IF
    END DO
    
    ! If we made it here, we narrowed the interval too far before
    ! losing the K=N term.  N must be within the proper interval.
    accept = .TRUE.
    
  END FUNCTION
  
  ! ------------------------------------------
  ! Calculates the logarithm of the Poisson probability
  !   P(k|mu) = e^{-mu} mu^k / k!
  PURE FUNCTION LogPoisson(K,mu) RESULT(lnP)
    IMPLICIT NONE
    REAL*8 :: lnP
    INTEGER, INTENT(IN) :: K
    REAL*8, INTENT(IN) :: mu
    IF (K .EQ. 0) THEN
      lnP = -mu
    ELSE IF (mu .EQ. 0d0) THEN
      lnP = -HUGE(1d0)
    ELSE
      lnP = -mu + K*LOG(mu) - LOG_GAMMA(K+1d0)
    END IF
  END FUNCTION
  
  ! ------------------------------------------
  ! Calculates ln(r), where the ratio
  !   R = P(K|b+s)/P(K|b+s0(K))
  ! is the ordering term used by Feldman-Cousins, with
  !   s0(K) = max(0,K-b)
  ! the best-fit signal.
  PURE FUNCTION FClnR(K,b,s) RESULT(lnR)
    IMPLICIT NONE
    REAL*8 :: lnR
    INTEGER, INTENT(IN) :: K
    REAL*8, INTENT(IN) :: b,s
    IF (K .EQ. 0) THEN
      lnR = -s
    ELSE IF (b+s .LE. 0d0) THEN
      lnR = -HUGE(1d0)
    ELSE IF ( K .LE. b) THEN
      lnR = K*LOGp1(s/b) - s
    ELSE
      lnR = K*LOG((b+s)/K) + K - (b+s)
    END IF
  END FUNCTION
  
  ! ------------------------------------------
  ! Finds the index K such that the Feldman-Cousins
  ! ratio R is maximized for the given b & s.
  PURE FUNCTION FCpeakRloc(b,s) RESULT(K)
    IMPLICIT NONE
    INTEGER :: K
    REAL*8, INTENT(IN) :: b,s
    ! When analytically continuing R(k) for continuous k,
    ! there is a single maximum located at k=b+s.  For the
    ! discrete case, it must be at either the floor or
    ! ceiling of b+s.
    K = MAX(INT(b+s),0)
    IF (FClnR(K+1,b,s) .GT. FClnR(K,b,s)) K = K+1
  END FUNCTION
  
  ! ------------------------------------------
  ! Finds the largest index K such that the Feldman-Cousins
  ! ratio R is as large as R(K=0) for the given b & s.
  PURE FUNCTION FClnRsearch0(b,s) RESULT(K)
    IMPLICIT NONE
    INTEGER :: K
    REAL*8, INTENT(IN) :: b,s
    INTEGER :: K1,K2,Km,Kstep
    REAL*8 :: lnR0,lnR1,lnR2,lnRm
    Kstep = 1
    K1 = FCpeakRloc(b,s)
    K2 = K1+Kstep
    lnR0 = FClnR(0,b,s)
    lnR1 = FClnR(K1,b,s)
    lnR2 = FClnR(K2,b,s)
    ! Bracket
    DO WHILE (lnR2 .GE. lnR0)
      Kstep = 2*Kstep
      K1    = K2
      lnR1  = lnR2
      K2    = K2+Kstep
      lnR2 = FClnR(K2,b,s)
    END DO
    ! Bisection
    DO WHILE (K2-K1 .GT. 1)
      Km   = K1 + (K2-K1)/2
      lnRm = FClnR(Km,b,s)
      IF (lnRm .GE. lnR0) THEN
        K1   = Km
        lnR1 = lnRm
      ELSE
        K2   = Km
        lnR2 = lnRm
      END IF
    END DO
    K = K1
  END FUNCTION
  
END SUBROUTINE


! ----------------------------------------------------------------------
! For the Poisson distribution P(k|mu), calculates the log of the
! p-value, where p is defined as:
!   p = \Sum_{k\le N} P(k|mu)
! 
PURE FUNCTION LogPoissonP(N,mu) RESULT(lnp)
  IMPLICIT NONE
  REAL*8 :: lnp
  INTEGER, INTENT(IN) :: N
  REAL*8, INTENT(IN) :: mu
  REAL*8 :: lnlesum,lngesum
  
  ! Utility routine calculates P(k|mu) for k\le N and k\ge N.
  CALL LogPoissonSums(N,mu,lnlesum,lngesum)
  lnp = lnlesum
  
END FUNCTION


! ----------------------------------------------------------------------
! For the Poisson distribution P(k|mu), calculates the log of the sums:
!   lesum = \Sum_{k\le N} P(k|mu)
!   gesum = \Sum_{k\ge N} P(k|mu)
! This routine calculates the smaller of the two and uses the relation
!   lesum + gesum = 1 + P(N|mu)
! to find the other (note the k=N term appears in both sums).
! 
! Input arguments:
!   N           Maximum/minimum k in sum
!   mu          Mean of the Poisson
! Output arguments:
!   lnlesum     The logarithm of the sum of Poisson terms k = 0,1,...,N.
!   lngesum     The logarithm of the sum of Poisson terms k = N,N+1,....
! 
PURE SUBROUTINE LogPoissonSums(N,mu,lnlesum,lngesum)
  IMPLICIT NONE
  REAL*8 :: lnp
  INTEGER, INTENT(IN) :: N
  REAL*8, INTENT(IN) :: mu
  REAL*8, INTENT(OUT) :: lnlesum,lngesum
  INTEGER :: K,Kmax
  REAL*8 :: lnpmf0,lnpmf
  REAL*8, PARAMETER :: LN_PRECISION = -35d0  ! Precision of ~ 1d-15
  
  ! Special cases
  IF (mu .LE. 0d0) THEN
    lnlesum = 0d0
    IF (N .EQ. 0) THEN
      lngesum = 0d0
    ELSE
      lngesum = -HUGE(1d0)
    END IF
    RETURN
  ELSE IF (N .EQ. 0d0) THEN
    lnlesum = -mu
    lngesum = 0d0
    RETURN
  END IF
  
  ! General case
  lnpmf = N*LOG(mu) - mu - LOG_GAMMA(N+1d0)
  lnpmf0 = lnpmf
  
  ! NOTE: This algorithm calculates the log of the sum of both
  !       all K <= N (lnlesum) and all K >= N (lngesum).
  ! The distribution peaks around n = mu; to avoid a loss of
  ! precision, we will do the lower sum for n < mu and the
  ! upper sum for n > mu.
  IF (N .LE. mu) THEN
    K = N
    lnlesum = lnpmf
    DO WHILE (k .GT. 0)
      K = K-1
      lnpmf = lnpmf + LOG((K+1)/mu)
      lnlesum = LOG_SUM(lnlesum,lnpmf)
      IF (lnpmf - lnlesum .LE. LN_PRECISION) EXIT
    END DO
    lngesum = LOG(1d0 - (EXP(lnlesum) - EXP(lnpmf0)))
  ELSE
    K = N
    lngesum = lnpmf
    ! Determine a conservative upper limit on sum terms.
    ! Will hopefully hit precision condition first, but include
    ! this upper limit in case we didn't think of some pathological
    ! cases.
    IF ((N-mu)**2 .GT. mu) THEN
      Kmax = N + NINT(-LN_PRECISION * (N/(N-mu))) + 10
    ELSE
      Kmax = N + NINT(-LN_PRECISION * (1d0+SQRT(mu))) + 10
    END IF
    DO WHILE (K .LT. Kmax)
      K = K+1
      lnpmf = lnpmf + LOG(mu/K)
      lngesum = LOG_SUM(lngesum,lnpmf)
      IF (lnp - lngesum .LE. LN_PRECISION) EXIT
    END DO
    lnlesum = LOG(1d0 - (EXP(lngesum) - EXP(lnpmf0)))
  END IF
  
END SUBROUTINE


! ----------------------------------------------------------------------
! Calculates the log of the maximum gap statistic.  See:
!   S. Yellin, Phys. Rev. D 66, 032005 (2002) [arXiv:physics/0203002]
! This statistic is an upper bound on the p-value in the event the
! background spectrum is unknown.  The p-value is equal to 1-C_0 and
! is given by:
!   p = \sum_{k=1}^{floor(mu/x)} (k*x-mu)^(k-1) e^(-k*x) (mu-k(x-1)) / k!
! 
! NOTE: IMPLEMENTATION NEEDS TESTING.
! 
! Input arguments:
!   mu          Average expected number of signal events
!   x           Largest expected number of signal events in any
!               interval/gap.
! 
PURE FUNCTION LogMaximumGapP(mu,x) RESULT(lnp)
  IMPLICIT NONE
  REAL*8 :: lnp
  REAL*8, INTENT(IN) :: mu,x
  INTEGER :: K,Kmax
  REAL*8 :: p,psum,z
  
  ! Bad cases
  IF ((mu .LT. 0d0) .OR. (x .LT. 0d0)) THEN
    lnp = HUGE(1d0)
    RETURN
  END IF
  
  ! Special cases
  IF (mu .EQ. 0d0) THEN
    lnp = 0d0
    RETURN
  ELSE IF (x .GE. mu) THEN
    lnp = -mu
    RETURN
  ELSE IF (x .EQ. 0d0) THEN
    lnp = 0d0
    RETURN
  END IF
  
  ! Not optimized for log case or any extreme case
  ! WARNING: CONDITIONS AND ALGORITHMS NOT FULLY TESTED.
  
  IF (mu*EXP(-x) .GT. 12.5d0) THEN
    lnp = 0d0
    RETURN
  END IF
  
  ! Calculation involves sum from 1 to Kmax = floor(mu/x).  By
  ! definition, Kmax should never exceed the number of intervals.
  ! We handle the lowest Kmax cases explicitly and do the sum for
  ! larger Kmax.
  
  ! Small epsilon added to avoid bad truncation results
  ! when mu/x is an integer.
  Kmax = INT(mu/x+SQRT(EPSILON(1d0)))
  SELECT CASE (Kmax)
  CASE (:0)
    lnp = HUGE(1d0)
  CASE (1)
    ! p = (mu-x+1) e^-x
    ! Note LOGp1(z) = ln(1+z)
    lnp = -x + LOGp1(mu-x)
  CASE (2)
    ! p = (mu-x+1) e^-x [1 - 1/2 (mu-2x) e^(-x) (1 - (x-1)/(mu-(x-1)))]
    ! maximum z in range of validity is 0.1925 (safe here)
    z = 0.5d0 * (mu-2*x) * EXP(-x) * (mu-2*(x-1)) / (mu-x+1)
    lnp = -x + LOGp1(mu-x) + LOGp1(-z)
  CASE (3:)
    ! Do explicit sum.
    ! Note finite alternating series makes accuracy/precision
    ! and convergence determinations difficult.
    psum = 0d0
    DO K = 1,Kmax
      p = (K*x-mu)**(K-1) * EXP(-K*x) * (mu - K*(x-1)) / GAMMAI(K+1)
      psum = psum + p
    END DO
    lnp = LOG(p)
  END SELECT
  
END FUNCTION



!=======================================================================
! USAGE
!=======================================================================

! ----------------------------------------------------------------------
! Checks if the command line contains any requests to show the program
! usage.
! 
FUNCTION ShowUsageRequested() RESULT(flag)
  IMPLICIT NONE
  LOGICAL :: flag
  INTEGER :: I,Narg
  CHARACTER*32 :: arg
  
  flag = .FALSE.
  Narg = IARGC()
  
  DO I=1,Narg
    CALL GETARG(I,arg)
    IF (arg(1:1) .EQ. '?') THEN
      flag = .TRUE.
      RETURN
    ELSE IF ( (arg(1:2) .EQ. '-?') .OR. (arg(1:2) .EQ. '-h') ) THEN
      flag = .TRUE.
      RETURN
    ELSE IF (arg(1:6) .EQ. '--help') THEN
      flag = .TRUE.
      RETURN
    END IF
  END DO
  
END FUNCTION ShowUsageRequested


! ----------------------------------------------------------------------
! Shows usage.
! Prints out contents of given file.
! 
! Optional input argument:
!   file        File containing usage contents.  Default is the
!               calling program's name with '.use' appended.
!   
SUBROUTINE ShowUsage(file)
  IMPLICIT NONE
  CHARACTER*(*), INTENT(IN), OPTIONAL :: file
  CHARACTER*1024 :: file0
  INTEGER, PARAMETER :: USAGE_FID = 19
  CHARACTER*1024 :: line
  INTEGER :: ios,K
  
  IF (PRESENT(file)) THEN
    file0 = file
  ELSE
    CALL GETARG(0,file0)
    K = INDEX(file0,'/',.TRUE.)
    IF (K .NE. 0) file0 = file0(K+1:)
    file0 = TRIM(file0) // '.use'
  END IF
  
  OPEN(FILE=file0,UNIT=USAGE_FID,STATUS='OLD',                          &
       FORM='FORMATTED',ACTION='READ',IOSTAT=ios)
  IF (ios .NE. 0) THEN
    WRITE(0,*) 'Unable to display usage:'
    WRITE(0,*) 'Could not open usage file ' // TRIM(file0)
    WRITE(0,*)
    RETURN
  END IF
  
  DO WHILE (.TRUE.)
    READ(UNIT=USAGE_FID,FMT='(A)',IOSTAT=ios) line
    IF (ios .LT. 0) EXIT
    WRITE(*,'(A)') TRIM(line)
  END DO
  
  CLOSE(UNIT=USAGE_FID,IOSTAT=ios)
  
END SUBROUTINE ShowUsage



!=======================================================================
! COMMAND LINE PROCESSING
! Several types:
!   *) GetXXXArg(index,status)
!      Parses the command line argument at the given index and returns
!      as type XXX (logical,integer,real,complex).  A negative index
!      will count from the end of the argument list.  The optional
!      'status' parameter indicates if the argument exists and was
!      properly parsed.
!   *) GetShortArg(akey)
!      Indicates if the given akey appears in any argument of the form
!      -<akey> or -<akey1><akey2><akey3>....  In this case, akey
!      is meant to be a single character.
!   *) GetLongArg(akey)
!      Indicates if the given akey appears in any argument of the form
!      --<akey> or -<akey>=<aval>.
!   *) GetLongArgXXX(akey,aval)
!      Indicates if the given akey appears in any argument of the form
!      --<akey> or -<akey>=<aval>.  If present, <aval> will be parsed
!      as type XXX (string,logical,integer,real,complex) and placed in
!      aval.  The first instance of a particular key will always be
!      parsed; if additional arguments use the same key, they will be
!      ignored.  This routine returns .FALSE. if there is a failure to
!      parse <aval>.
!=======================================================================

! ----------------------------------------------------------------------
! Obtains the command line argument at the given index as a LOGICAL.
! Negative index counts from the end of the argument list.
! The optional status argument indicates if the given argument exists
! and was properly parsed.
! 
FUNCTION GetLogicalArg(index,status) RESULT(aval)
  IMPLICIT NONE
  LOGICAL :: aval
  INTEGER, INTENT(IN) :: index
  LOGICAL, INTENT(OUT), OPTIONAL :: status
  CHARACTER*256 :: arg
  INTEGER :: index0,Narg,ios
  
  aval = .FALSE.
  IF (PRESENT(status)) status = .FALSE.
  
  ! Check index validity, count from end if necessary
  Narg   = IARGC()
  IF (index .LT. 0) THEN
    index0 = Narg + 1 + index
  ELSE
    index0 = index
  END IF
  IF ((index0 .LT. 1) .OR. (index0 .GT. Narg)) RETURN
  
  ! Parse argument
  CALL GETARG(index0,arg)
  READ(UNIT=arg,FMT=*,IOSTAT=ios) aval
  IF (ios .EQ. 0) THEN
    IF (PRESENT(status)) status = .TRUE.
  END IF
  
END FUNCTION GetLogicalArg


! ----------------------------------------------------------------------
! Obtains the command line argument at the given index as an INTEGER.
! Negative index counts from the end of the argument list.
! The optional status argument indicates if the given argument exists
! and was properly parsed.
! Accepts real valued strings, which will be rounded to the nearest
! integer.
! 
FUNCTION GetIntegerArg(index,status) RESULT(aval)
  IMPLICIT NONE
  INTEGER :: aval
  INTEGER, INTENT(IN) :: index
  LOGICAL, INTENT(OUT), OPTIONAL :: status
  CHARACTER*256 :: arg
  INTEGER :: index0,Narg,ios
  REAL*8 :: x
  
  aval = -HUGE(aval)
  IF (PRESENT(status)) status = .FALSE.
  
  ! Check index validity, count from end if necessary
  Narg   = IARGC()
  IF (index .LT. 0) THEN
    index0 = Narg + 1 + index
  ELSE
    index0 = index
  END IF
  IF ((index0 .LT. 1) .OR. (index0 .GT. Narg)) RETURN
  
  ! Parse argument
  ! Try reading as integer first, then as real and convert
  ! to integer
  CALL GETARG(index0,arg)
  READ(UNIT=arg,FMT=*,IOSTAT=ios) aval
  IF (ios .EQ. 0) THEN
    IF (PRESENT(status)) status = .TRUE.
  ELSE
    READ(UNIT=arg,FMT=*,IOSTAT=ios) x
    IF (ios .EQ. 0) THEN
      IF (PRESENT(status)) status = .TRUE.
      aval = NINT(x)
    END IF
  END IF
  
END FUNCTION GetIntegerArg


! ----------------------------------------------------------------------
! Obtains the command line argument at the given index as a REAL*8.
! Negative index counts from the end of the argument list.
! The optional status argument indicates if the given argument exists
! and was properly parsed.
! 
FUNCTION GetRealArg(index,status) RESULT(aval)
  IMPLICIT NONE
  REAL*8 :: aval
  INTEGER, INTENT(IN) :: index
  LOGICAL, INTENT(OUT), OPTIONAL :: status
  CHARACTER*256 :: arg
  INTEGER :: index0,Narg,ios
  
  aval = -HUGE(aval)
  IF (PRESENT(status)) status = .FALSE.
  
  ! Check index validity, count from end if necessary
  Narg   = IARGC()
  IF (index .LT. 0) THEN
    index0 = Narg + 1 + index
  ELSE
    index0 = index
  END IF
  IF ((index0 .LT. 1) .OR. (index0 .GT. Narg)) RETURN
  
  ! Parse argument
  CALL GETARG(index0,arg)
  READ(UNIT=arg,FMT=*,IOSTAT=ios) aval
  IF (ios .EQ. 0) THEN
    IF (PRESENT(status)) status = .TRUE.
  END IF
  
END FUNCTION GetRealArg


! ----------------------------------------------------------------------
! Obtains the command line argument at the given index as a COMPLEX*16.
! Negative index counts from the end of the argument list.
! The optional status argument indicates if the given argument exists
! and was properly parsed.
! 
FUNCTION GetComplexArg(index,status) RESULT(aval)
  IMPLICIT NONE
  COMPLEX*16 :: aval
  INTEGER, INTENT(IN) :: index
  LOGICAL, INTENT(OUT), OPTIONAL :: status
  CHARACTER*256 :: arg
  INTEGER :: index0,Narg,ios
  
  aval = -HUGE(1d0)*(1,1)
  IF (PRESENT(status)) status = .FALSE.
  
  ! Check index validity, count from end if necessary
  Narg   = IARGC()
  IF (index .LT. 0) THEN
    index0 = Narg + 1 + index
  ELSE
    index0 = index
  END IF
  IF ((index0 .LT. 1) .OR. (index0 .GT. Narg)) RETURN
  
  ! Parse argument
  CALL GETARG(index0,arg)
  READ(UNIT=arg,FMT=*,IOSTAT=ios) aval
  IF (ios .EQ. 0) THEN
    IF (PRESENT(status)) status = .TRUE.
  END IF
  
END FUNCTION GetComplexArg


! ----------------------------------------------------------------------
! Checks if the given argument key is given as a command line argument
! of the form -akey or -<akey1><akey2><akey3>...
! Returns .TRUE. if valid key was found, .FALSE. otherwise.
! 
FUNCTION GetShortArg(akey) RESULT(status)
  IMPLICIT NONE
  LOGICAL :: status
  CHARACTER*(*), INTENT(IN) :: akey
  CHARACTER*256 :: arg
  INTEGER :: I,Narg,pos
  
  Narg   = IARGC()
  status = .FALSE.
  
  DO I=1,Narg
    CALL GETARG(I,arg)
    IF (LEN_TRIM(arg) .LT. 2)  CYCLE
    IF (arg(1:1) .NE. '-')  CYCLE
    IF (arg(2:2) .EQ. '-')  CYCLE
    pos = INDEX(arg,akey)
    IF (pos .GE. 2) THEN
      status = .TRUE.
      RETURN
    END IF
  END DO
  
END FUNCTION GetShortArg


! ----------------------------------------------------------------------
! Checks if the given argument key is given as a command line argument
! of the form --akey or --akey=<val>.
! Returns .TRUE. if valid key was found, .FALSE. otherwise.
! 
FUNCTION GetLongArg(akey) RESULT(status)
  IMPLICIT NONE
  LOGICAL :: status
  CHARACTER*(*), INTENT(IN) :: akey
  CHARACTER*256 :: arg
  INTEGER :: I,Narg,len
  
  Narg   = IARGC()
  len    = LEN_TRIM(akey)
  status = .FALSE.
  
  DO I=1,Narg
    CALL GETARG(I,arg)
    IF (arg .EQ. '--' // akey) THEN
      status = .TRUE.
      RETURN
    ELSE IF (arg(:3+len) .EQ. '--' // akey // '=') THEN
      status = .TRUE.
      RETURN
    END IF
  END DO
  
END FUNCTION GetLongArg


! ----------------------------------------------------------------------
! Checks if the given argument key is given as a command line argument
! of the form --akey=<aval> and inserts the appropriate string into
! aval.
! Returns .TRUE. if valid key was found, .FALSE. otherwise.
! 
FUNCTION GetLongArgString(akey,aval) RESULT(status)
  IMPLICIT NONE
  LOGICAL :: status
  CHARACTER*(*), INTENT(IN) :: akey
  CHARACTER*(*), INTENT(OUT) :: aval
  CHARACTER*256 :: arg
  INTEGER :: I,Narg,len,pos
  
  Narg   = IARGC()
  len    = LEN_TRIM(akey)
  status = .FALSE.
  
  DO I=1,Narg
    CALL GETARG(I,arg)
    IF (arg(:2+len) .EQ. '--' // akey) THEN
      pos = INDEX(arg,'=')
      IF (pos .EQ. 3+len) THEN
        status = .TRUE.
        aval = TRIM(arg(pos+1:))
        RETURN
      END IF
    END IF
  END DO
  
END FUNCTION GetLongArgString


! ----------------------------------------------------------------------
! Checks if the given argument key is given as a command line argument
! of the form --akey=<aval1>,<aval2>,... and inserts the appropriate
! CHARACTER*N values into aval (ALLOCATABLE array of length Nval).
! The aval array MUST have deferred character length, i.e. declare as:
!   CHARACTER(LEN=:), DIMENSION(:), ALLOCATABLE :: aval
! Returns .TRUE. if valid key was found, .FALSE. otherwise.
! 
! NOTE: This function must be defined within a module or the use of
!       interfaces will be required.
! 
FUNCTION GetLongArgStrings(akey,N,aval,Nval) RESULT(status)
  IMPLICIT NONE
  LOGICAL :: status
  CHARACTER*(*), INTENT(IN) :: akey
  INTEGER, INTENT(IN) :: N
  CHARACTER(LEN=:), DIMENSION(:), ALLOCATABLE, INTENT(OUT) :: aval
  INTEGER, INTENT(OUT) :: Nval
  CHARACTER*256 :: sval
  INTEGER :: I1,I2,Ia,ios
  
  IF (.NOT. GetLongArgString(akey,sval)) THEN
    status = .FALSE.
    RETURN
  END IF

  status = .TRUE.
  
  ! Find number of values
  I1   = 1
  Nval = 1
  DO WHILE (INDEX(sval(I1:),',') .GT. 0)
    I1 = I1 + INDEX(sval(I1:),',')
    Nval = Nval + 1
  END DO
  IF (ALLOCATED(aval)) DEALLOCATE(aval)
  ALLOCATE(CHARACTER(LEN=N) :: aval(1:Nval))
  aval = ''
  
  ! Parse values
  READ(UNIT=sval(I1:),FMT='(A)',IOSTAT=ios) aval(Nval)
  IF (ios .NE. 0) THEN
    status = .FALSE.
    Nval   = 0
    IF (ALLOCATED(aval)) DEALLOCATE(aval)
    ALLOCATE(CHARACTER(LEN=N) :: aval(0))
    RETURN
  END IF
  I2 = 1
  DO Ia = 1, Nval-1
    I1 = I2
    I2 = I1 + INDEX(sval(I1:),',')
    READ(UNIT=sval(I1:I2-2),FMT='(A)',IOSTAT=ios) aval(Ia)
    IF (ios .NE. 0) THEN
      status = .FALSE.
      Nval   = 0
      IF (ALLOCATED(aval)) DEALLOCATE(aval)
      ALLOCATE(CHARACTER(LEN=N) :: aval(0))
      RETURN
    END IF
  END DO
  
END FUNCTION GetLongArgStrings


! ----------------------------------------------------------------------
! Checks if the given argument key is given as a command line argument
! of the form --akey=<aval> and inserts the appropriate LOGICAL value
! into aval.
! Returns .TRUE. if valid key was found, .FALSE. otherwise.
! 
FUNCTION GetLongArgLogical(akey,aval) RESULT(status)
  IMPLICIT NONE
  LOGICAL :: status
  CHARACTER*(*), INTENT(IN) :: akey
  LOGICAL, INTENT(OUT) :: aval
  CHARACTER*256 :: sval
  INTEGER :: ios
  IF (.NOT. GetLongArgString(akey,sval)) THEN
    status = .FALSE.
    RETURN
  END IF
  READ(UNIT=sval,FMT=*,IOSTAT=ios) aval
  status = (ios .EQ. 0)
END FUNCTION GetLongArgLogical


! ----------------------------------------------------------------------
! Checks if the given argument key is given as a command line argument
! of the form --akey=<aval1>,<aval2>,... and inserts the appropriate
! LOGICAL values into aval (ALLOCATABLE array of length Nval).
! Returns .TRUE. if valid key was found, .FALSE. otherwise.
! 
! NOTE: This function must be defined within a module or the use of
!       interfaces will be required.
! 
FUNCTION GetLongArgLogicals(akey,aval,Nval) RESULT(status)
  IMPLICIT NONE
  LOGICAL :: status
  CHARACTER*(*), INTENT(IN) :: akey
  LOGICAL, ALLOCATABLE, INTENT(OUT) :: aval(:)
  INTEGER, INTENT(OUT) :: Nval
  INTEGER, PARAMETER :: N = 64
  CHARACTER(LEN=:), ALLOCATABLE :: sval(:)
  INTEGER :: I,ios
  
  IF (.NOT. GetLongArgStrings(akey,N,sval,Nval)) THEN
    status = .FALSE.
    Nval   = 0
    IF (ALLOCATED(aval)) DEALLOCATE(aval)
    ALLOCATE(aval(0))
    RETURN
  END IF
  
  status = .TRUE.
  IF (ALLOCATED(aval)) DEALLOCATE(aval)
  ALLOCATE(aval(1:Nval))
  
  DO I=1,Nval
    READ(UNIT=sval(I),FMT=*,IOSTAT=ios) aval(I)
    IF (ios .NE. 0) THEN
      status = .FALSE.
      Nval   = 0
      IF (ALLOCATED(aval)) DEALLOCATE(aval)
      ALLOCATE(aval(0))
      RETURN
    END IF
  END DO
  
END FUNCTION GetLongArgLogicals


! ----------------------------------------------------------------------
! Checks if the given argument key is given as a command line argument
! of the form --akey=<aval> and inserts the appropriate INTEGER value
! into aval.
! Accepts real valued strings, which will be rounded to the nearest
! integer.
! Returns .TRUE. if valid key was found, .FALSE. otherwise.
! 
FUNCTION GetLongArgInteger(akey,aval) RESULT(status)
  IMPLICIT NONE
  LOGICAL :: status
  CHARACTER*(*), INTENT(IN) :: akey
  INTEGER, INTENT(OUT) :: aval
  CHARACTER*256 :: sval
  INTEGER :: ios
  REAL*8 :: x
  IF (.NOT. GetLongArgString(akey,sval)) THEN
    status = .FALSE.
    RETURN
  END IF
  ! Try reading as integer first, then as real and convert to integer
  READ(UNIT=sval,FMT=*,IOSTAT=ios) aval
  IF (ios .NE. 0) THEN
    READ(UNIT=sval,FMT=*,IOSTAT=ios) x
    IF (ios .EQ. 0) aval = NINT(x)
  END IF
  status = (ios .EQ. 0)
END FUNCTION GetLongArgInteger


! ----------------------------------------------------------------------
! Checks if the given argument key is given as a command line argument
! of the form --akey=<aval1>,<aval2>,... and inserts the appropriate
! INTEGER values into aval (ALLOCATABLE array of length Nval).
! Returns .TRUE. if valid key was found, .FALSE. otherwise.
! 
! NOTE: This function must be defined within a module or the use of
!       interfaces will be required.
! 
FUNCTION GetLongArgIntegers(akey,aval,Nval) RESULT(status)
  IMPLICIT NONE
  LOGICAL :: status
  CHARACTER*(*), INTENT(IN) :: akey
  INTEGER, ALLOCATABLE, INTENT(OUT) :: aval(:)
  INTEGER, INTENT(OUT) :: Nval
  INTEGER, PARAMETER :: N = 64
  CHARACTER(LEN=:), ALLOCATABLE :: sval(:)
  INTEGER :: I,ios
  
  IF (.NOT. GetLongArgStrings(akey,N,sval,Nval)) THEN
    status = .FALSE.
    Nval   = 0
    IF (ALLOCATED(aval)) DEALLOCATE(aval)
    ALLOCATE(aval(0))
    RETURN
  END IF
  
  status = .TRUE.
  IF (ALLOCATED(aval)) DEALLOCATE(aval)
  ALLOCATE(aval(1:Nval))
  
  DO I=1,Nval
    READ(UNIT=sval(I),FMT=*,IOSTAT=ios) aval(I)
    IF (ios .NE. 0) THEN
      status = .FALSE.
      Nval   = 0
      IF (ALLOCATED(aval)) DEALLOCATE(aval)
      ALLOCATE(aval(0))
      RETURN
    END IF
  END DO
  
END FUNCTION GetLongArgIntegers


! ----------------------------------------------------------------------
! Checks if the given argument key is given as a command line argument
! of the form --akey=<aval> and inserts the appropriate REAL*8 value
! into aval.
! Returns .TRUE. if valid key was found, .FALSE. otherwise.
! 
FUNCTION GetLongArgReal(akey,aval) RESULT(status)
  IMPLICIT NONE
  LOGICAL :: status
  CHARACTER*(*), INTENT(IN) :: akey
  REAL*8, INTENT(OUT) :: aval
  CHARACTER*256 :: sval
  INTEGER :: ios
  IF (.NOT. GetLongArgString(akey,sval)) THEN
    status = .FALSE.
    RETURN
  END IF
  READ(UNIT=sval,FMT=*,IOSTAT=ios) aval
  status = (ios .EQ. 0)
END FUNCTION GetLongArgReal


! ----------------------------------------------------------------------
! Checks if the given argument key is given as a command line argument
! of the form --akey=<aval1>,<aval2>,... and inserts the appropriate
! REAL*8 values into aval (ALLOCATABLE array of length Nval).
! Returns .TRUE. if valid key was found, .FALSE. otherwise.
! 
! NOTE: This function must be defined within a module or the use of
!       interfaces will be required.
! 
FUNCTION GetLongArgReals(akey,aval,Nval) RESULT(status)
  IMPLICIT NONE
  LOGICAL :: status
  CHARACTER*(*), INTENT(IN) :: akey
  REAL*8, ALLOCATABLE, INTENT(OUT) :: aval(:)
  INTEGER, INTENT(OUT) :: Nval
  INTEGER, PARAMETER :: N = 64
  CHARACTER(LEN=:), ALLOCATABLE :: sval(:)
  INTEGER :: I,ios
  
  IF (.NOT. GetLongArgStrings(akey,N,sval,Nval)) THEN
    status = .FALSE.
    Nval   = 0
    IF (ALLOCATED(aval)) DEALLOCATE(aval)
    ALLOCATE(aval(0))
    RETURN
  END IF
  
  status = .TRUE.
  IF (ALLOCATED(aval)) DEALLOCATE(aval)
  ALLOCATE(aval(1:Nval))
  
  DO I=1,Nval
    READ(UNIT=sval(I),FMT=*,IOSTAT=ios) aval(I)
    IF (ios .NE. 0) THEN
      status = .FALSE.
      Nval   = 0
      IF (ALLOCATED(aval)) DEALLOCATE(aval)
      ALLOCATE(aval(0))
      RETURN
    END IF
  END DO
  
END FUNCTION GetLongArgReals


! ----------------------------------------------------------------------
! Checks if the given argument key is given as a command line argument
! of the form --akey=<aval> and inserts the appropriate COMPLEX*16 value
! into aval.
! Returns .TRUE. if valid key was found, .FALSE. otherwise.
! 
FUNCTION GetLongArgComplex(akey,aval) RESULT(status)
  IMPLICIT NONE
  LOGICAL :: status
  CHARACTER*(*), INTENT(IN) :: akey
  COMPLEX*16, INTENT(OUT) :: aval
  CHARACTER*256 :: sval
  INTEGER :: ios
  IF (.NOT. GetLongArgString(akey,sval)) THEN
    status = .FALSE.
    RETURN
  END IF
  READ(UNIT=sval,FMT=*,IOSTAT=ios) aval
  status = (ios .EQ. 0)
END FUNCTION GetLongArgComplex


! ----------------------------------------------------------------------
! Returns the full command line used to run the program.
! This routine is nearly identical to the Intel Fortran routine
! GET_COMMAND, but is provided here because some older compilers
! do not provide that routine.  White space between arguments will
! not be conserved: arguments will be separated by a single space,
! regardless of actual white space on the command line.
! 
SUBROUTINE GetFullCommand(cmd)
  IMPLICIT NONE
  CHARACTER*(*), INTENT(OUT) :: cmd
  INTEGER :: pos,I,Narg,alen
  CHARACTER*256 :: arg
  
  cmd = ''
  pos = 1
  
  ! Command
  !CALL GETARG(0,arg,alen)
  CALL GETARG(0,arg)
  alen = LEN_TRIM(arg)
  cmd = arg
  pos = pos + alen
  
  ! Arguments
  Narg = IARGC()
  DO I=1, Narg
    !CALL GETARG(I,arg,alen)
    CALL GETARG(I,arg)
    alen = LEN_TRIM(arg)
    cmd(pos:pos) = ' '
    pos = pos + 1
    cmd(pos:pos+alen-1) = arg(1:alen)
    pos = pos + alen
  END DO
  
  ! Following necessary to terminate/clear string
  cmd(pos:) = ''
  
END SUBROUTINE GetFullCommand



!=======================================================================
! EXPORT/IMPORT DATA
!=======================================================================

! ----------------------------------------------------------------------
! Returns an available (unused) file I/O unit number or -1 if no
! available unit number could be found.
! NOTE: A free unit number is a unit number not currently connected
!       at the time this function is called.  Numbers are not reserved,
!       so do not assume this number will remain free if it is not used
!       to open a file immediately.
! 
FUNCTION FreeIOUnit() RESULT(Nio)
  IMPLICIT NONE
  INTEGER :: Nio
  INTEGER, PARAMETER :: START_UNIT = 50
  INTEGER, PARAMETER :: MAX_UNIT   = 99
  LOGICAL opened
  
  ! Cycle through unit numbers until we find one that is not connected
  Nio = START_UNIT
  INQUIRE(UNIT=Nio,OPENED=opened)
  DO WHILE (opened .AND. (Nio .LT. MAX_UNIT))
    Nio = Nio + 1
    INQUIRE(UNIT=Nio,OPENED=opened)
  END DO
  
  ! Did not find unopen unit number in allowed range
  IF (opened) THEN
    WRITE(UNIT=5,FMT=*) 'ERROR: an error occurred while trying '        &
        // 'to find an available unit number.'
    Nio = -1
    RETURN
  END IF
  
END FUNCTION


! ----------------------------------------------------------------------
! Determines if the given line is a comment (non-data) line.
! A comment line is any blank line or line beginning with the comment
! character (default: '#').
!   line            the line to check
!   commentchar     (Optional) comment character.  Default is '#'.
! 
LOGICAL FUNCTION IsCommentLine(line,commentchar)
  IMPLICIT NONE
  CHARACTER*(*), INTENT(IN) :: line
  CHARACTER, INTENT(IN), OPTIONAL :: commentchar
  
  IsCommentLine = .FALSE.
  IF (LEN_TRIM(line) .EQ. 0) THEN
    IsCommentLine = .TRUE.
  ELSE IF (PRESENT(commentchar)) THEN
    IF (line(1:1) .EQ. commentchar) IsCommentLine = .TRUE.
  ELSE
    IF (line(1:1) .EQ. '#') IsCommentLine = .TRUE.
  END IF
  
END FUNCTION


! ----------------------------------------------------------------------
! Determines the number of lines in the given file.
!   fid             The identifier for the file to be read
!   status          (Optional) .TRUE. if file was successfully read
! 
FUNCTION FileLines(fid,status) RESULT(lines)
  IMPLICIT NONE
  INTEGER :: lines
  INTEGER, INTENT(IN) :: fid
  LOGICAL, INTENT(OUT), OPTIONAL :: status
  INTEGER :: ios
  
  lines = 0
  IF (PRESENT(status)) status = .FALSE.
  
  REWIND(UNIT=fid,IOSTAT=ios)
  IF (ios .LT. 0) RETURN
  
  DO WHILE (ios .GE. 0)
    lines = lines + 1
    READ(UNIT=fid,FMT=*,IOSTAT=ios)
  END DO
  
  ! Count included failed final read, so subtract 1
  lines = lines - 1
  
  REWIND(UNIT=fid,IOSTAT=ios)
  IF (PRESENT(status)) status = .TRUE.
  
END FUNCTION


! ----------------------------------------------------------------------
! Determines the number of data (non-comment) lines in the given file.
! Blank lines and lines beginning with the comment character (default:
! '#') are considered comment lines.
!   fid             The identifier for the file to be read
!   commentchar     (Optional) comment character.  Default is '#'.
!   status          (Optional) .TRUE. if file was successfully read
! 
FUNCTION FileDataLines(fid,commentchar,status) RESULT(lines)
  IMPLICIT NONE
  INTEGER :: lines
  INTEGER, INTENT(IN) :: fid
  CHARACTER, INTENT(IN), OPTIONAL :: commentchar
  LOGICAL, INTENT(OUT), OPTIONAL :: status
  INTEGER :: ios
  CHARACTER :: commentchar0
  CHARACTER*1024 :: line
  
  IF (PRESENT(commentchar)) THEN
    commentchar0 = commentchar
  ELSE
    commentchar0 = '#'
  END IF
  
  lines = 0
  IF (PRESENT(status)) status = .FALSE.
  
  REWIND(UNIT=fid,IOSTAT=ios)
  IF (ios .LT. 0) RETURN
  
  DO WHILE (ios .GE. 0)
    READ(UNIT=fid,FMT='(A)',IOSTAT=ios) line
    IF (ios .GE. 0) THEN
      IF (.NOT. IsCommentLine(line)) lines = lines + 1
    END IF
  END DO
  
  REWIND(UNIT=fid,IOSTAT=ios)
  IF (PRESENT(status)) status = .TRUE.
  
END FUNCTION


! ----------------------------------------------------------------------
! Determines number of fields in the given string.
! 
PURE FUNCTION NumberOfFields(in) RESULT(N)
  IMPLICIT NONE
  INTEGER :: N
  CHARACTER*(*), INTENT(IN) :: in
  INTEGER :: I,len,pos
  LOGICAL :: is_blank
  
  len = LEN_TRIM(in)
  
  pos      = 0
  N        = 0
  is_blank = .TRUE.
  
  DO I = 1, len
    IF (is_blank .AND. (in(I:I) .NE. ' ')) THEN
      N = N + 1
      is_blank = .FALSE.
    ELSE
      is_blank = (in(I:I) .EQ. ' ')
    END IF
  END DO
  
END FUNCTION


! ----------------------------------------------------------------------
! Determines the number of data columns in the given file.
! This is based upon the number of fields separated by spaces on a
! data line.
! NOTE: Currently limited to first 1024 characters in a line.
!       This can be increased below.
!   fid             The identifier for the file to be read
!   commentchar     (Optional) comment character.  Default is '#'.
!   status          (Optional) .TRUE. if file was successfully read
! 
FUNCTION FileDataColumns(fid,commentchar,status) RESULT(columns)
  IMPLICIT NONE
  INTEGER :: columns
  INTEGER, INTENT(IN) :: fid
  CHARACTER, INTENT(IN), OPTIONAL :: commentchar
  LOGICAL, INTENT(OUT), OPTIONAL :: status
  INTEGER :: ios
  CHARACTER :: commentchar0
  CHARACTER*1024 :: dataline    ! Increase for wide files
  
  IF (PRESENT(commentchar)) THEN
    commentchar0 = commentchar
  ELSE
    commentchar0 = '#'
  END IF
  
  columns = 0
  IF (PRESENT(status)) status = .FALSE.
  
  REWIND(UNIT=fid,IOSTAT=ios)
  IF (ios .LT. 0) RETURN
  
  ! Find a data line
  ios = 0
  dataline = ''
  DO WHILE ((ios .GE. 0) .AND. IsCommentLine(dataline,commentchar0))
    READ(UNIT=fid,FMT='(A)',IOSTAT=ios) dataline
  END DO
  ! No data lines?
  IF (ios .LT. 0) RETURN
  
  columns = NumberOfFields(dataline)
  
  REWIND(UNIT=fid,IOSTAT=ios)
  IF (PRESENT(status)) status = .TRUE.
  
END FUNCTION


! ----------------------------------------------------------------------
! Loads data from a formatted (text) file.
! Take an allocatable 2D array as an argument, which will be allocated
! to the correct size.  Any line beginning with a letter or symbol is
! treated as a comment line and ignored.
! Optional input arguments (at least one required):
!     file       The name of the file to be read
!     fid        The identifier for the file to be read.
!                File must already be open if given.
! Other optional input arguments:
!     commentchar  Comment character.  Default is '#'.
! Output arguments:
!     Nrow,Ncol  Number of data rows and columns; this indicates the
!                size of the allocated array
!     data       Allocatable REAL*8 array that will be allocated to
!                size data(Nrow,Ncol) and filled with data from the
!                file
! Optional output arguments:
!     success    Set to .TRUE. if the data was successfully loaded;
!                otherwise .FALSE.
! 
SUBROUTINE LoadTable(file,fid,commentchar,Nrow,Ncol,data,status)
  IMPLICIT NONE
  CHARACTER*(*), INTENT(IN), OPTIONAL :: file
  INTEGER, INTENT(IN), OPTIONAL :: fid
  CHARACTER, INTENT(IN), OPTIONAL :: commentchar
  INTEGER, INTENT(OUT), OPTIONAL :: Nrow,Ncol
  REAL*8, ALLOCATABLE, INTENT(OUT), OPTIONAL :: data(:,:)
  LOGICAL, INTENT(OUT), OPTIONAL :: status
  INTEGER :: fid0,Nrow0,Ncol0,ios,I
  LOGICAL :: status0
  CHARACTER :: commentchar0
  CHARACTER*256 :: buf
  
  ! Argument checking
  IF (.NOT. (PRESENT(file) .OR. PRESENT(fid))) THEN
    WRITE(0,*) 'ERROR: LoadTable requires a file or fid argument.'
    STOP
  END IF
  
  IF (PRESENT(commentchar)) THEN
    commentchar0 = commentchar
  ELSE
    commentchar0 = '#'
  END IF
  
  IF (PRESENT(status)) status = .FALSE.
  
  ! Open file, if necessary
  IF (PRESENT(fid)) THEN
    fid0 = fid
  ELSE
    fid0 = FreeIOUnit()
    OPEN(UNIT=fid0,FILE=file,STATUS='OLD',FORM='FORMATTED',             &
         ACTION='READ',IOSTAT=ios)
    IF (ios .NE. 0) RETURN
  END IF
  
  ! Rewind file, in case it has already been read from
  REWIND(UNIT=fid0,IOSTAT=ios)
  IF (ios .LT. 0) RETURN
  
  ! Get number of data lines
  Nrow0 = FileDataLines(fid0,commentchar0,status0)
  IF (.NOT. status0) RETURN
  IF (PRESENT(Nrow)) Nrow = Nrow0
  
  ! Get number of data columns
  Ncol0 = FileDataColumns(fid0,commentchar0,status0)
  IF (.NOT. status0) RETURN
  IF (PRESENT(Ncol)) Ncol = Ncol0
  
  ! No data array argument: nothing left to do
  IF (.NOT. PRESENT(data)) THEN
    IF (.NOT. PRESENT(fid)) THEN
      CLOSE(UNIT=fid0,IOSTAT=ios)
    END IF
    IF (PRESENT(status)) status = .TRUE.
    RETURN
  END IF
  
  ! Load data into array
  IF (ALLOCATED(data)) DEALLOCATE(data)
  ALLOCATE(data(Nrow0,Ncol0))
  I = 1
  DO WHILE (I .LE. Nrow0)
    READ(UNIT=fid0,FMT='(A)',IOSTAT=ios) buf
    ! Check if end of file or read error (ios < 0 for eof and
    ! ios > 0 for an I/O error)
    IF (ios .NE. 0) RETURN
    ! Check if comment line; if so, go back and read another line
    IF (IsCommentLine(buf)) CYCLE
    ! Parse line
    BACKSPACE(UNIT=fid0)
    READ(UNIT=fid0,FMT=*) data(I,:)
    I = I+1
  END DO
  
  ! Cleanup
  IF (.NOT. PRESENT(fid)) THEN
    CLOSE(UNIT=fid0,IOSTAT=ios)
  END IF
  
  IF (PRESENT(status)) status = .TRUE.
  
END SUBROUTINE


! ----------------------------------------------------------------------
! Loads data columns from a formatted (text) file.
! Currently allows up to 5 selectable columns to be read.
! Takes allocatable arrays as arguments, which will be allocated to
! the correct size.  Any line beginning with a letter or symbol is
! treated as a comment line and ignored.
! Optional input arguments (at least one required):
!     file       The name of the file to be read
!     fid        The identifier for the file to be read.
!                File must already be open if given.
! Other optional input arguments:
!     commentchar  Comment character.  Default is '#'.
! Output arguments:
!     N          Length of data columns; this indicates the length
!                of the allocated arrays
! Optional input arguments:
!     N1,N2,N3,N4,N5
!                File columns to read into corresponding arrays.
!                If not specified, columns 1-5 will be read.
!                A negative number counts from the end (-1 is last
!                column).
! Optional output arguments:
!     C1,C2,C3,C4,C5
!                Allocatable REAL*8 arrays that will be allocated and
!                filled with data from the columns in the file
!                corresponding to N1 - N5.
!     success    Set to .TRUE. if the data was successfully loaded;
!                otherwise .FALSE.
! 
SUBROUTINE LoadArrays(file,fid,commentchar,N,N1,C1,N2,C2,N3,C3,N4,C4,   &
                      N5,C5,status)
  IMPLICIT NONE
  CHARACTER*(*), INTENT(IN), OPTIONAL :: file
  INTEGER, INTENT(IN), OPTIONAL :: fid
  CHARACTER, INTENT(IN), OPTIONAL :: commentchar
  INTEGER, INTENT(OUT), OPTIONAL :: N
  INTEGER, INTENT(IN), OPTIONAL :: N1,N2,N3,N4,N5
  REAL*8, ALLOCATABLE, INTENT(OUT), OPTIONAL :: C1(:),C2(:),C3(:),      &
                                                C4(:),C5(:)
  LOGICAL, INTENT(OUT), OPTIONAL :: status
  INTEGER :: fid0,N0,Ncol0,ios,I,J,J1,J2,J3,J4,J5
  LOGICAL :: status0
  REAL*8, ALLOCATABLE :: data(:,:)
  CHARACTER :: commentchar0
  CHARACTER*256 :: buf
  
  ! Argument checking
  IF (.NOT. (PRESENT(file) .OR. PRESENT(fid))) THEN
    WRITE(0,*) 'ERROR: LoadArrays requires a file or fid argument.'
    STOP
  END IF

  IF (PRESENT(commentchar)) THEN
    commentchar0 = commentchar
  ELSE
    commentchar0 = '#'
  END IF
  
  IF (PRESENT(status)) status = .FALSE.
  
  ! Open file, if necessary
  IF (PRESENT(fid)) THEN
    fid0 = fid
  ELSE
    fid0 = FreeIOUnit()
    OPEN(UNIT=fid0,FILE=file,STATUS='OLD',FORM='FORMATTED',             &
         ACTION='READ',IOSTAT=ios)
    IF (ios .NE. 0) RETURN
  END IF
  
  ! Rewind file, in case it has already been read from
  REWIND(UNIT=fid0,IOSTAT=ios)
  IF (ios .LT. 0) RETURN
  
  ! Get number of data lines
  N0 = FileDataLines(fid0,commentchar0,status0)
  IF (.NOT. status0) RETURN
  IF (PRESENT(N)) N = N0
  
  ! Get number of data columns
  Ncol0 = FileDataColumns(fid0,commentchar0,status0)
  IF (.NOT. status0) RETURN
  
  ! Find how many columns should be read
  ! Following sets indices to 0, default, or appropriate
  ! optional argument
  IF (UpdateColNum(PRESENT(C1),PRESENT(N1),1,J1)) J1 = N1
  IF (PRESENT(C1)) CALL UpdateColNum2(Ncol0,J1,status0)
  IF (UpdateColNum(PRESENT(C2),PRESENT(N2),2,J2)) J2 = N2
  IF (PRESENT(C2)) CALL UpdateColNum2(Ncol0,J2,status0)
  IF (UpdateColNum(PRESENT(C3),PRESENT(N3),3,J3)) J3 = N3
  IF (PRESENT(C3)) CALL UpdateColNum2(Ncol0,J3,status0)
  IF (UpdateColNum(PRESENT(C4),PRESENT(N4),4,J4)) J4 = N4
  IF (PRESENT(C4)) CALL UpdateColNum2(Ncol0,J4,status0)
  IF (UpdateColNum(PRESENT(C5),PRESENT(N5),5,J5)) J5 = N5
  IF (PRESENT(C5)) CALL UpdateColNum2(Ncol0,J5,status0)
  IF (.NOT. status0) RETURN
  
  ! Only read in to last necessary column
  Ncol0 = MAX(J1,J2,J3,J4,J5)
  
  ! Load data into temporary array
  ALLOCATE(data(N0,Ncol0))
  I = 1
  DO WHILE (I .LE. N0)
    READ(UNIT=fid0,FMT='(A)',IOSTAT=ios) buf
    ! Check if end of file or read error (ios < 0 for eof and
    ! ios > 0 for an I/O error)
    IF (ios .NE. 0) RETURN
    ! Check if comment line; if so, go back and read another line
    IF (IsCommentLine(buf)) CYCLE
    ! Parse line
    BACKSPACE(UNIT=fid0)
    READ(UNIT=fid0,FMT=*) data(I,:)
    I = I+1
  END DO
  
  ! Allocate and fill arrays
  IF (PRESENT(C1)) CALL UpdateArray(J1,C1)
  IF (PRESENT(C2)) CALL UpdateArray(J2,C2)
  IF (PRESENT(C3)) CALL UpdateArray(J3,C3)
  IF (PRESENT(C4)) CALL UpdateArray(J4,C4)
  IF (PRESENT(C5)) CALL UpdateArray(J5,C5)
  
  ! Cleanup
  DEALLOCATE(data)
  IF (.NOT. PRESENT(fid)) THEN
    CLOSE(UNIT=fid0,IOSTAT=ios)
  END IF
  
  IF (PRESENT(status)) status = .TRUE.
  
  
  CONTAINS
    
  ! ------------------------------------------
  ! Utility to determine column number
  ! Will set column number to 0 if no column will be read or
  ! the default defCN if a column will be read.  If givenCN
  ! is .TRUE. (indicating if the optional NJ argument was given)
  ! and the column is to be read, this function will return
  ! .TRUE. indicating CN should be read from NJ instead.
  FUNCTION UpdateColNum(usecol,givenCN,defCN,CN)
    IMPLICIT NONE
    LOGICAL :: UpdateColNum
    LOGICAL, INTENT(IN) :: usecol,givenCN
    INTEGER, INTENT(IN) :: defCN
    INTEGER, INTENT(OUT) :: CN
    UpdateColNum = .FALSE.
    IF (usecol) THEN
      CN = defCN
      IF (givenCN) UpdateColNum = .TRUE.
    ELSE
      CN = 0
    END IF
  END FUNCTION UpdateColNum
  
  ! ------------------------------------------
  ! Utility to update column number
  ! Negative indices (indicating column number from end) are
  ! converted to true indices.  Validity of the column number
  ! is checked.
  SUBROUTINE UpdateColNum2(Ncol,CN,status0)
    IMPLICIT NONE
    INTEGER, INTENT(IN) :: Ncol
    INTEGER, INTENT(INOUT) :: CN
    LOGICAL, INTENT(INOUT) :: status0
    IF (Ncol .LT. 0) THEN
      CN = Ncol + 1 - CN
    END IF
    status0 = status0 .AND. (CN .GT. 0) .AND. (CN .LE. Ncol)
  END SUBROUTINE UpdateColNum2
  
  ! ------------------------------------------
  ! Allocates and fills an array arr with data from column J
  SUBROUTINE UpdateArray(J,arr)
    IMPLICIT NONE
    INTEGER, INTENT(IN) :: J
    REAL*8, ALLOCATABLE, INTENT(OUT) :: arr(:)
    IF (ALLOCATED(arr)) DEALLOCATE(arr)
    ALLOCATE(arr(1:N0))
    arr = data(:,J)
  END SUBROUTINE UpdateArray
  
END SUBROUTINE



!=======================================================================
! TABULATION ROUTINES
!=======================================================================

! ----------------------------------------------------------------------
! Extracts parameters from arguments of the form:
!   --<akey>=<min>,<max>,<N>,<use_log>
! Does not change the existing value if the parameter is not given
! or cannot be parsed.  The integer argument N can be given as a
! floating-point number and the logical argument use_log can be
! as 'T'/'F' or numerically (zero is false, all other values true).
! 
SUBROUTINE GetTabulationArgs(akey,xmin,xmax,N,use_log)
  IMPLICIT NONE
  CHARACTER*(*), INTENT(IN) :: akey
  REAL*8, INTENT(INOUT) :: xmin,xmax
  INTEGER, INTENT(INOUT) :: N
  LOGICAL, INTENT(INOUT) :: use_log
  LOGICAL :: status,Ltmp
  INTEGER :: Nval,ios,Itmp
  REAL*8 :: Rtmp
  INTEGER, PARAMETER :: NCHAR = 32
  CHARACTER(LEN=:), DIMENSION(:), ALLOCATABLE :: aval
  
  IF (.NOT. GetLongArgStrings(akey,NCHAR,aval,Nval)) RETURN
  
  IF (Nval .GE. 1) THEN
    READ(UNIT=aval(1),FMT=*,IOSTAT=ios) Rtmp
    IF (ios .EQ. 0) xmin = Rtmp
  END IF
  
  IF (Nval .GE. 2) THEN
    READ(UNIT=aval(2),FMT=*,IOSTAT=ios) Rtmp
    IF (ios .EQ. 0) xmax = Rtmp
  END IF
  
  IF (Nval .GE. 3) THEN
    READ(UNIT=aval(3),FMT=*,IOSTAT=ios) Rtmp  ! read as real
    IF (ios .EQ. 0) N = Rtmp
  END IF
  
  IF (Nval .GE. 4) THEN
    READ(UNIT=aval(4),FMT=*,IOSTAT=ios) Ltmp
    IF (ios .EQ. 0) THEN
      use_log = Ltmp
    ELSE
      READ(UNIT=aval(4),FMT=*,IOSTAT=ios) Rtmp
      IF (ios .EQ. 0) use_log = .NOT. (Rtmp .EQ. 0)
    END IF
  END IF
  
END SUBROUTINE


! ----------------------------------------------------------------------
! Initializes the given TabulationStruct to use a tabulation specified
! by the given parameters.  Some sanity checks are performed.
! 
! Output argument:
!   TS              TabulationStruct to be filled with tabulation
!                   parameterization data.
! Input arguments:
!   xmin,xmax       Tabulation range.  Might be adjusted by up to 1/2
!                   bin size to ensure even sized bins in logarithmic
!                   case with N < 0.
!   N               Number of intervals between tabulation points.
!                   A negative number indicates intervals per decade.
!   use_log         If logarithmic rather than linear spacing is to
!                   be used.
! 
SUBROUTINE InitTabulation(TS,xmin,xmax,N,use_log)
  IMPLICIT NONE
  TYPE(TabulationStruct), INTENT(OUT) :: TS
  REAL*8, INTENT(IN) :: xmin,xmax
  INTEGER, INTENT(IN) :: N
  LOGICAL, INTENT(IN) :: use_log
  
  TS%logarithmic = use_log
  
  IF (use_log) THEN
    IF (xmin .LE. 0d0) THEN
      IF (xmax .LE. 0d0) THEN
        TS%xmin =   1d0
        TS%xmax = 100d0
      ELSE
        TS%xmin = xmax/100
        TS%xmax = xmax
      END IF
    ELSE
      IF (xmax .LE. 0d0) THEN
        TS%xmin = xmin
        TS%xmax = 100*xmin
      ELSE
        TS%xmin = xmin
        TS%xmax = xmax
      END IF
    END IF
    IF (N .EQ. 0) THEN
      TS%N = -10
    ELSE
      TS%N = N
    END IF
    TS%lnxmin = LOG(TS%xmin)
    TS%lnxmax = LOG(TS%xmax)
  ELSE
    TS%xmin = MIN(xmin,xmax)
    TS%xmax = MAX(xmin,xmax)
    IF (N .EQ. 0) THEN
      TS%N = 100
    ELSE
      TS%N = ABS(N)
    END IF
    TS%lnxmin = 0d0
    TS%lnxmax = 0d0
  END IF
  
  ! Special case (single point)
  IF (TS%xmax .EQ. TS%xmin) THEN
    TS%N     = 0
    TS%delta = 0d0
    RETURN
  END IF
  
  ! Logarithmic case
  IF (use_log) THEN
    IF (TS%N .LT. 0) THEN
      TS%delta = LOG(10d0) / ABS(TS%N)
      TS%N     = NINT(ABS(TS%N) * LOG10(TS%xmax/TS%xmin))
      TS%xmax  = TS%xmin*EXP(TS%N*TS%delta)
      TS%lnxmax = LOG(TS%xmax)
    ELSE
      TS%delta = (TS%lnxmax - TS%lnxmin) / TS%N
    END IF
  ! Linear case
  ELSE
    TS%delta = (TS%xmax - TS%xmin) / TS%N
  END IF
  
END SUBROUTINE


! ----------------------------------------------------------------------
! Returns the Kth tabulation value, with K=0 (TS%N) corresponding to
! the minimum (maximum) of the tabulation range.
! 
PURE FUNCTION TabulationValue(TS,K) RESULT (x)
  IMPLICIT NONE
  REAL*8 :: x
  TYPE(TabulationStruct), INTENT(IN) :: TS
  INTEGER, INTENT(IN) :: K
  
  IF (TS%logarithmic) THEN
    x = EXP(TS%lnxmin + K*TS%delta)
  ELSE
    x = TS%xmin + K*TS%delta
  END IF
  
END FUNCTION


! ----------------------------------------------------------------------
! Returns the tabulation index K such that x_K <= x < x_{K+1}, or -1 if
! x does not fall within the tabulation range.  Note tabulation points
! are x_K with K=0,...,TS%N.
! 
PURE FUNCTION TabulationInterval(TS,x) RESULT (K)
  IMPLICIT NONE
  INTEGER :: K
  TYPE(TabulationStruct), INTENT(IN) :: TS
  REAL*8, INTENT(IN) :: x
  
  IF (TS%N .EQ. 0) THEN
    IF (x .EQ. TS%xmin) THEN
      K = 0
    ELSE
      K = -1
    END IF
    RETURN
  END IF
  
  IF (x .LT. TS%xmin) THEN
    K = -1
  ELSE IF (TS%logarithmic) THEN
    K = INT((LOG(x)-TS%lnxmin)/TS%delta)
  ELSE
    K = INT((x-TS%xmin)/TS%delta)
  END IF
  IF ((K .LT. 0) .OR. (K .GT. TS%N-1)) K = -1
  
END FUNCTION


! ----------------------------------------------------------------------
! Returns the tabulation index K such that x falls within a bin centered
! on x_K and with a width equal to the tabulation spacing.  A value of
! -1 is returned if x falls outside the tabulation bins.  For
! logarithmic tabulation, the bin center and size are taken in
! logarithmic space.  Note tabulation points are x_K with K=0,...,TS%N.
! 
PURE FUNCTION TabulationBin(TS,x) RESULT (K)
  IMPLICIT NONE
  INTEGER :: K
  TYPE(TabulationStruct), INTENT(IN) :: TS
  REAL*8, INTENT(IN) :: x
  
  IF (TS%N .EQ. 0) THEN
    IF (x .EQ. TS%xmin) THEN
      K = 0
    ELSE
      K = -1
    END IF
    RETURN
  END IF
  
  IF (TS%logarithmic) THEN
    K = NINT((LOG(x)-TS%lnxmin)/TS%delta)
  ELSE
    K = NINT((x-TS%xmin)/TS%delta)
  END IF
  IF ((K .LT. 0) .OR. (K .GT. TS%N)) K = -1
  
END FUNCTION



!=======================================================================
! RANDOM NUMBER GENERATOR
! The generator here is based on the algorithm of Marsaglia & Zaman
! (and later Tsang).  Some of the routines below are based upon an
! implementation by Christian Walck.  Due to the use of an OpenMP
! THREADPRIVATE directive when declaring the internal state structure,
! the various random number routines below should not have any
! performance issues when using multi-threading through OpenMP (as
! opposed to the RANDOM_NUMBER intrinsic, which may lead to _slower_
! code in multi-threaded cases due to its serial-only implementation
! leading to possible thread pile-up).
! 
! See:
!   G. Marsaglia and A. Zaman, "Toward a Universal Random Number
!     Generator," Florida State University Report: FSU-SCRI-87-50
!     (1987).
!   G. Marsaglia, A. Zaman and W.W. Tsang, "Toward a universal
!     random number generator," Statistics & Probability Letters,
!     9, 35, (1990).
!   F. James, "A Review of Pseudorandom Number Generators,"
!     Comput. Phys. Commun. 60, 329 (1990).
! 
!=======================================================================

! ----------------------------------------------------------------------
! Initializes state data for the random number generator of Marsaglia,
! Zaman & Tsang, optionally using the given seed.
! 
! Optional input argument:
!     seed       INTEGER seed used to generate state data.
!                If not given, a random seed will be used.
! Optional output argument:
!     state      A RandState structure containing RNG state
!                data that will be initialized.  If not given,
!                the internal RNG state will be initialized.
! 
SUBROUTINE rand_init(seed,state)
  IMPLICIT NONE
  INTEGER, INTENT(IN), OPTIONAL :: seed
  TYPE(RandState), INTENT(OUT), OPTIONAL :: state
  INTEGER :: seed0,I,J,IF1,IF2,IF3,IC1,M
  TYPE(RandState) :: state0
  REAL*8 :: x,S,T
  INTEGER, PARAMETER :: MAX_SEED = 921350144
  
  IF (PRESENT(seed)) THEN
    seed0 = MODULO(seed,MAX_SEED)
  ELSE
    CALL RANDOM_NUMBER(x)
    seed0 = INT(x*MAX_SEED)
  END IF
  
  state0%initialized = .TRUE.
  state0%I = 97
  state0%J = 33
  
  ! Three Fibonacci generator seeds (2-177) and one congruential
  ! generator seed (0-168)
  IF1 = MOD(seed0/169/176/176,176) + 2
  IF2 = MOD(seed0/169/176,176) + 2
  IF3 = MOD(seed0/169,176) + 2
  IC1 = MOD(seed0,169)
  
  DO I = 1,97
     S = 0.0d0
     T = 0.5d0
     DO J = 1,24
        M = MOD(MOD(IF1*IF2,179)*IF3,179)
        IF1 = IF2
        IF2 = IF3
        IF3 = M
        IC1 = MOD(53*IC1+1,169)
        IF ( MOD(IC1*M,64) .GE. 32 ) S = S + T
        T = 0.5d0 * T
     END DO
     state0%U(I) = S
  END DO
  
  state0%C  =   362436.0d0 / 16777216.0d0
  state0%CD =  7654321.0d0 / 16777216.0d0
  state0%CM = 16777213.0d0 / 16777216.0d0
  
  IF (PRESENT(state)) THEN
    state = state0
  ELSE
    DEFAULT_RandState = state0
  END IF
  
END SUBROUTINE


! ----------------------------------------------------------------------
! Returns a unit random number using the algorithm of Marsaglia, Zaman
! & Tsang, optionally using/updating the given state data.
! 
! Optional input/output argument:
!     state      A RandState structure containing RNG state
!                data that will be used for generating the random
!                number; the state will be updated.  If not given,
!                an internal RNG state will be used.
! 
FUNCTION rand(state) RESULT(x)
  IMPLICIT NONE
  REAL*8 :: x
  TYPE(RandState), INTENT(INOUT), OPTIONAL :: state
  IF (PRESENT(state)) THEN
    IF (.NOT. state%initialized) CALL rand_init(state=state)
    CALL rand_number(state,x)
  ELSE
    IF (.NOT. DEFAULT_RandState%initialized) CALL rand_init(state=DEFAULT_RandState)
    CALL rand_number(DEFAULT_RandState,x)
  END IF
END FUNCTION


! ----------------------------------------------------------------------
! Generates a unit random number using the algorithm of Marsaglia, Zaman
! & Tsang.  Requires a state structure to be given, which _must_ be
! initialized.  The rand() function above is the intended to be the
! main routine for external use.
! 
! Required input/output argument:
!     state      A RandState structure containing RNG state
!                data that will be used for generating the random
!                number; the state will be updated.
! Required output argument:
!     x          The uniform random number.
! 
ELEMENTAL SUBROUTINE rand_number(state,x)
  IMPLICIT NONE
  TYPE(RandState), INTENT(INOUT) :: state
  REAL*8, INTENT(OUT) :: x
  LOGICAL :: valid
  
  valid = .FALSE.
  DO WHILE (.NOT. valid)
    x = state%U(state%I) - state%U(state%J)
    IF (x .LT. 0d0) x = x + 1d0
    state%U(state%I) = x
    
    state%I = state%I - 1
    IF (state%I .EQ. 0) state%I = 97
    state%J = state%J - 1
    IF (state%J .EQ. 0) state%J = 97
    
    state%C = state%C - state%CD
    IF (state%C .LT. 0d0) state%C = state%C + state%CM
    x = x - state%C
    IF (x .LT. 0d0) x = x + 1d0
    
    ! Avoid returning zero
    valid = (x .GT. 0d0) .AND. (x .LT. 1d0)
  END DO
  
END SUBROUTINE



!=======================================================================
! MATH FUNCTIONS
!=======================================================================

! ----------------------------------------------------------------------
! Returns the given number coerced into a range where it can be printed
! in at most w characters, optionally with a fixed number of decimal
! digits d (not using scientific notation).  Large magnitude numbers
! are reduced and very small magnitude numbers are set to zero.
! 
ELEMENTAL FUNCTION CoerceNumber(x,w,d) RESULT(y)
  IMPLICIT NONE
  REAL*8 :: y
  REAL*8, INTENT(IN) :: x
  INTEGER, INTENT(IN) :: w
  INTEGER, INTENT(IN), OPTIONAL :: d
  REAL*8 :: xmin,xmax
  y = x
  IF (PRESENT(d)) THEN
    xmin = (0.5d0+EPSILON(1d0)) * 10d0**(-d)
    IF (ABS(x) .LT. xmin) THEN
      y = 0d0
    ELSE IF (x .GE. 0) THEN
      xmax = 10d0**(w-d-1) - 2*xmin
      IF (x .GT. xmax) y = xmax
    ELSE
      xmax = 10d0**(w-d-2) - 2*xmin
      IF (ABS(x) .GT. xmax) y = -xmax
    END IF
  ELSE IF (x .GE. 0) THEN
    xmin = (0.5d0+EPSILON(1d0)) * 10d0**(2-w)
    xmax = 10d0**w - 1d0
    IF (x .LT. xmin) THEN
      y = 0d0
    ELSE IF (x .GT. xmax) THEN
      y = xmax
    END IF
  ELSE
    xmin = (0.5d0+EPSILON(1d0)) * 10d0**(3-w)
    xmax = 10d0**(w-1) - 1d0
    IF (ABS(x) .LT. xmin) THEN
      y = 0d0
    ELSE IF (ABS(x) .GT. xmax) THEN
      y = -xmax
    END IF
  END IF
  RETURN
END FUNCTION


! ----------------------------------------------------------------------
! Returns the given number coerced into a range with an exponent of at
! most N digits.  Large magnitude numbers are reduced and very small
! magnitude numbers are set to zero.  The number of significant digits
! Ns can optionally be given (controls how many 9's appear in upper
! cutoff).
! 
ELEMENTAL FUNCTION CoerceExponent(x,N,Ns) RESULT(y)
  IMPLICIT NONE
  REAL*8 :: y
  REAL*8, INTENT(IN) :: x
  INTEGER, INTENT(IN) :: N
  INTEGER, INTENT(IN), OPTIONAL :: Ns
  REAL*8 :: xmin,xmax
  xmin = 10d0**(-10**N+1)
  IF (PRESENT(Ns)) THEN
    xmax = (1-0.1d0**Ns)*10d0**(10**N)
  ELSE
    xmax = 0.9*10d0**(10**N)
  END IF
  IF (ABS(x) .LT. xmin) THEN
    y = 0d0
  ELSE IF (ABS(x) .GE. xmax) THEN
    y = SIGN(1d0,x)*xmax
  ELSE
    y = x
  END IF
  RETURN
END FUNCTION


!-----------------------------------------------------------------------
! Error function between x1 and x2, i.e. erf(x2)-erf(x1).
! This routine accounts for the case when x1 and x2 are similar
! and loss of precision will result from canceling when an explicit
! subtraction of two calls to the implicit ERF function is performed.
! 
! Implementation here appears to be valid to ~30*epsilon, where
! epsilon is the smallest unit of precision (e.g. 1e-15 for double
! precision).  Precision can get somewhat worse than this when
! x1,x2 > 15 or x1,x2 < -15, but only in cases where erf(x1,x2)
! < 1e-100.  The precision has been determined only by testing various
! cases using Mathematica and has not been formally proven.
! 
! Possibly useful identities:
!   erf(x)  = GammaP(1/2,x^2)
!   erfc(x) = GammaQ(1/2,x^2)
! 
ELEMENTAL FUNCTION ERF2(x1,x2) RESULT(z)
  IMPLICIT NONE
  REAL*8 :: z
  REAL*8, INTENT(IN) :: x1,x2
  REAL*8 :: xc,delx
  REAL*8, PARAMETER :: SQRTPI = 1.7724538509055160d0
  ! 1-eps is approximate level of cancelation at which to handle
  ! the difference more carefully
  REAL*8, PARAMETER :: EPS = 0.03d0
  
  ! Opposite sign: no canceling to worry about here
  IF (x1*x2 .LE. 0d0) THEN
    z = erf(x2) - erf(x1)
    RETURN
  END IF
  
  xc   = 0.5d0 * (x1+x2)
  delx = 0.5d0 * (x2-x1)
  
  ! Smaller arguments:
  !   |xc| < 1    --> |x1|,|x2| < 2
  ! Canceling is significant if |delx| < eps*|xc|
  IF ((ABS(xc) .LE. 1d0) .AND. (ABS(delx) .GT. EPS*ABS(xc))) THEN
    ! erf(x2) - erf(x1)
    z = erf(x2) - erf(x1)
    RETURN
    
  ! At least one argument is "large": 
  !   |xc| > 1    --> |x1| > 1 or |x2| > 1
  ! Canceling is significant if |4*xc*delx| < eps
  ELSE IF ((ABS(xc) .GT. 1d0) .AND. (ABS(4*xc*delx) .GT. EPS)) THEN
    IF (xc .GT. 0d0) THEN
      ! Difference of complementary error function gives better
      ! precision here:
      ! erf(x2) - erf(x1) = erfc(x1) - erfc(x2)
      z = -(erfc(x2) - erfc(x1))
    ELSE
      ! Difference of complementary error function gives better
      ! precision here (use symmetry of erf(x)):
      ! erf(x2) - erf(x1) = erf(-x1) - erf(-x2) = erfc(-x2) - erfc(-x1)
      z = erfc(-x2) - erfc(-x1)
    END IF
    RETURN
  END IF
  
  ! If we reached this point, there is significant canceling.
  ! For these cases, the integrand in the error function does not
  ! change much over x1 < x < x2.  Taylor expand the integrand
  ! about xc = (x1+x2)/2 and integrate.  The following keeps terms
  ! up through tenth order.
  z = 4 * delx * EXP(-xc**2) / SQRTPI                                   &
        * (1 + (2*xc**2 - 1)*delx**2 / 3                                &
             + (4*xc**4 - 12*xc**2 + 3)*delx**4 / 30                    &
             + (8*xc**6 - 60*xc**4 + 90*xc**2 - 15)*delx**6 / 630       &
             + (16*xc**8 - 224*xc**6 + 840*xc**4 - 840*xc**2            &
                         + 105)*delx**8 / 22680                         &
          )
  
END FUNCTION


!-----------------------------------------------------------------------
! The inverse error function, finding x given y such that y = ERF(x).
! 
! This implementation uses Halley's method.  It seems to be reasonable
! in terms of speed and precision and has had a cursory check for
! robustness, but there may be more optimal algorithms than this.
! Precision seems to be within ~10*EPSILON (usually even closer).
! 
ELEMENTAL FUNCTION ERFINV(y) RESULT(x)
  IMPLICIT NONE
  REAL*8 :: x
  REAL*8, INTENT(IN) :: y
  INTEGER :: K
  REAL*8 :: y0,y2,w,fk,delta
  REAL*8, PARAMETER :: QUARTERPI  = 0.78539816339744831d0  ! Pi/4
  REAL*8, PARAMETER :: INVSQRTPI  = 0.56418958354775629d0  ! 1/Sqrt(Pi)
  
  y0 = ABS(y)
  y2 = y*y
  
  ! Initial guess.  Taken from B.A. Popov, ACM SIGSAM 34, 25 (2000).
  IF (y0 .EQ. 0d0) THEN
    x = 0d0
    RETURN
  ELSE IF (y0 .LE. 0.97314979d0) THEN
    x = y*(-0.95493118d0+y2*(0.53160534d0+y2*0.23343441d0)) / (-1.0977154d0+y2)
  ELSE IF (y0 .LE. 0.99767065d0) THEN
    x = y*(1.6200516d0+y2*(-4.9295187d0+y2*3.2890636d0)) / (-1.0083317d0+y2)
  ELSE IF (y0 .LE. 0.99978842d0) THEN
    x = y*(29.849915d0+y2*(-61.896833d0+y2*32.044810d0)) / (-1.0007339d0+y2)
  ELSE IF (y0 .LT. 1d0) THEN
    w = -LOG(1-y2)
    x = SIGN(SQRT(w - 0.5d0*LOG(QUARTERPI*w)),y)
  ELSE
    x = SIGN(HUGE(x),y)
    RETURN
  END IF
  
  ! Halley's method.  Checked by hand that double precision is achieved
  ! by three iterations for all tested cases (epsilon, 1-epsilon, +/-,
  ! etc).  No safety checks as the algorithm appears to be convergent
  ! in all cases (though not proved).
  DO K=1,3
    fk    = ERF(x) - y
    ! Newton's
    !delta = - fk / (2*INVSQRTPI*EXP(-x**2))
    ! Halley's
    delta = fk / (x*fk - 2*INVSQRTPI*EXP(-x**2))
    x     = x + delta
  END DO
  
END FUNCTION


!-----------------------------------------------------------------------
! The inverse complementary error function, finding x given y such that
! y = ERFC(x).
! 
! This is a quick implementation that has not been optimized for speed
! and has not been tested for precision (though it should be close to
! double precision in most cases).
! 
ELEMENTAL FUNCTION ERFCINV(y) RESULT(x)
  IMPLICIT NONE
  REAL*8 :: x
  REAL*8, INTENT(IN) :: y
  INTEGER :: K
  REAL*8 :: fk,delta
  REAL*8, PARAMETER :: INVPI      = 0.31830988618379067d0  ! 1/Pi
  REAL*8, PARAMETER :: INVSQRTPI  = 0.56418958354775629d0  ! 1/Sqrt(Pi)

  ! Use ERFINV via the relation erfc^-1(1-z) = erf^-1(z).
  ! Below algorith (Halley's method, 5 iterations) works fine for
  ! y < 0.5 and possibly to higher y values, but ERFINV is faster
  ! (by x2-5), so we use that where loss of precision is minimal.
  IF (y .GT. 0.01d0) THEN
    x = ERFINV(1d0-y)
    RETURN
  ELSE IF (y .LE. 0d0) THEN
    x = HUGE(x)
    RETURN
  END IF

  ! Only small, positive y at this point.

  ! Initial guess: invert first term of asymptotic expansion
  !   y = erfc(x) = e^{-x^2} / (x\sqrt{\pi}) [1 + O(1/x^2)]
  x = SQRT(0.5d0*LAMBERTW(2*INVPI/y**2))

  ! Halley's method.  Checked by hand that double precision is achieved
  ! by five iterations for tested cases (epsilon, ~ 0.5).  No safety
  ! checks as the algorithm appears to be convergent in all cases
  ! (though not proved).
  DO K=1,5
    fk    = ERFC(x) - y
    ! Newton's
    !delta = fk / (2*INVSQRTPI*EXP(-x**2))
    ! Halley's
    delta = fk / (x*fk + 2*INVSQRTPI*EXP(-x**2))
    x     = x + delta
  END DO

END FUNCTION


!-----------------------------------------------------------------------
! The value of exp(x2)-exp(x1).
! Accounts for the case when x2-x1 is small and loss of precision
! due to canceling of the two terms can occur.
ELEMENTAL FUNCTION EXP2(x1,x2) RESULT(z)
  IMPLICIT NONE
  REAL*8 :: z
  REAL*8, INTENT(IN) :: x1,x2
  REAL*8 :: x,yk
  INTEGER :: K
  x = x2-x1
  ! Good to ~ 100*EPSILON precision (matched to sum terms below)
  IF (ABS(x) .GT. 0.007d0) THEN
    z = EXP(x2) - EXP(x1)
    RETURN
  END IF
  IF (x1 .EQ. x2) THEN
    z = 0d0
    RETURN
  END IF
  ! Can write: e^x2 - e^x1 = e^x1 [e^(x2-x1) - 1]
  ! Here, we find e^(x2-x1) - 1
  K = 1
  yk = x
  z = yk
  ! Nearly full precision, but does check limit optimization?
  !DO WHILE (ABS(yk) .GT. EPSILON(1d0)*ABS(z))
  !  K = K+1
  !  yk = yk*x/K
  !  z = z + yk
  !END DO
  ! Optimization: might be faster to calculate a fixed number
  !               of terms rather than check after each term if
  !               further terms are required.
  ! Possibilities:
  !     Good to ~   20*EPSILON in 7 terms for |x| < 0.04.
  !     Good to ~   50*EPSILON in 6 terms for |x| < 0.02.
  !     Good to ~  100*EPSILON in 5 terms for |x| < 0.007.
  !     Good to ~  500*EPSILON in 4 terms for |x| < 0.002.
  !     Good to ~ 4000*EPSILON in 3 terms for |x| < 0.00025.
  DO K = 2,5
    yk = yk*x/K
    z = z + yk
  END DO
  z = EXP(x1)*z
END FUNCTION


!-----------------------------------------------------------------------
! The value of exp(x)-1.
! Accounts for the case when x is small and loss of precision due
! to canceling of the two terms can occur.
ELEMENTAL FUNCTION EXPm1(x) RESULT(z)
  IMPLICIT NONE
  REAL*8 :: z
  REAL*8, INTENT(IN) :: x
  z = EXP2(0d0,x)
END FUNCTION


! ----------------------------------------------------------------------
! Function to calculate the quantity:
!   ln(1+x)
! Accounts for the case where x is small.
! 
! Precision is ~ 100*EPSILON.
! 
ELEMENTAL FUNCTION LOGp1(x) RESULT(z)
  IMPLICIT NONE
  REAL*8 :: z
  REAL*8, INTENT(IN) :: x
  REAL*8 :: xabs,xk
  INTEGER :: k,klast
  
  xabs = ABS(x)
  
  ! If x is not too small, we can just evaluate explicitly
  IF (xabs .GT. 0.01d0) THEN
    z = LOG(1+x)
    RETURN
  END IF
  
  ! We will use an expansion about zero, keeping terms in the
  ! expansion up to x^klast/klast.
  ! klast is chosen below to give almost double precision.
  ! Precision can be as poor as ~ 100*EPSILON, but is better
  ! for smaller x.  The expansion is:
  !     \sum_{k=1}^{\infty} (-1)^{k+1} x^k / k
  ! The precision is approximately:
  !     x^klast / (klast+1)
  ! where klast is the last term in the sum.
  
  IF (xabs .LT. 1d-4) THEN
    klast = 4
  ELSE
    klast = 8
  END IF
  ! Go to klast=12 for |x| < 0.05
  ! Go to klast=16 for |x| < 0.1
  
  ! Use expansion about zero
  xk = x
  z  = xk
  DO k = 2,klast
    xk      = -x*xk
    z = z + xk/k
  END DO
  
END FUNCTION LOGp1


! ----------------------------------------------------------------------
! Function to calculate the quantity ln(a+b) given ln(a) and ln(b).
! Precision is ~ EPSILON.
! 
ELEMENTAL FUNCTION LOG_SUM(lna,lnb) RESULT(z)
  IMPLICIT NONE
  REAL*8 :: z
  REAL*8, INTENT(IN) :: lna,lnb
  REAL*8 :: lnx,r
  
  IF (lna .GE. lnb) THEN
    lnx = lna
    r   = EXP(lnb-lna)
  ELSE
    lnx = lnb
    r   = EXP(lna-lnb)
  END IF
  
  ! Below, we use an expansion of ln(1+r) if r is small.
  ! The sum is terminated at approximately double precision.
  IF (r .EQ. 0d0) THEN
    z = lnx
  ELSE IF (r .GT. 0.01d0) THEN
    z = lnx + LOG(1d0 + r)
  ELSE IF (r .GT. 0.001d0) THEN
    z = lnx + r*(1d0                                                    &
                 -r*((1d0/2d0)                                          &
                     -r*((1d0/3d0)                                      &
                         -r*((1d0/4d0)                                  &
                             -r*((1d0/5d0)                              &
                                 -r*((1d0/6d0)                          &
                                     -r*((1d0/7d0)                      &
                                         -r*((1d0/8d0)                  &
                                             -r*(1d0/9d0)               &
                ))))))))
  ELSE IF (r .GT. 0.00001d0) THEN
    z = lnx + r*(1d0                                                    &
                 -r*((1d0/2d0)                                          &
                     -r*((1d0/3d0)                                      &
                         -r*((1d0/4d0)                                  &
                             -r*((1d0/5d0)                              &
                                 -r*(1d0/6d0)                           &
                )))))
  ELSE
    z = lnx + r*(1d0                                                    &
                 -r*((1d0/2d0)                                          &
                     -r*((1d0/3d0)                                      &
                         -r*(1d0/4d0)                                   &
                )))
  END IF
  
END FUNCTION


!-----------------------------------------------------------------------
! Factorial
! 
ELEMENTAL FUNCTION FACTORIAL(n) RESULT(z)
  IMPLICIT NONE
  INTEGER, INTENT(IN) :: n
  INTEGER :: z
  ! Tabulated values
  INTEGER, PARAMETER :: NMAX = 20
  INTEGER, PARAMETER, DIMENSION(0:NMAX) :: FVALS = &
    (/ 1,                                           & ! n = 0
       1,                  2,                    & ! n = 1-2
       6,                  24,                   & ! n = 3-4
       120,                720,                  & ! n = 5-6
       5040,               40320,                & ! n = 7-8
       362880,             3628800,              & ! n = 9-10
       39916800,           479001600,            & ! n = 11-12
       6227020800,         87178291200,          & ! n = 13-14
       1307674368000,      20922789888000,       & ! n = 15-16
       355687428096000,    6402373705728000,     & ! n = 17-18
       121645100408832000, 2432902008176640000  /) ! n = 19-20
  IF (n .LT. 0) THEN
    z = -HUGE(n)
    !STOP 'ERROR: factorial cannot be called with negative argument'
  ELSE IF (n .LE. NMAX) THEN
    z = FVALS(n)
  ELSE
    z = HUGE(n)
  END IF
END FUNCTION


!-----------------------------------------------------------------------
! Gamma function of integer argument [double precision]
! NOTE: Intrinsic routine of real argument available in Fortran 2008;
!       the speed of the intrinsic routine may be comparable to this
!       tabulated case.
! 
ELEMENTAL FUNCTION GAMMAI(n) RESULT(z)
  IMPLICIT NONE
  INTEGER, INTENT(IN) :: n
  REAL*8 :: z
  REAL*8 :: x,xinv
  INTEGER, PARAMETER :: NMAX  = 171    ! n > NMAX exceeds HUGE(1d0)
  ! Tabulated values
  ! NOTE: number of continuation lines required to place all tabulated
  !       values into one table exceeds F95 spec (>39).
  !       Both Intel and GNU Fortran compilers allow more continuation
  !       lines, but we use multiple tables for portability.
  INTEGER, PARAMETER :: NMAX1 = 100
  INTEGER, PARAMETER :: NMAX2 = 171
  ! Table for n=1-100
  REAL*8, PARAMETER, DIMENSION(1:NMAX1) :: GVALS1 = &
    (/  1.0d0,                  1.0d0,                  2.0d0,                  6.0d0,                  & ! 1-4
        24.0d0,                 120.0d0,                720.0d0,                5040.0d0,               & ! 5-8
        40320.0d0,              362880.0d0,             3.6288000000000000d6,   3.9916800000000000d7,   & ! 9-12
        4.7900160000000000d8,   6.2270208000000000d9,   8.7178291200000000d10,  1.3076743680000000d12,  & ! 13-16
        2.0922789888000000d13,  3.5568742809600000d14,  6.4023737057280000d15,  1.2164510040883200d17,  & ! 17-20
        2.4329020081766400d18,  5.1090942171709440d19,  1.1240007277776077d21,  2.5852016738884977d22,  & ! 21-24
        6.2044840173323944d23,  1.5511210043330986d25,  4.0329146112660564d26,  1.0888869450418352d28,  & ! 25-28
        3.0488834461171386d29,  8.8417619937397020d30,  2.6525285981219106d32,  8.2228386541779228d33,  & ! 29-32
        2.6313083693369353d35,  8.6833176188118865d36,  2.9523279903960414d38,  1.0333147966386145d40,  & ! 33-36
        3.7199332678990122d41,  1.3763753091226345d43,  5.2302261746660111d44,  2.0397882081197443d46,  & ! 37-40
        8.1591528324789773d47,  3.3452526613163807d49,  1.4050061177528799d51,  6.0415263063373836d52,  & ! 41-44
        2.6582715747884488d54,  1.1962222086548019d56,  5.5026221598120889d57,  2.5862324151116818d59,  & ! 45-48
        1.2413915592536073d61,  6.0828186403426756d62,  3.0414093201713378d64,  1.5511187532873823d66,  & ! 49-52
        8.0658175170943879d67,  4.2748832840600256d69,  2.3084369733924138d71,  1.2696403353658276d73,  & ! 53-56
        7.1099858780486345d74,  4.0526919504877217d76,  2.3505613312828786d78,  1.3868311854568984d80,  & ! 57-60
        8.3209871127413901d81,  5.0758021387722480d83,  3.1469973260387938d85,  1.9826083154044401d87,  & ! 61-64
        1.2688693218588416d89,  8.2476505920824707d90,  5.4434493907744306d92,  3.6471110918188685d94,  & ! 65-68
        2.4800355424368306d96,  1.7112245242814131d98,  1.1978571669969892d100, 8.5047858856786232d101, & ! 69-72
        6.1234458376886087d103, 4.4701154615126843d105, 3.3078854415193864d107, 2.4809140811395398d109, & ! 73-76
        1.8854947016660503d111, 1.4518309202828587d113, 1.1324281178206298d115, 8.9461821307829753d116, & ! 77-80
        7.1569457046263802d118, 5.7971260207473680d120, 4.7536433370128417d122, 3.9455239697206587d124, & ! 81-84
        3.3142401345653533d126, 2.8171041143805503d128, 2.4227095383672732d130, 2.1077572983795277d132, & ! 85-88
        1.8548264225739844d134, 1.6507955160908461d136, 1.4857159644817615d138, 1.3520015276784030d140, & ! 89-92
        1.2438414054641307d142, 1.1567725070816416d144, 1.0873661566567431d146, 1.0329978488239059d148, & ! 93-96
        9.9167793487094969d149, 9.6192759682482120d151, 9.4268904488832477d153, 9.3326215443944153d155 /) ! 97-100
  ! Table for n=101-171
  REAL*8, PARAMETER, DIMENSION(NMAX1+1:NMAX2) :: GVALS2 = &
    (/  9.3326215443944153d157, 9.4259477598383594d159, 9.6144667150351266d161, 9.9029007164861804d163, & ! 101-104
        1.0299016745145628d166, 1.0813967582402909d168, 1.1462805637347084d170, 1.2265202031961379d172, & ! 105-108
        1.3246418194518290d174, 1.4438595832024936d176, 1.5882455415227429d178, 1.7629525510902447d180, & ! 109-112
        1.9745068572210740d182, 2.2311927486598136d184, 2.5435597334721876d186, 2.9250936934930157d188, & ! 113-116
        3.3931086844518982d190, 3.9699371608087209d192, 4.6845258497542907d194, 5.5745857612076059d196, & ! 117-120
        6.6895029134491271d198, 8.0942985252734437d200, 9.8750442008336014d202, 1.2146304367025330d205, & ! 121-124
        1.5061417415111409d207, 1.8826771768889261d209, 2.3721732428800469d211, 3.0126600184576595d213, & ! 125-128
        3.8562048236258042d215, 4.9745042224772874d217, 6.4668554892204737d219, 8.4715806908788205d221, & ! 129-132
        1.1182486511960043d224, 1.4872707060906857d226, 1.9929427461615189d228, 2.6904727073180505d230, & ! 133-136
        3.6590428819525487d232, 5.0128887482749917d234, 6.9177864726194885d236, 9.6157231969410890d238, & ! 137-140
        1.3462012475717525d241, 1.8981437590761710d243, 2.6953641378881628d245, 3.8543707171800728d247, & ! 141-144
        5.5502938327393048d249, 8.0479260574719919d251, 1.1749972043909108d254, 1.7272458904546389d256, & ! 145-148
        2.5563239178728656d258, 3.8089226376305697d260, 5.7133839564458546d262, 8.6272097742332404d264, & ! 149-152
        1.3113358856834525d267, 2.0063439050956824d269, 3.0897696138473509d271, 4.7891429014633939d273, & ! 153-156
        7.4710629262828944d275, 1.1729568794264144d278, 1.8532718694937348d280, 2.9467022724950383d282, & ! 157-160
        4.7147236359920613d284, 7.5907050539472187d286, 1.2296942187394494d289, 2.0044015765453026d291, & ! 161-164
        3.2872185855342962d293, 5.4239106661315888d295, 9.0036917057784374d297, 1.5036165148649990d300, & ! 165-168
        2.5260757449731984d302, 4.2690680090047053d304, 7.2574156153079990d306  /)                        ! 169-171
  ! Coefficients for asymptotic expansion
  ! Not all terms are necessary or used here
  INTEGER, PARAMETER :: NC = 20
  REAL*8, PARAMETER, DIMENSION(0:NC) :: C = &
    (/  1.0d0,                                                  &
        0.083333333333333333d0,     0.0034722222222222222d0,    &
       -0.0026813271604938272d0,   -0.00022947209362139918d0,   &
        0.00078403922172006663d0,   0.000069728137583658578d0,  &
       -0.00059216643735369388d0,  -0.000051717909082605922d0,  &
        0.00083949872067208728d0,   0.000072048954160200106d0,  &
       -0.0019144384985654775d0,   -0.00016251626278391582d0,   &
        0.0064033628338080698d0,    0.00054016476789260452d0,   &
       -0.029527880945699121d0,    -0.0024817436002649977d0,    &
        0.17954011706123486d0,      0.015056113040026424d0,     &
       -1.3918010932653375d0,      -0.1165462765994632d0       /)
  
  IF (n .LE. 0) THEN
    z = -HUGE(z)
    !STOP 'ERROR: gamma cannot be called with non-positive argument'
  ELSE IF (n .GT. NMAX) THEN
    z = HUGE(z)
  ELSE IF (n .LE. NMAX1) THEN
    z = GVALS1(n)
  ELSE IF (n .LE. NMAX2) THEN
    z = GVALS2(n)
  ELSE
    ! This case should not occur
    z = HUGE(z)
  END IF
  ! Algorithm here unnecessary, but shown for reference
  !! Asymptotic expansion (Stirling's)
  !! Error will be ~ C[5]/x^5
  !! Good to ~ 1e-15 for n > 171
  !! Result would be HUGE(1d0) for n > 171
  !x = n
  !xinv = 1d0/x
  !z = x**(x-0.5d0) * EXP(-x) * SQRT2PI                               &
  !    * (C(0) +                                                      &
  !       xinv*(C(1) +                                                &
  !             xinv*(C(2) +                                          &
  !                   xinv*(C(3) +                                    &
  !                         xinv*(C(4) +                              &
  !                               xinv*C(5) ) ) ) ) )
  !! Alternatively, could use LogGamma function for large n
  !!x = n
  !!z = EXP(LOG_GAMMA(x))
END FUNCTION


!-----------------------------------------------------------------------
! Lower gamma function of real arguments [double precision]
!   \gamma(s,x) = \int_0^x dt t^{s-1} e^{-t}
! 
ELEMENTAL FUNCTION LOWER_GAMMA(s,x) RESULT(z)
  IMPLICIT NONE
  REAL*8, INTENT(IN) :: s,x
  REAL*8 :: z
  REAL*8 :: P,Q
  CALL GAMMA_PQ(s,x,P,Q)
  z = GAMMA(s) * P
END FUNCTION


!-----------------------------------------------------------------------
! Upper gamma function of real arguments [double precision]
!   \Gamma(s,x) = \int_x^\infty dt t^{s-1} e^{-t}
! 
ELEMENTAL FUNCTION UPPER_GAMMA(s,x) RESULT(z)
  IMPLICIT NONE
  REAL*8, INTENT(IN) :: s,x
  REAL*8 :: z
  REAL*8 :: P,Q
  CALL GAMMA_PQ(s,x,P,Q)
  z = GAMMA(s) * Q
END FUNCTION


!-----------------------------------------------------------------------
! Regularized incomplete gamma functions P, Q of real arguments
! [double precision].  These are defined as:
!   P(s,x) = \gamma(s,x) / \Gamma(s)
!   Q(s,x) = \Gamma(s,x) / \Gamma(s)
! where \gamma(s,x) & \Gamma(s,x) are the lower & upper incomplete
! gamma functions:
!   \gamma(s,x) = \int_0^x dt t^{s-1} e^{-t}
!   \Gamma(s,x) = \int_x^\infty dt t^{s-1} e^{-t}
! The following relations hold:
!   P(s,x) + Q(s,x) = 1
!   \gamma(s,x) + \Gamma(s,x) = \Gamma(s)
! The below routine calculates the approximately smaller of P or Q
! and uses P+Q=1 to determine the other.
! 
! NOTE: Valid only for s,x >= 0 and not intended for large s.
! 
ELEMENTAL SUBROUTINE GAMMA_PQ(s,x,P,Q)
  IMPLICIT NONE
  REAL*8, INTENT(IN) :: s,x
  REAL*8, INTENT(OUT) :: P,Q
  
  ! Special cases
  IF (x .EQ. 0d0) THEN
    P = 0d0
    Q = 1d0
    RETURN
  END IF
  
  ! Bad cases
  IF ((x .LT. 0d0) .OR. (s .LT. 0d0)) THEN
    P = -HUGE(1d0)
    Q = HUGE(1d0)
    RETURN
  END IF
  
  ! Calculate P using Taylor series
  IF (x .LE. s) THEN
    P = GAMMA_P_TS(s,x)
    Q = 1-P
  ! Calculate Q using continued fraction
  ELSE
    Q = GAMMA_Q_CF(s,x)
    P = 1-Q
  END IF
  
  CONTAINS
  
  !---------------------------------------------
  ! Calculates gamma function quantity P(s,x) using a Taylor
  ! series expansion.
  !   P(s,x) = \gamma(s,x) / \Gamma(s)
  !   \gamma(s,x) = \int_0^x dt t^{s-1} e^{-t}
  ! NOTE: Requires 0 < s, 0 <= x.  Should only be used for x
  !       not (much) larger than s or convergence may be slow.
  !       Also best for s not large (timing scales as maybe
  !       sqrt(s)).
  PURE FUNCTION GAMMA_P_TS(s,x) RESULT(P)
    IMPLICIT NONE
    REAL*8, INTENT(IN) :: s,x
    REAL*8 :: P
    INTEGER :: K
    REAL*8 :: zk,sum
    REAL*8 :: LOG_GAMMA
    ! P(s,x) = x^s e^{-x} \Sum_{k=0}^\infty x^k / \Gamma(s+k+1)
    K   = 0
    zk  = 1d0
    sum = zk
    DO WHILE (zk .GT. EPSILON(zk))
      K   = K+1
      zk  = zk * x / (s+K)
      sum = sum + zk
      IF (K .GE. 10000) EXIT
    END DO
    P = sum * EXP(s*LOG(x) - x - LOG_GAMMA(s+1))
  END FUNCTION
  
  !---------------------------------------------
  ! Calculates gamma function quantity Q(s,x) using a
  ! continued fraction.
  !   Q(s,x) = \Gamma(s,x) / \Gamma(s)
  !   \Gamma(s,x) = \int_x^{\infty} dt t^{s-1} e^{-t}
  ! NOTE: Requires 0 < x,s.  Should only be used for x not
  !       (much) smaller than s or convergence may be slow
  !       (or simply fail).  Also best for s not large
  !       (timing scales as maybe sqrt(s)).
  PURE FUNCTION GAMMA_Q_CF(s,x) RESULT(Q)
    IMPLICIT NONE
    REAL*8, INTENT(IN) :: s,x
    REAL*8 :: Q
    INTEGER :: K
    REAL*8 :: xs1,fk,Ck,Dk,Deltak
    REAL*8 :: LOG_GAMMA
    ! Continued fraction (see Numerical Recipes):
    !   Q(s,x) = x^s e^{-x} / \Gamma(s)
    !            / (x-s+1 + K_k(-k(k-s),-s+2k+x+1)_{1}^{\infty})
    ! where
    !   K_k(a_k,b_k)_1 = a_1/b1+ a2/b2+ a3/b3+ ...
    ! is the Gauss notation for a continued fraction.
    xs1 = x-s+1
    K  = 0
    Ck = xs1
    Dk = 0
    fk = xs1
    Deltak = HUGE(x)
    DO WHILE (ABS(Deltak-1) .GE. EPSILON(x))
      K  = K+1
      Ck = (xs1+2*K) - K*(K-s)/Ck
      IF (Ck .EQ. 0d0) Ck = 1d-30
      Dk = (xs1+2*K) - K*(K-s)*Dk
      IF (Dk .EQ. 0d0) Dk = 1d-30
      Dk = 1/Dk
      Deltak = Ck*Dk
      fk = Deltak*fk
      IF (K .GE. 10000) EXIT
    END DO
    Q = EXP(s*LOG(x) - x - LOG_GAMMA(s)) / fk
  END FUNCTION
  
END SUBROUTINE


!-----------------------------------------------------------------------
! Binomial coefficient (n k) = n!/k!(n-k)! [integer]
! Various techniques are used to provide a fast calculation in
! different cases, but no check for integer overflow is performed
! for the final result.
! 
ELEMENTAL FUNCTION BINOMIAL_COEFF(n,k) RESULT(z)
  IMPLICIT NONE
  INTEGER, INTENT(IN) :: n,k
  INTEGER :: z
  INTEGER :: K0,J
  ! Array of coefficients indexed by [K,N] for K <= N/2
  INTEGER, PARAMETER :: BC_ARRAY(0:6,0:12) = RESHAPE(                   &
    (/ 1,  0,  0,   0,   0,   0,   0,                                   & ! n=0
       1,  1,  0,   0,   0,   0,   0,                                   & ! n=1
       1,  2,  1,   0,   0,   0,   0,                                   & ! n=2
       1,  3,  3,   1,   0,   0,   0,                                   & ! n=3
       1,  4,  6,   4,   1,   0,   0,                                   & ! n=4
       1,  5, 10,  10,   5,   1,   0,                                   & ! n=5
       1,  6, 15,  20,  15,   6,   1,                                   & ! n=6
       1,  7, 21,  35,  35,  21,   7,                                   & ! n=7
       1,  8, 28,  56,  70,  56,  28,                                   & ! n=8
       1,  9, 36,  84, 126, 126,  84,                                   & ! n=9
       1, 10, 45, 120, 210, 252, 210,                                   & ! n=10
       1, 11, 55, 165, 330, 462, 462,                                   & ! n=11
       1, 12, 66, 220, 495, 792, 924  /),                               & ! n=12
    (/7,13/) )
  IF ((n .LT. k) .OR. (k .LT. 0)) THEN
    z = -HUGE(n)
    !STOP 'ERROR: invalid binomial coefficient arguments (0 <= k <= n)'
    RETURN
  END IF
  ! Use (n k) = (n n-k)
  K0 = MIN(k,n-k)
  ! Use tabulated values for small n
  IF (n .LE. 12) THEN
    z = BC_ARRAY(K0,n)
  ! Use recursion: (n k) = n/k (n-1 k-1)
  ! Note overflow for large enough k and/or n.
  ! Below check should avoid all potential overflow cases for
  ! 4-byte integers.
  ELSE IF (n*K0*K0 .LT. 2842) THEN
    z = 1d0
    DO J=1,K0
      z = (z*(n-K0+J))/J
    END DO
  ! Use LogGamma for large arguments.
  ! Loss of precision for very large N (> 20,000,000) and K = 1.  Other
  ! cases seem to give full INTEGER*4 precision when not in an overflow
  ! condition.
  ELSE
    z = NINT(EXP(LOG_GAMMA(n+1d0) - LOG_GAMMA(k+1d0) - LOG_GAMMA(n-k+1d0)))
  END IF
END FUNCTION


!-----------------------------------------------------------------------
! Calculates W(x) to double precision, where W is the Lambert W
! function defined as the solution w = W(x) to the equation:
!     x = w e^w
! Returns the principal branch value (the larger of two solutions
! over -1/e < x < 0; there is only one solution for x > 0).  The
! W function is undefined for x < -1/e.
! 
! Valid input:  -1/e < x < \infty
! 
ELEMENTAL FUNCTION LAMBERTW(x) RESULT(w)
  IMPLICIT NONE
  REAL*8 :: w
  REAL*8, INTENT(IN) :: x
  REAL*8 :: epsk,zk,qk,wk,wk1,p,num,den,a,b,ia
  REAL*8, PARAMETER :: e    = 2.7182818284590452d0
  REAL*8, PARAMETER :: einv = 0.36787944117144232d0
  
  ! No solution for x < 1/e
  IF (x .LT. -einv) THEN
    !STOP 'Error in lambertw: argument must be larger than -EXP(-1)'
    w = -HUGE(w)
    RETURN
  END IF
  
  ! Could use Newton's or Halley's iteration method to find the
  ! solution, but the Lambert function has a faster method,
  ! Fritsch's iteration:
  !    W_{k+1} = W_k * (1 + eps_k)
  ! with
  !    eps_k   = z_k/(1 + W_k) * (q_k - z_k)/(q_k - 2*z_k)
  !    z_k     = ln(x/W_k) - W_k
  !    q_k     = 2 (1 + W_k) (1 + W_k + (2/3)*z_k)
  ! If eps_k is the error in W_k, then the error in W_{k+1} is of
  ! order (eps_k)^4, a faster convergent that Halley's method.
  ! For a first guess accurate to order O(10^-4), double precision
  ! can be achieved in only a single iteration.  We use estimates
  ! for the initial guess as determined by Veberic.
  ! 
  ! For further information, see:
  !   D. Veberic, arXiv:1003.1628.
  !   F.N. Fritsch, R.E. Shafer and W.P. Crowley,
  !       Commun. ACM 16, 123 (1973).
  
  ! Initial estimate by Veberic
  IF (x .LT. -0.32358170806015724d0) THEN
    ! branch point expansion
    p = SQRT(2*(1+e*x))
    wk = -1 + p*(1 + p*(-1d0/3 + p*(11d0/72 + p*(-43d0/540              &
              + p*(769d0/17280 + p*(-221d0/8505                         &
              + p*(680863d0/43545600 + p*(-1963d0/204120                &
              + p*(226287557d0/37623398400d0) ))))))))
  ELSE IF (x .LT. 0.14546954290661823d0) THEN
    ! rational fit
    num = x * (1 + x *                                                  &
              (5.931375839364438d0 + x *                                &
              (11.392205505329132d0+ x *                                &
              (7.338883399111118d0 + x * 0.6534490169919599d0) )))
    den = 1 + x *                                                       &
             (6.931373689597704d0 + x *                                 &
             (16.82349461388016d0 + x *                                 &
             (16.43072324143226d0 + x * 5.115235195211697d0) ))
    wk = num / den
  ELSE IF (x .LT. 8.706658967856612d0) THEN
    ! rational fit
    num = x * (1 + x *                                                  &
              (2.4450530707265568d0 + x *                               &
              (1.3436642259582265d0 + x *                               &
              (0.14844005539759195d0+ x * 8.047501729129999d-4) )))
    den = 1 + x *                                                       &
             (3.4447089864860025d0 + x *                                &
             (3.2924898573719523d0 + x *                                &
             (0.9164600188031222d0 + x * 0.05306864044833221d0) ))
    wk = num / den
  ELSE
    ! asymptotic expansion
    a = LOG(x)
    b = LOG(a)
    ia = 1/a
    wk = a - b + b * ia *                                               &
                (1 + ia *                                               &
                (0.5d0*(-2 + b) + ia *                                  &
                (1/6d0*(6 + b*(-9 + b*2)) + ia *                        &
                (1/12d0*(-12 + b*(36 + b*(-22 + b*3))) + ia *           &
                 1/60d0*(60 + b*(-300 + b*(350 + b*(-125 + b*12))))     &
                ))))
  END IF
  
  ! Special cases:
  ! For x equal to 0 or -1/e, the Fritsch iteration does
  ! not work as some of the terms go to infinity.  However,
  ! for x sufficiently near 0 or -1/e, the above first
  ! approximation is already nearly double precision.
  IF ((ABS(x) .LT. 1d-7) .OR. (x .LT. -einv+1d-6) ) THEN
    w = wk
    RETURN
  END IF
  
  ! Now apply Fritsch iteration
  wk1  = wk + 1
  zk   = LOG(x/wk) - wk
  qk   = 2 * wk1 * (wk1 + (2d0/3)*zk)
  epsk = zk * (qk - zk) / (wk1 * (qk - 2*zk))
  wk   = wk * (1 + epsk)
  ! In most cases, no further iterations will be necessary
  DO WHILE (ABS(epsk) .GT. 1d-5)
    wk1  = wk + 1
    zk   = LOG(x/wk) - wk
    qk   = 2 * wk1 * (wk1 + (2d0/3)*zk)
    epsk = zk * (qk - zk) / (wk1 * (qk - 2*zk))
    wk   = wk * (1 + epsk)
  END DO
  
  w = wk
  
END FUNCTION


!-----------------------------------------------------------------------
! Calculates W(x) to double precision, where W is the Lambert W
! function defined as the solution w = W(x) to the equation:
!     x = w e^w
! Returns the secondary branch value (the smaller of two solutions
! over -1/e < x < 0).  This branch is defined only for
! -1/e < x < 0.
! 
! Valid input:  -1/e < x < 0
! 
ELEMENTAL FUNCTION LAMBERTW2(x) RESULT(w2)
  IMPLICIT NONE
  REAL*8 :: w2
  REAL*8, INTENT(IN) :: x
  INTEGER :: k
  REAL*8 :: epsk,zk,qk,wk,wk1,p,num,den,a
  REAL*8, PARAMETER :: e    = 2.7182818284590452d0
  REAL*8, PARAMETER :: einv = 0.36787944117144232d0
  
  ! No solution for x < 1/e
  IF (x .LT. -einv) THEN
    !STOP 'Error in lambertw2: argument must be larger than -EXP(-1)'
    w2 = HUGE(w2)
    RETURN
  END IF
  
  ! No second branch for x > 0
  IF (x .GT. 0) THEN
    !STOP 'Error in lambertw2: argument must be smaller than 0'
    w2 = HUGE(w2)
    RETURN
  END IF
  
  ! Limit as x ->0 is -\infty
  IF (x .EQ. 0) THEN
    w2 = -HUGE(w2)
    RETURN
  END IF
  
  ! Could use Newton's or Halley's iteration method to find the
  ! solution, but the Lambert function has a faster method,
  ! Fritsch's iteration:
  !    W_{k+1} = W_k * (1 + eps_k)
  ! with
  !    eps_k   = z_k/(1 + W_k) * (q_k - z_k)/(q_k - 2*z_k)
  !    z_k     = ln(x/W_k) - W_k
  !    q_k     = 2 (1 + W_k) (1 + W_k + (2/3)*z_k)
  ! If eps_k is the error in W_k, then the error in W_{k+1} is of
  ! order (eps_k)^4, a faster convergent that Halley's method.
  ! For a first guess accurate to order O(10^-4), double precision
  ! can be achieved in only a single iteration.  We use estimates
  ! for the initial guess as determined by Veberic.
  ! 
  ! For further information, see:
  !   D. Veberic, arXiv:1003.1628.
  !   F.N. Fritsch, R.E. Shafer and W.P. Crowley,
  !       Commun. ACM 16, 123 (1973).
  
  ! Initial estimate by Veberic
  IF (x .LT. -0.30298541769d0) THEN
    ! branch point expansion
    p = -SQRT(2*(1+e*x))
    wk = -1 + p*(1 + p*(-1d0/3 + p*(11d0/72 + p*(-43d0/540              &
              + p*(769d0/17280 + p*(-221d0/8505                         &
              + p*(680863d0/43545600 + p*(-1963d0/204120                &
              + p*(226287557d0/37623398400d0) ))))))))
  ELSE IF (x .LT. -0.051012917658221676d0) THEN
    ! rational fit
    num = -7.814176723907436d0 + x *                                    &
            (253.88810188892484d0 + x * 657.9493176902304d0)
    den = 1 + x *                                                       &
             (-60.43958713690808d0+ x *                                 &
             (99.98567083107612d0 + x *                                 &
             (682.6073999909428d0 + x *                                 &
             (962.1784396969866d0 + x * 1477.9341280760887d0) )))
    wk = num / den
  ELSE
    ! continued logarithm
    a  = LOG(-x)
    wk = a
    DO k=1,9
      wk = a - LOG(-wk)
    END DO
  END IF
  
  ! Special cases:
  ! For x equal to -1/e, the Fritsch iteration does not
  ! not work as some of the terms go to infinity.  However,
  ! for x sufficiently near -1/e, the above first
  ! approximation is already nearly double precision.
  IF (x .LT. -einv+1d-6) THEN
    w2 = wk
    RETURN
  END IF
  
  ! Now apply Fritsch iteration
  wk1  = wk + 1
  zk   = LOG(x/wk) - wk
  qk   = 2 * wk1 * (wk1 + (2d0/3)*zk)
  epsk = zk * (qk - zk) / (wk1 * (qk - 2*zk))
  wk   = wk * (1 + epsk)
  ! In most cases, no further iterations will be necessary
  DO WHILE (ABS(epsk) .GT. 1d-5)
    wk1  = wk + 1
    zk   = LOG(x/wk) - wk
    qk   = 2 * wk1 * (wk1 + (2d0/3)*zk)
    epsk = zk * (qk - zk) / (wk1 * (qk - 2*zk))
    wk   = wk * (1 + epsk)
  END DO
  
  w2 = wk
  
END FUNCTION


!-----------------------------------------------------------------------
! Calculates W(x=e^ln(x)) to double precision, where W is the Lambert W
! function defined as the solution w = W(x) to the equation:
!     x = w e^w
! Returns the principal branch value (the larger of two solutions
! over -1/e < x < 0; there is only one solution for x > 0).  The
! W function is undefined for x < -1/e.
! 
! This version takes ln(x) as the input to allow for cases where x
! is large.
! 
! Valid input:  -\infty < lnx < \infty
! 
ELEMENTAL FUNCTION LAMBERTWLN(lnx) RESULT(w)
  IMPLICIT NONE
  REAL*8 :: w
  REAL*8, INTENT(IN) :: lnx
  REAL*8 :: epsk,zk,qk,wk,wk1,a,b,ia
  REAL*8, PARAMETER :: e    = 2.7182818284590452d0
  REAL*8, PARAMETER :: einv = 0.36787944117144232d0
  
  ! Here, we only calculate W(x) for very large x.  If x is a
  ! not very large, we use the lambertw routine.
  IF (lnx .LT. 300d0) THEN
    w = LAMBERTW(EXP(lnx))
    RETURN
  END IF
  
  ! Could use Newton's or Halley's iteration method to find the
  ! solution, but the Lambert function has a faster method,
  ! Fritsch's iteration:
  !    W_{k+1} = W_k * (1 + eps_k)
  ! with
  !    eps_k   = z_k/(1 + W_k) * (q_k - z_k)/(q_k - 2*z_k)
  !    z_k     = ln(x/W_k) - W_k
  !    q_k     = 2 (1 + W_k) (1 + W_k + (2/3)*z_k)
  ! If eps_k is the error in W_k, then the error in W_{k+1} is of
  ! order (eps_k)^4, a faster convergent that Halley's method.
  ! For a first guess accurate to order O(10^-4), double precision
  ! can be achieved in only a single iteration.  We use estimates
  ! for the initial guess as determined by Veberic.
  ! 
  ! For further information, see:
  !   D. Veberic, arXiv:1003.1628.
  !   F.N. Fritsch, R.E. Shafer and W.P. Crowley,
  !       Commun. ACM 16, 123 (1973).
  
  ! Initial estimate by Veberic
  ! asymptotic expansion
  a = lnx
  b = LOG(a)
  ia = 1/a
  wk = a - b + b * ia *                                                 &
              (1 + ia *                                                 &
              (0.5d0*(-2 + b) + ia *                                    &
              (1/6d0*(6 + b*(-9 + b*2)) + ia *                          &
              (1/12d0*(-12 + b*(36 + b*(-22 + b*3))) + ia *             &
               1/60d0*(60 + b*(-300 + b*(350 + b*(-125 + b*12))))       &
              ))))
  
  ! Now apply Fritsch iteration
  wk1  = wk + 1
  zk   = lnx - LOG(wk) - wk
  qk   = 2 * wk1 * (wk1 + (2d0/3)*zk)
  epsk = zk * (qk - zk) / (wk1 * (qk - 2*zk))
  wk   = wk * (1 + epsk)
  ! In most cases, no further iterations will be necessary
  DO WHILE (ABS(epsk) .GT. 1d-5)
    wk1  = wk + 1
    zk   = lnx - LOG(wk) - wk
    qk   = 2 * wk1 * (wk1 + (2d0/3)*zk)
    epsk = zk * (qk - zk) / (wk1 * (qk - 2*zk))
    wk   = wk * (1 + epsk)
  END DO
  
  w = wk
  
END FUNCTION



!=======================================================================
! PROBABILITY DISTRIBUTIONS
!=======================================================================

!----------UNIFORM DISTRIBUTION-----------------------------------------
! Uniform distribution has probability:
!   P(x|xmin,xmax) = 1/(xmax-xmin)    xmin < x < xmax
!                  = 0                otherwise
! The range of the distribution [xmin,xmax] has default [0,1].
!-----------------------------------------------------------------------

!----------------------------------------
! Probability for uniform distribution
!   x           Value distributed normally
!   xmin        Minimum of uniform values (optional, default is 0)
!   xmax        Maximum of uniform values (optional, default is 1)
ELEMENTAL FUNCTION UniformPDF(x,xmin,xmax)
  IMPLICIT NONE
  REAL*8 :: UniformPDF
  REAL*8, INTENT(IN) :: x
  REAL*8, INTENT(IN), OPTIONAL :: xmin,xmax
  REAL*8 :: xmin0,xmax0
  IF (PRESENT(xmin)) THEN
    xmin0 = xmin
  ELSE
    xmin0 = 0d0
  END IF
  IF (PRESENT(xmax)) THEN
    xmax0 = xmax
  ELSE
    xmax0 = 1d0
  END IF
  IF ((x .GE. xmin0) .AND. (x .LE. xmax0)) THEN
    UniformPDF = 1d0 / (xmax0-xmin0)
  ELSE
    UniformPDF = 0d0
  END IF
END FUNCTION UniformPDF

!----------------------------------------
! Log of probability for uniform distribution
!   x           Value distributed normally
!   xmin        Minimum of uniform values (optional, default is 0)
!   xmax        Maximum of uniform values (optional, default is 1)
ELEMENTAL FUNCTION UniformLogPDF(x,xmin,xmax)
  IMPLICIT NONE
  REAL*8 :: UniformLogPDF
  REAL*8, INTENT(IN) :: x
  REAL*8, INTENT(IN), OPTIONAL :: xmin,xmax
  REAL*8 :: xmin0,xmax0
  IF (PRESENT(xmin)) THEN
    xmin0 = xmin
  ELSE
    xmin0 = 0d0
  END IF
  IF (PRESENT(xmax)) THEN
    xmax0 = xmax
  ELSE
    xmax0 = 1d0
  END IF
  IF ((x .GE. xmin0) .AND. (x .LE. xmax0)) THEN
    UniformLogPDF = LOG(1d0 / (xmax0-xmin0))
  ELSE
    UniformLogPDF = -HUGE(1d0)
  END IF
END FUNCTION UniformLogPDF

!----------------------------------------
! CDF for uniform distribution
!   x           Value distributed normally
!   xmin        Minimum of uniform values (optional, default is 0)
!   xmax        Maximum of uniform values (optional, default is 1)
ELEMENTAL FUNCTION UniformCDF(x,xmin,xmax)
  IMPLICIT NONE
  REAL*8 :: UniformCDF
  REAL*8, INTENT(IN) :: x
  REAL*8, INTENT(IN), OPTIONAL :: xmin,xmax
  REAL*8 :: xmin0,xmax0
  IF (PRESENT(xmin)) THEN
    xmin0 = xmin
  ELSE
    xmin0 = 0d0
  END IF
  IF (PRESENT(xmax)) THEN
    xmax0 = xmax
  ELSE
    xmax0 = 1d0
  END IF
  IF (x .LE. xmin0) THEN
    UniformCDF = 0d0
  ELSE IF (x .GE. xmax0) THEN
    UniformCDF = 1d0
  ELSE
    UniformCDF = (x-xmin0) / (xmax0-xmin0)
  END IF
END FUNCTION UniformCDF

!----------------------------------------
! 1-CDF for uniform distribution
!   x           Value distributed normally
!   xmin        Minimum of uniform values (optional, default is 0)
!   xmax        Maximum of uniform values (optional, default is 1)
ELEMENTAL FUNCTION Uniform1mCDF(x,xmin,xmax)
  IMPLICIT NONE
  REAL*8 :: Uniform1mCDF
  REAL*8, INTENT(IN) :: x
  REAL*8, INTENT(IN), OPTIONAL :: xmin,xmax
  REAL*8 :: xmin0,xmax0
  IF (PRESENT(xmin)) THEN
    xmin0 = xmin
  ELSE
    xmin0 = 0d0
  END IF
  IF (PRESENT(xmax)) THEN
    xmax0 = xmax
  ELSE
    xmax0 = 1d0
  END IF
  IF (x .LE. xmin0) THEN
    Uniform1mCDF = 1d0
  ELSE IF (x .GE. xmax0) THEN
    Uniform1mCDF = 0d0
  ELSE
    Uniform1mCDF = (xmax-x) / (xmax0-xmin0)
  END IF
END FUNCTION Uniform1mCDF

!----------------------------------------
! Random number for uniform distribution
!   xmin        Minimum of uniform values (optional, default is 0)
!   xmax        Maximum of uniform values (optional, default is 1)
FUNCTION RandUniform(xmin,xmax)
  IMPLICIT NONE
  REAL*8 :: RandUniform
  REAL*8, INTENT(IN), OPTIONAL :: xmin,xmax
  REAL*8 :: urand
  urand = rand()
  IF (PRESENT(xmin)) THEN
    IF (PRESENT(xmax)) THEN
      RandUniform = xmin + urand*(xmax-xmin)
    ELSE
      RandUniform = xmin + urand*(1d0-xmin)
    END IF
  ELSE
    IF (PRESENT(xmax)) THEN
      RandUniform = urand*xmax
    ELSE
      RandUniform = urand
    END IF
  END IF
END FUNCTION RandUniform


!----------NORMAL DISTRIBUTION------------------------------------------
! Normal distribution has probability:
!   P(x|mu,sigma) = EXP(-(x-mu)^2/(2 sigma^2)) / SQRT(2 PI sigma^2)
! The average mu has default 0 and the standard deviation sigma
! has default 1.
!-----------------------------------------------------------------------

!----------------------------------------
! Probability for normal distribution
!   x           Value distributed normally
!   mu          Average (optional, default is 0)
!   sigma       Standard deviation (optional, default is 1)
ELEMENTAL FUNCTION NormalPDF(x,mu,sigma)
  IMPLICIT NONE
  REAL*8 :: NormalPDF
  REAL*8, INTENT(IN) :: x
  REAL*8, INTENT(IN), OPTIONAL :: mu,sigma
  REAL*8 :: z
  REAL*8, PARAMETER :: SQRT2PI = 2.5066282746310005d0
  IF (PRESENT(mu)) THEN
    z = x - mu
  ELSE
    z = x
  END IF
  IF (PRESENT(sigma)) THEN
    NormalPDF = EXP(-z**2/(2*sigma**2)) / (SQRT2PI*sigma)
  ELSE
    NormalPDF = EXP(-z**2/2) / SQRT2PI
  END IF
END FUNCTION NormalPDF

!----------------------------------------
! Log of probability for normal distribution
!   x           Value distributed normally
!   mu          Average (optional, default is 0)
!   sigma       Standard deviation (optional, default is 1)
ELEMENTAL FUNCTION NormalLogPDF(x,mu,sigma)
  IMPLICIT NONE
  REAL*8 :: NormalLogPDF
  REAL*8, INTENT(IN) :: x
  REAL*8, INTENT(IN), OPTIONAL :: mu,sigma
  REAL*8 :: z
  REAL*8, PARAMETER :: LOGSQRT2PI = 0.91893853320467274d0
  IF (PRESENT(mu)) THEN
    z = x - mu
  ELSE
    z = x
  END IF
  IF (PRESENT(sigma)) THEN
    NormalLogPDF = -z**2/(2*sigma**2) - LOGSQRT2PI - LOG(sigma)
  ELSE
    NormalLogPDF = -z**2/2 - LOGSQRT2PI
  END IF
END FUNCTION NormalLogPDF

!----------------------------------------
! CDF for normal distribution
!   x           Value distributed normally
!   mu          Average (optional, default is 0)
!   sigma       Standard deviation (optional, default is 1)
ELEMENTAL FUNCTION NormalCDF(x,mu,sigma)
  IMPLICIT NONE
  REAL*8 :: NormalCDF
  REAL*8, INTENT(IN) :: x
  REAL*8, INTENT(IN), OPTIONAL :: mu,sigma
  REAL*8 :: z
  REAL*8, PARAMETER :: SQRT2 = 1.4142135623730950d0
  IF (PRESENT(mu)) THEN
    z = x - mu
  ELSE
    z = x
  END IF
  IF (PRESENT(sigma)) THEN
    z = z / (SQRT2*sigma)
  ELSE
    z = z / SQRT2
  END IF
  NormalCDF = 0.5d0 * ERFC(-z)
END FUNCTION NormalCDF

!----------------------------------------
! 1-CDF for normal distribution
!   x           Value distributed normally
!   mu          Average (optional, default is 0)
!   sigma       Standard deviation (optional, default is 1)
ELEMENTAL FUNCTION Normal1mCDF(x,mu,sigma)
  IMPLICIT NONE
  REAL*8 :: Normal1mCDF
  REAL*8, INTENT(IN) :: x
  REAL*8, INTENT(IN), OPTIONAL :: mu,sigma
  REAL*8 :: z
  REAL*8, PARAMETER :: SQRT2 = 1.4142135623730950d0
  IF (PRESENT(mu)) THEN
    z = x - mu
  ELSE
    z = x
  END IF
  IF (PRESENT(sigma)) THEN
    z = z / (SQRT2*sigma)
  ELSE
    z = z / SQRT2
  END IF
  Normal1mCDF = 0.5d0 * ERFC(z)
END FUNCTION Normal1mCDF

!----------------------------------------
! Random number for normal distribution
!   mu          Average (optional, default is 0)
!   sigma       Standard deviation (optional, default is 1)
FUNCTION RandNormal(mu,sigma)
  IMPLICIT NONE
  REAL*8 :: RandNormal
  REAL*8, INTENT(IN), OPTIONAL :: mu,sigma
  REAL*8 :: u,v,r2,w,urand(2),rand1
  REAL*8, SAVE :: rand2
  LOGICAL, SAVE :: needs_calc = .TRUE.
  ! Algorithm calculates two random normal numbers at a time.
  ! Store second for next call to this function.
  IF (needs_calc) THEN
    r2 = 1d0
    DO WHILE (r2 .GE. 1d0)
      urand(1) = rand()
      urand(2) = rand()
      u = 2d0*urand(1) - 1d0
      v = 2d0*urand(2) - 1d0
      r2 = u**2 + v**2
    END DO
    w = SQRT(-2d0 * LOG(r2) / r2)
    rand1 = u * w
    rand2 = v * w
    RandNormal  = rand1
    needs_calc = .FALSE.
  ELSE
    RandNormal = rand2
    needs_calc = .TRUE.
  END IF
  IF (PRESENT(sigma)) RandNormal = sigma*RandNormal
  IF (PRESENT(mu)) RandNormal = mu + RandNormal
END FUNCTION RandNormal


!----------LOG NORMAL DISTRIBUTION--------------------------------------
! Normal distribution has probability:
!   P(x|mu,sigma) = EXP(-(ln(x)-mu)^2/(2 sigma^2)) / SQRT(2 PI sigma^2) / x
! The location mu has default 0 and the scale sigma has default 1.
! Note the parameters mu and sigma are not the mean and standard
! deviation.
!   mean:      muL      = exp(mu + sigma^2/2)
!   variance:  sigmaL^2 = (exp(sigma^2)-1) exp(2*mu+sigma^2)
!       =>     mu      = -1/2 log[(1+sigmaL^2/muL^2)/muL^2]
!              sigma^2 = log[1+sigmaL^2/muL^2]
! A normal distribution with parameters sigmaN << muN is asymptotically
! similar to a log normal distribution with parameters:
!   mu    = ln(muN)
!   sigma = sigmaN/muN
! Specifically, setting muL = muN and sigmaL = sigmaN:
!   mu      = log(muN) - 1/2 log(1+sigmaN^2/muN^2)
!           = log(muN) + O[sigmaN^2/muN^2]
!   sigma^2 = log(1+sigmaN^2/muN^2)
!           = sigmaN^2/muN^2 (1 + O[sigmaN^2/muN^2])
!-----------------------------------------------------------------------

!----------------------------------------
! Probability for log normal distribution
!   x           Value distributed log-normally
!   mu          Location (optional, default is 0)
!   sigma       Scale (optional, default is 1)
ELEMENTAL FUNCTION LogNormalPDF(x,mu,sigma)
  IMPLICIT NONE
  REAL*8 :: LogNormalPDF
  REAL*8, INTENT(IN) :: x
  REAL*8, INTENT(IN), OPTIONAL :: mu,sigma
  REAL*8 :: z
  REAL*8, PARAMETER :: SQRT2PI = 2.5066282746310005d0
  ! Bad case
  IF (x .LE. 0d0) THEN
    LogNormalPDF = -HUGE(1d0)
    RETURN
  END IF
  IF (PRESENT(mu)) THEN
    z = LOG(x) - mu
  ELSE
    z = LOG(x)
  END IF
  IF (PRESENT(sigma)) THEN
    LogNormalPDF = EXP(-z**2/(2*sigma**2)) / (SQRT2PI*sigma*x)
  ELSE
    LogNormalPDF = EXP(-z**2/2) / (SQRT2PI*x)
  END IF
END FUNCTION LogNormalPDF

!----------------------------------------
! Log of probability for log normal distribution
!   x           Value distributed normally
!   mu          Average (optional, default is 0)
!   sigma       Standard deviation (optional, default is 1)
ELEMENTAL FUNCTION LogNormalLogPDF(x,mu,sigma)
  IMPLICIT NONE
  REAL*8 :: LogNormalLogPDF
  REAL*8, INTENT(IN) :: x
  REAL*8, INTENT(IN), OPTIONAL :: mu,sigma
  REAL*8 :: z
  REAL*8, PARAMETER :: LOGSQRT2PI = 0.91893853320467274d0
  ! Bad case
  IF (x .LE. 0d0) THEN
    LogNormalLogPDF = -HUGE(1d0)
    RETURN
  END IF
  IF (PRESENT(mu)) THEN
    z = LOG(x) - mu
  ELSE
    z = LOG(x)
  END IF
  IF (PRESENT(sigma)) THEN
    LogNormalLogPDF = -z**2/(2*sigma**2) - LOGSQRT2PI - LOG(x*sigma)
  ELSE
    LogNormalLogPDF = -z**2/2 - LOGSQRT2PI - LOG(x)
  END IF
END FUNCTION LogNormalLogPDF

!----------------------------------------
! CDF for log normal distribution
!   x           Value distributed log-normally
!   mu          Location (optional, default is 0)
!   sigma       Scale (optional, default is 1)
ELEMENTAL FUNCTION LogNormalCDF(x,mu,sigma)
  IMPLICIT NONE
  REAL*8 :: LogNormalCDF
  REAL*8, INTENT(IN) :: x
  REAL*8, INTENT(IN), OPTIONAL :: mu,sigma
  REAL*8 :: z
  REAL*8, PARAMETER :: SQRT2 = 1.4142135623730950d0
  ! Bad case
  IF (x .LE. 0d0) THEN
    LogNormalCDF = -HUGE(1d0)
    RETURN
  END IF
  IF (PRESENT(mu)) THEN
    z = LOG(x) - mu
  ELSE
    z = LOG(x)
  END IF
  IF (PRESENT(sigma)) THEN
    z = LOG(z) / (SQRT2*sigma)
  ELSE
    z = LOG(z) / SQRT2
  END IF
  LogNormalCDF = 0.5d0 * ERFC(-z)
END FUNCTION LogNormalCDF

!----------------------------------------
! 1-CDF for log normal distribution
!   x           Value distributed log-normally
!   mu          Location (optional, default is 0)
!   sigma       Scale (optional, default is 1)
ELEMENTAL FUNCTION LogNormal1mCDF(x,mu,sigma)
  IMPLICIT NONE
  REAL*8 :: LogNormal1mCDF
  REAL*8, INTENT(IN) :: x
  REAL*8, INTENT(IN), OPTIONAL :: mu,sigma
  REAL*8 :: z
  REAL*8, PARAMETER :: SQRT2 = 1.4142135623730950d0
  ! Bad case
  IF (x .LE. 0d0) THEN
    LogNormal1mCDF = -HUGE(1d0)
    RETURN
  END IF
  IF (PRESENT(mu)) THEN
    z = LOG(x) - mu
  ELSE
    z = LOG(x)
  END IF
  IF (PRESENT(sigma)) THEN
    z = LOG(z) / (SQRT2*sigma)
  ELSE
    z = LOG(z) / SQRT2
  END IF
  LogNormal1mCDF = 0.5d0 * ERFC(z)
END FUNCTION LogNormal1mCDF

!----------------------------------------
! Random number for log normal distribution
!   mu          Location (optional, default is 0)
!   sigma       Scale (optional, default is 1)
FUNCTION RandLogNormal(mu,sigma)
  IMPLICIT NONE
  REAL*8 :: RandLogNormal
  REAL*8, INTENT(IN), OPTIONAL :: mu,sigma
  REAL*8 :: x
  x = RandNormal()
  IF (PRESENT(sigma)) x = sigma*x
  IF (PRESENT(mu)) x = mu + x
  RandLogNormal = EXP(x)
END FUNCTION RandLogNormal


!----------EXPONENTIAL DISTRIBUTION-------------------------------------
! Exponential distribution has probability:
!   P(x|xmin,xmax,xs) = A EXP(-x/xs)    xmin < x < xmax
!                     = 0               otherwise
! where the constant 'A' is a normalization factor.
! The range of the distribution [xmin,xmax] has default [0,infinity].
! The scale of the exponential xs has default 1.
!-----------------------------------------------------------------------

!----------------------------------------
! Probability for exponential distribution
!   x           Value distributed exponentially
!   xmin        Minimum value (optional, default is 0)
!   xmax        Maximum value (optional, default is infinity)
!   xs          The exponential scale (optional, default is 1)
ELEMENTAL FUNCTION ExponentialPDF(x,xmin,xmax,xs)
  IMPLICIT NONE
  REAL*8 :: ExponentialPDF
  REAL*8, INTENT(IN) :: x
  REAL*8, INTENT(IN), OPTIONAL :: xmin,xmax,xs
  REAL*8 :: xmin0,xmax0
  IF (PRESENT(xmin)) THEN
    xmin0 = xmin
  ELSE
    xmin0 = 0d0
  END IF
  IF (PRESENT(xmax)) THEN
    xmax0 = xmax
  ELSE
    xmax0 = HUGE(1d0)
  END IF
  IF ((x .LT. xmin0) .OR. (x .GT. xmax0)) THEN
    ExponentialPDF = 0d0
    RETURN
  END IF
  IF (PRESENT(xs)) THEN
    IF (xs .EQ. 0d0) THEN
      ExponentialPDF = 1d0 / (xmax0-xmin0)
    ELSE
      ExponentialPDF = EXP((xmin0-x)/xs)    &
                       / (1d0 - EXP((xmin0-xmax0)/xs))    &
                       / xs
    END IF
  ELSE
    ExponentialPDF = EXP(xmin0-x) / (1d0 - EXP(xmin0-xmax0))
  END IF
END FUNCTION ExponentialPDF

!----------------------------------------
! Log of probability for exponential distribution
!   x           Value distributed exponentially
!   xmin        Minimum value (optional, default is 0)
!   xmax        Maximum value (optional, default is infinity)
!   xs          The exponential scale (optional, default is 1)
ELEMENTAL FUNCTION ExponentialLogPDF(x,xmin,xmax,xs)
  IMPLICIT NONE
  REAL*8 :: ExponentialLogPDF
  REAL*8, INTENT(IN) :: x
  REAL*8, INTENT(IN), OPTIONAL :: xmin,xmax,xs
  REAL*8 :: xmin0,xmax0
  IF (PRESENT(xmin)) THEN
    xmin0 = xmin
  ELSE
    xmin0 = 0d0
  END IF
  IF (PRESENT(xmax)) THEN
    xmax0 = xmax
  ELSE
    xmax0 = HUGE(1d0)
  END IF
  IF ((x .LT. xmin0) .OR. (x .GT. xmax0)) THEN
    ExponentialLogPDF = -HUGE(1d0)
    RETURN
  END IF
  IF (PRESENT(xs)) THEN
    IF (xs .EQ. 0d0) THEN
      ExponentialLogPDF = LOG(1d0 / (xmax0-xmin0))
    ELSE
      ExponentialLogPDF = EXP((xmin0-x)/xs)    &
                          - LOG(1d0 - EXP((xmin0-xmax0)/xs))    &
                          - LOG(xs)
    END IF
  ELSE
    ExponentialLogPDF = (xmin0-x) - LOG(1d0 - EXP(xmin0-xmax0))
  END IF
END FUNCTION ExponentialLogPDF

!----------------------------------------
! CDF for exponential distribution
!   x           Value distributed exponentially
!   xmin        Minimum value (optional, default is 0)
!   xmax        Maximum value (optional, default is infinity)
!   xs          The exponential scale (optional, default is 1)
ELEMENTAL FUNCTION ExponentialCDF(x,xmin,xmax,xs)
  IMPLICIT NONE
  REAL*8 :: ExponentialCDF
  REAL*8, INTENT(IN) :: x
  REAL*8, INTENT(IN), OPTIONAL :: xmin,xmax,xs
  REAL*8 :: xmin0,xmax0
  IF (PRESENT(xmin)) THEN
    xmin0 = xmin
  ELSE
    xmin0 = 0d0
  END IF
  IF (PRESENT(xmax)) THEN
    xmax0 = xmax
  ELSE
    xmax0 = HUGE(1d0)
  END IF
  IF (x .LE. xmin0) THEN
    ExponentialCDF = 0d0
  ELSE IF (x .GE. xmax0) THEN
    ExponentialCDF = 1d0
  ELSE
    IF (PRESENT(xs)) THEN
      IF (xs .EQ. 0d0) THEN
        ExponentialCDF = (x-xmin0) / (xmax0-xmin0)
      ELSE
        !ExponentialCDF = (1d0 - EXP((xmin0-x)/xs))    &
        !                 / (1d0 - EXP((xmin0-xmax0)/xs))
        ExponentialCDF = EXPm1((xmin0-x)/xs) / EXPm1((xmin0-xmax0)/xs)
      END IF
    ELSE
      !ExponentialCDF = (1d0 - EXP(xmin0-x)) / (1d0 - EXP(xmin0-xmax0))
      ExponentialCDF = EXPm1(xmin0-x) / EXPm1(xmin0-xmax0)
    END IF
  END IF
END FUNCTION ExponentialCDF

!----------------------------------------
! 1-CDF for exponential distribution
!   x           Value distributed exponentially
!   xmin        Minimum value (optional, default is 0)
!   xmax        Maximum value (optional, default is infinity)
!   xs          The exponential scale (optional, default is 1)
ELEMENTAL FUNCTION Exponential1mCDF(x,xmin,xmax,xs)
  IMPLICIT NONE
  REAL*8 :: Exponential1mCDF
  REAL*8, INTENT(IN) :: x
  REAL*8, INTENT(IN), OPTIONAL :: xmin,xmax,xs
  REAL*8 :: xmin0,xmax0
  IF (PRESENT(xmin)) THEN
    xmin0 = xmin
  ELSE
    xmin0 = 0d0
  END IF
  IF (PRESENT(xmax)) THEN
    xmax0 = xmax
  ELSE
    xmax0 = HUGE(1d0)
  END IF
  IF (x .LE. xmin0) THEN
    Exponential1mCDF = 1d0
  ELSE IF (x .GE. xmax0) THEN
    Exponential1mCDF = 0d0
  ELSE
    IF (PRESENT(xs)) THEN
      IF (xs .EQ. 0d0) THEN
        Exponential1mCDF = (xmax0-x) / (xmax0-xmin0)
      ELSE
        !Exponential1mCDF = (EXP((xmax0-x)/xs) - 1d0)    &
        !                 / (EXP((xmax0-xmin0)/xs) - 1d0)
        Exponential1mCDF = EXPm1((xmax0-x)/xs) / EXPm1((xmax0-xmin0)/xs)
      END IF
    ELSE
      !Exponential1mCDF = (EXP(xmax0-x) - 1d0) / (EXP(xmax0-xmin0) - 1d0)
      Exponential1mCDF = EXPm1(xmax0-x) / EXPm1(xmax0-xmin0)
    END IF
  END IF
END FUNCTION Exponential1mCDF

!----------------------------------------
! Random number for exponential distribution
!   xmin        Minimum value (optional, default is 0)
!   xmax        Maximum value (optional, default is infinity)
!   xs          The exponential scale (optional, default is 1)
FUNCTION RandExponential(xmin,xmax,xs)
  IMPLICIT NONE
  REAL*8 :: RandExponential
  REAL*8, INTENT(IN), OPTIONAL :: xmin,xmax,xs
  REAL*8 :: xmin0,xmax0
  REAL*8 :: urand
  urand = rand()
  IF (PRESENT(xmin)) THEN
    xmin0 = xmin
  ELSE
    xmin0 = 0d0
  END IF
  IF (PRESENT(xmax)) THEN
    xmax0 = xmax
  ELSE
    xmax0 = HUGE(1d0)
  END IF
  IF (PRESENT(xs)) THEN
    IF (xs .EQ. 0d0) THEN
      RandExponential = xmin0 + urand*(xmax0-xmin0)
    ELSE
      RandExponential = xmin0 - xs*LOG(1d0 - urand*(1d0-EXP((xmin0-xmax0)/xs)))
    END IF
  ELSE
    RandExponential = xmin0 - LOG(1d0 - urand*(1d0-EXP(xmin0-xmax0)))
  END IF
END FUNCTION RandExponential


!----------CHI-SQUARE DISTRIBUTION--------------------------------------
! Chi-square distribution has probability:
!   P(x|k) = x^(k/2 - 1) EXP(-x/2) / 2^(k/2) / GAMMA(k/2)
!-----------------------------------------------------------------------

!----------------------------------------
! Probability for chi-square distribution
!   x           Value chi-square distributed
!   k           Degrees of freedom
ELEMENTAL FUNCTION ChiSquarePDF(x,k)
  IMPLICIT NONE
  REAL*8 :: ChiSquarePDF
  REAL*8, INTENT(IN) :: x
  INTEGER, INTENT(IN) :: k
  IF (x .GE. 0d0) THEN
    ChiSquarePDF = x**(0.5d0*k - 1) * EXP(-0.5d0*x)                     &
                   / ( 2**(0.5d0*k) * GAMMA(0.5d0*k) )
  ELSE
    ChiSquarePDF = 0d0
  END IF
END FUNCTION ChiSquarePDF

!----------------------------------------
! Log of probability for chi-square distribution
!   x           Value chi-square distributed
!   k           Degrees of freedom
ELEMENTAL FUNCTION ChiSquareLogPDF(x,k)
  IMPLICIT NONE
  REAL*8 :: ChiSquareLogPDF
  REAL*8, INTENT(IN) :: x
  INTEGER, INTENT(IN) :: k
  REAL*8, PARAMETER :: LOG2 = 0.69314718055994531d0
  IF (x .GE. 0d0) THEN
    ChiSquareLogPDF = (0.5d0*k - 1)*LOG(x) - 0.5d0*x                    &
                      - (0.5d0*k)*LOG2 - LOG_GAMMA(0.5d0*k)
  ELSE
    ChiSquareLogPDF = 0d0
  END IF
END FUNCTION ChiSquareLogPDF

!----------------------------------------
! CDF for chi-square distribution
!   x           Value chi-square distributed
!   k           Degrees of freedom
FUNCTION ChiSquareCDF(x,k)
!ELEMENTAL FUNCTION ChiSquareCDF(x,k)
  IMPLICIT NONE
  REAL*8 :: ChiSquareCDF
  REAL*8, INTENT(IN) :: x
  INTEGER, INTENT(IN) :: k
  IF (x .GE. 0d0) THEN
    ! Regularized gamma function P(k/2,x/2)
    !ChiSquareCDF = LIGAMMA(0.5d0*k,0.5d0*x) / GAMMA(0.5d0*k)
  ELSE
    ChiSquareCDF = 0d0
  END IF
  WRITE(*,*) 'ERROR: ChiSquareCDF not implemented (LIGAMMA)'
  STOP
END FUNCTION ChiSquareCDF

!----------------------------------------
! 1-CDF for chi-square distribution
!   x           Value chi-square distributed
!   k           Degrees of freedom
FUNCTION ChiSquare1mCDF(x,k)
!ELEMENTAL FUNCTION ChiSquare1mCDF(x,k)
  IMPLICIT NONE
  REAL*8 :: ChiSquare1mCDF
  REAL*8, INTENT(IN) :: x
  INTEGER, INTENT(IN) :: k
  IF (x .GE. 0d0) THEN
    ! Regularized gamma function P(k/2,x/2)
    !ChiSquare1mCDF = UIGAMMA(0.5d0*k,0.5d0*x) / GAMMA(0.5d0*k)
  ELSE
    ChiSquare1mCDF = 0d0
  END IF
  WRITE(*,*) 'ERROR: ChiSquare1mCDF not implemented (UIGAMMA)'
  STOP
END FUNCTION ChiSquare1mCDF

!----------------------------------------
! Random number for normal distribution
!   k           Degrees of freedom
FUNCTION RandChiSquare(k)
  IMPLICIT NONE
  REAL*8 :: RandChiSquare
  INTEGER, INTENT(IN) :: k
  REAL*8 :: x
  x = rand()
  !RandChiSquare = REDUCEDGAMMAINV(0.5d0*k,x)
  RandChiSquare = -1d0
  WRITE(*,*) 'ERROR: RandChiSquare not implemented (REDUCEDGAMMAINV)'
  STOP
END FUNCTION RandChiSquare


!----------POISSON DISTRIBUTION-----------------------------------------
! Poisson distribution has probability:
!   P(N|mu) = EXP(-mu) mu^N / N!
!-----------------------------------------------------------------------

!----------------------------------------
! Probability for Poisson distribution
!   N           Number Poisson distributed
!   mu          Average
ELEMENTAL FUNCTION PoissonPDF(N,mu)
  IMPLICIT NONE
  REAL*8 :: PoissonPDF
  INTEGER, INTENT(IN) :: N
  REAL*8, INTENT(IN) :: mu
  REAL*8, PARAMETER :: MUMAX = 100.0
  REAL*8, PARAMETER :: SQRT2PI = 2.5066282746310005d0
  INTEGER :: I
  ! Bad cases
  IF ((N .LT. 0) .OR. (mu .LT. 0d0)) THEN
    PoissonPDF = -HUGE(1d0)
    RETURN
  END IF
  ! Catch special case mu = 0
  IF (mu .EQ. 0.0) THEN
    IF (N .EQ. 0) THEN
      PoissonPDF = 1d0
    ELSE
      PoissonPDF = 0d0
    END IF
    RETURN
  END IF
  ! Large mu limit: normal distribution with mean mu and
  ! variance mu
  IF (mu .GT. MUMAX) THEN
    PoissonPDF = NormalPDF(1d0*N,mu,SQRT(mu))
    RETURN
  END IF
  ! General case
  !PoissonPDF = EXP(-mu)
  !DO I = 1,N
  !  PoissonPDF = PoissonPDF * mu / I
  !END DO
  PoissonPDF = EXP(-mu) * mu**N / EXP(LOG_GAMMA(N+1d0))
END FUNCTION PoissonPDF

!----------------------------------------
! Log of probability for Poisson distribution
!   N           Number Poisson distributed
!   mu          Average
ELEMENTAL FUNCTION PoissonLogPDF(N,mu)
  IMPLICIT NONE
  REAL*8 :: PoissonLogPDF
  INTEGER, INTENT(IN) :: N
  REAL*8, INTENT(IN) :: mu
  REAL*8, PARAMETER :: MUMAX = 100.0
  REAL*8, PARAMETER :: SQRT2PI = 2.5066282746310005d0
  INTEGER :: I
  REAL*8 :: fact
  ! Bad cases
  IF ((N .LT. 0) .OR. (mu .LT. 0d0)) THEN
    PoissonLogPDF = -HUGE(1d0)
    RETURN
  END IF
  ! Catch special case mu = 0
  IF (mu .EQ. 0.0) THEN
    IF (N .EQ. 0) THEN
      PoissonLogPDF = 0d0
    ELSE
      PoissonLogPDF = -HUGE(1d0)
    END IF
    RETURN
  END IF
  ! Large mu limit: normal distribution with mean mu and
  ! variance mu
  IF (mu .GT. MUMAX) THEN
    PoissonLogPDF = NormalLogPDF(1d0*N,mu,SQRT(mu))
    RETURN
  END IF
  ! General case
  !fact = 1d0
  !DO I = 1,N
  !  fact = fact*I
  !END DO
  !PoissonLogPDF = -mu + N*LOG(mu) - LOG(fact)
  PoissonLogPDF = -mu + N*LOG(mu) - LOG_GAMMA(N+1d0)
END FUNCTION PoissonLogPDF

!----------------------------------------
! CDF for Poisson distribution
!   N           Number Poisson distributed
!   mu          Average
ELEMENTAL FUNCTION PoissonCDF(N,mu)
  IMPLICIT NONE
  REAL*8 :: PoissonCDF
  INTEGER, INTENT(IN) :: N
  REAL*8, INTENT(IN) :: mu
  REAL*8, PARAMETER :: MUMAX = 100.0
  REAL*8, PARAMETER :: SQRT2PI = 2.5066282746310005d0
  REAL*8 :: pdf
  INTEGER :: I
  ! Bad cases
  IF ((N .LT. 0) .OR. (mu .LT. 0d0)) THEN
    PoissonCDF = -HUGE(1d0)
    RETURN
  END IF
  ! Catch special case mu = 0
  IF (mu .EQ. 0.0) THEN
    PoissonCDF = 1d0
    RETURN
  END IF
  ! Large mu limit: normal distribution with mean mu and
  ! variance mu
  ! Continuity correction applied
  IF (mu .GT. MUMAX) THEN
    PoissonCDF = NormalCDF(1d0*N+0.5d0,mu,SQRT(mu))
    RETURN
  END IF
  ! General case
  ! Note cdf = RegularizedGamma[N+1,mu]
  pdf = EXP(-mu)
  PoissonCDF = pdf
  DO I = 1,N
    pdf = pdf * mu / I
    PoissonCDF = PoissonCDF + pdf
  END DO
END FUNCTION PoissonCDF

!----------------------------------------
! 1-CDF for Poisson distribution
!   N           Number Poisson distributed
!   mu          Average
ELEMENTAL FUNCTION Poisson1mCDF(N,mu)
  IMPLICIT NONE
  REAL*8 :: Poisson1mCDF
  INTEGER, INTENT(IN) :: N
  REAL*8, INTENT(IN) :: mu
  REAL*8, PARAMETER :: MUMAX = 100.0
  REAL*8, PARAMETER :: SQRT2PI = 2.5066282746310005d0
  REAL*8 :: pdf
  INTEGER :: I
  ! Bad cases
  IF ((N .LT. 0) .OR. (mu .LT. 0d0)) THEN
    Poisson1mCDF = -HUGE(1d0)
    RETURN
  END IF
  ! Catch special case mu = 0
  IF (mu .EQ. 0.0) THEN
    Poisson1mCDF = 0d0
    RETURN
  END IF
  ! Large mu limit: normal distribution with mean mu and
  ! variance mu
  ! Continuity correction applied
  IF (mu .GT. MUMAX) THEN
    Poisson1mCDF = Normal1mCDF(1d0*N+0.5d0,mu,SQRT(mu))
    RETURN
  END IF
  ! General case
  ! Note cdf = RegularizedGamma[N+1,mu]
  pdf = EXP(-mu)
  Poisson1mCDF = 0
  DO WHILE (pdf .GE. EPSILON(pdf)*Poisson1mCDF)
    pdf = pdf * mu / I
    Poisson1mCDF = Poisson1mCDF + pdf
  END DO
END FUNCTION Poisson1mCDF

!----------------------------------------
! Random number for Poisson distribution
!   mu          Average
FUNCTION RandPoisson(mu)
  IMPLICIT NONE
  INTEGER :: RandPoisson
  REAL*8, INTENT(IN) :: mu
  REAL*8 x,xsum,y
  REAL*8, PARAMETER :: MUMAX = 100.0
  INTEGER :: I
  ! Large mu limit: normal distribution with mean mu and
  ! variance mu (continuity correction applied)
  IF (mu .GT. MUMAX) THEN
    !x = mu + SQRT(2*mu) * INVERF(2*y-1)
    x = mu + SQRT(mu) * RandNormal()
    RandPoisson = NINT(x)
    RETURN
  END IF
  ! General case
  y = rand()
  I = 0
  x = EXP(-mu)
  xsum = x
  DO WHILE (xsum .LT. y)
    I = I+1
    x = mu * x / I
    xsum = xsum + x
    ! Bad case: start over (can occur for y extremely close to 1)
    IF (I .GT. 2*MUMAX) THEN
      y = rand()
      I = 0
      x = EXP(-mu)
      xsum = x
    END IF
  END DO
  RandPoisson = I
END FUNCTION RandPoisson


!----------BINOMIAL DISTRIBUTION----------------------------------------
! Binomial distribution has probability:
!   P(k|p,N) = [N!/(k! (N-k)!)] p^k (1-p)^(N-k)
!-----------------------------------------------------------------------

!----------------------------------------
! Probability for binomial distribution
!   k           Number of successful trials
!   p           Probability of success for each trial
!   N           Number of trials
ELEMENTAL FUNCTION BinomialPDF(k,p,N)
  IMPLICIT NONE
  REAL*8 :: BinomialPDF
  INTEGER, INTENT(IN) :: k,N
  REAL*8, INTENT(IN) :: p
  INTEGER, PARAMETER :: NMAX = 100
  REAL*8, PARAMETER :: MUMAX = 20.0
  REAL*8 :: p0,q0,mu,pq
  INTEGER :: k0,I
  
  ! Bad cases
  IF ((k .LT. 0) .OR. (k .GT. N)) THEN
    BinomialPDF = -HUGE(1d0)
    RETURN
  END IF
  
  ! Small N case
  IF (N .LE. NMAX) THEN
    ! Can write CDF(k|p,N) = CDF(N-k|1-p,N) = CDF(k0,p0,N)
    ! Choose smaller of k or N-k to work with
    IF (k .GT. N-k) THEN
      p0 = 1-p
      q0 = p
      k0 = N-k
    ELSE
      p0 = p
      q0 = 1-p
      k0 = k
    END IF
    pq = p0*q0
    BinomialPDF = 1d0
    DO I=1,k0
      BinomialPDF = BinomialPDF * (N-k0+I)*pq/I
    END DO
    BinomialPDF = BinomialPDF * q0**(N-2*k0)
  ! Large N case
  ELSE
    ! Can write P(k|p,N) = P(N-k|1-p,N) = P(k0,p0,N)
    ! Choose smaller of p or 1-p to work with
    IF (p .GT. 0.5d0) THEN
      p0 = 1-p
      q0 = p
      k0 = N-k
    ELSE
      p0 = p
      q0 = 1-p
      k0 = k
    END IF
    mu = N*p0
    ! Large N, small N*p case:
    ! Poisson distribution of average mu = N*p
    IF (mu .LE. MUMAX) THEN
      BinomialPDF = PoissonPDF(k0,mu)
    ! Large N, non-small N*p case:
    ! Normal distribution of average mu = N*p
    ! and variance N*p*(1-p)
    ELSE
      BinomialPDF = NormalPDF(1d0*k0,mu,SQRT(N*p0*q0))
    END IF
  END IF
END FUNCTION BinomialPDF

!----------------------------------------
! Probability for binomial distribution
!   k           Number of successful trials
!   p           Probability of success for each trial
!   N           Number of trials
ELEMENTAL FUNCTION BinomialLogPDF(k,p,N)
  IMPLICIT NONE
  REAL*8 :: BinomialLogPDF
  INTEGER, INTENT(IN) :: k,N
  REAL*8, INTENT(IN) :: p
  INTEGER, PARAMETER :: NMAX = 100
  REAL*8, PARAMETER :: MUMAX = 20.0
  REAL*8 :: p0,q0,mu,pq,x
  INTEGER :: k0,I
  
  ! Bad cases
  IF ((k .LT. 0) .OR. (k .GT. N)) THEN
    BinomialLogPDF = -HUGE(1d0)
    RETURN
  END IF
  
  ! Small N case
  IF (N .LE. NMAX) THEN
    ! Can write CDF(k|p,N) = CDF(N-k|1-p,N) = CDF(k0,p0,N)
    ! Choose smaller of k or N-k to work with
    IF (k .GT. N-k) THEN
      p0 = 1-p
      q0 = p
      k0 = N-k
    ELSE
      p0 = p
      q0 = 1-p
      k0 = k
    END IF
    pq = p0*q0
    x = 1d0
    DO I=1,k0
      x = x * (N-k0+I)*pq/I
    END DO
    x = x * q0**(N-2*k0)
    BinomialLogPDF = LOG(x)
  ! Large N case
  ELSE
    ! Can write P(k|p,N) = P(N-k|1-p,N) = P(k0,p0,N)
    ! Choose smaller of p or 1-p to work with
    IF (p .GT. 0.5d0) THEN
      p0 = 1-p
      q0 = p
      k0 = N-k
    ELSE
      p0 = p
      q0 = 1-p
      k0 = k
    END IF
    mu = N*p0
    ! Large N, small N*p case:
    ! Poisson distribution of average mu = N*p
    IF (mu .LE. MUMAX) THEN
      BinomialLogPDF = PoissonLogPDF(k0,mu)
    ! Large N, non-small N*p case:
    ! Normal distribution of average mu = N*p
    ! and variance N*p*(1-p)
    ELSE
      BinomialLogPDF = NormalLogPDF(1d0*k0,mu,SQRT(N*p0*q0))
    END IF
  END IF
END FUNCTION BinomialLogPDF

!----------------------------------------
! CDF for binomial distribution
!   k           Number of successful trials
!   p           Probability of success for each trial
!   N           Number of trials
ELEMENTAL FUNCTION BinomialCDF(k,p,N)
  IMPLICIT NONE
  REAL*8 :: BinomialCDF
  INTEGER, INTENT(IN) :: k,N
  REAL*8, INTENT(IN) :: p
  INTEGER, PARAMETER :: NMAX = 100
  REAL*8, PARAMETER :: MUMAX = 20.0
  REAL*8 :: p0,q0,mu,xI
  INTEGER :: k0,I

  ! Bad cases
  IF (k .LT. 0) THEN
    BinomialCDF = -HUGE(1d0)
    RETURN
  ELSE IF (k .GT. N) THEN
    BinomialCDF = HUGE(1d0)
    RETURN
  END IF
  
  ! Note cdf = BetaRegularized[1-p,N-k,k+1]
  
  ! Can write P(k|p,N) = P(N-k|1-p,N) = P(k0,p0,N)
  ! Choose smaller of p or 1-p to work with
  IF (p .GT. 0.5d0) THEN
    p0 = 1-p
    q0 = p
    k0 = N-k
  ELSE
    p0 = p
    q0 = 1-p
    k0 = k
  END IF
  
  ! Small N case
  IF (N .LE. NMAX) THEN
    xI = (1-p0)**N
    BinomialCDF = xI
    DO I=1,k0
      xI = xI * ((N-I+1)*p0) / (I*q0)
      BinomialCDF = BinomialCDF + xI
    END DO
  ! Large N case
  ELSE
    mu = N*p0
    ! Large N, small N*p case:
    ! Poisson distribution of average mu = N*p
    IF (mu .LE. MUMAX) THEN
      BinomialCDF = PoissonCDF(k0,mu)
    ! Large N, non-small N*p case:
    ! Normal distribution of average mu = N*p
    ! and variance N*p*(1-p)
    ! Continuity correction applied
    ELSE
      BinomialCDF = NormalCDF(1d0*k0+0.5d0,mu,SQRT(N*p0*q0))
    END IF
  END IF
END FUNCTION BinomialCDF

!----------------------------------------
! 1-CDF for binomial distribution
!   k           Number of successful trials
!   p           Probability of success for each trial
!   N           Number of trials
ELEMENTAL FUNCTION Binomial1mCDF(k,p,N)
  IMPLICIT NONE
  REAL*8 :: Binomial1mCDF
  INTEGER, INTENT(IN) :: k,N
  REAL*8, INTENT(IN) :: p
  INTEGER, PARAMETER :: NMAX = 100
  REAL*8, PARAMETER :: MUMAX = 20.0
  REAL*8 :: p0,q0,mu,xI
  INTEGER :: k0,I

  ! Bad cases
  IF (k .LT. 0) THEN
    Binomial1mCDF = HUGE(1d0)
    RETURN
  ELSE IF (k .GT. N) THEN
    Binomial1mCDF = -HUGE(1d0)
    RETURN
  END IF
  
  ! Note 1-cdf = BetaRegularized[p,k+1,N-k]
  
  ! Can write P(k|p,N) = P(N-k|1-p,N) = P(k0,p0,N)
  ! Choose smaller of p or 1-p to work with
  IF (p .GT. 0.5d0) THEN
    p0 = 1-p
    q0 = p
    k0 = N-k
  ELSE
    p0 = p
    q0 = 1-p
    k0 = k
  END IF
  
  ! Small N case
  IF (N .LE. NMAX) THEN
    xI = (1-p0)**N
    Binomial1mCDF = xI
    xI = EXP(LOG_GAMMA(N+1d0)-LOG_GAMMA(1d0*k0)-LOG_GAMMA(N-1d0*k0)) * p0**k0 * q0**(N-k0)
    Binomial1mCDF = 0
    I = k0
    DO WHILE ((I .LE. N) .AND. (xI .GT. 1d-15*Binomial1mCDF))
      xI = xI * ((N-I+1)*p0) / (I*q0)
      Binomial1mCDF = Binomial1mCDF + xI
    END DO
  ! Large N case
  ELSE
    mu = N*p0
    ! Large N, small N*p case:
    ! Poisson distribution of average mu = N*p
    IF (mu .LE. MUMAX) THEN
      Binomial1mCDF = Poisson1mCDF(k0,mu)
    ! Large N, non-small N*p case:
    ! Normal distribution of average mu = N*p
    ! and variance N*p*(1-p)
    ! Continuity correction applied
    ELSE
      Binomial1mCDF = Normal1mCDF(1d0*k0+0.5d0,mu,SQRT(N*p0*q0))
    END IF
  END IF
END FUNCTION Binomial1mCDF

!----------------------------------------
! Random number for binomial distribution
!   p           Probability of success for each trial
!   N           Number of trials
FUNCTION RandBinomial(p,N)
  IMPLICIT NONE
  INTEGER :: RandBinomial
  INTEGER, INTENT(IN) :: N
  REAL*8, INTENT(IN) :: p
  INTEGER, PARAMETER :: NMAX = 100
  REAL*8, PARAMETER :: MUMAX = 20.0
  LOGICAL :: flipped
  REAL*8 :: p0,q0,mu,xI,y,sum
  INTEGER :: I
  
  ! Can write P(k|p,N) = P(N-k|1-p,N) = P(k0,p0,N)
  ! Choose smaller of p or 1-p to work with
  IF (p .GT. 0.5d0) THEN
    p0 = 1-p
    q0 = p
    flipped = .TRUE.
  ELSE
    p0 = p
    q0 = 1-p
    flipped = .FALSE.
  END IF
  
  ! Small N case
  IF (N .LE. NMAX) THEN
    y = rand()
    I  = 0
    xI = q0**N
    sum = xI
    DO WHILE (sum .LT. y)
      I = I+1
      ! Bad case: start over (can occur for y extremely close to 1)
      IF (I .GT. N) THEN
        y = rand()
        I = 0
        xI = q0**N
        sum = xI
        CYCLE
      END IF
      xI  = xI * ((N-I+1)*p0) / (I*q0)
      sum = sum + xI
    END DO
    RandBinomial = I
  ! Large N case
  ELSE
    mu = N*p0
    ! Large N, small N*p case:
    ! Poisson distribution of average mu = N*p
    IF (mu .LE. MUMAX) THEN
      RandBinomial = RandPoisson(mu)
      DO WHILE ((RandBinomial .LT. 0) .OR. (RandBinomial .GT. N))
        RandBinomial = RandPoisson(mu)
      END DO
    ! Large N, non-small N*p case:
    ! Normal distribution of average mu = N*p
    ! and variance N*p*(1-p)
    ELSE
      RandBinomial = NINT(RandNormal(mu,SQRT(N*p0*q0)))
      DO WHILE ((RandBinomial .LT. 0) .OR. (RandBinomial .GT. N))
        RandBinomial = NINT(RandNormal(mu,SQRT(N*p0*q0)))
      END DO
    END IF
  END IF
  IF (flipped) RandBinomial = N - RandBinomial
END FUNCTION RandBinomial



!=======================================================================
! ARRAY SORT/SEARCH
!=======================================================================

!-----------------------------------------------------------------------
! Sorts the given array using the heap sort method.
! Faster than quicksort when arrays are close to sorted already.
!   N           length of array
!   x           array of data to be sorted
! 
PURE SUBROUTINE HSort(N,x)
  IMPLICIT NONE
  INTEGER, INTENT(IN) :: N
  REAL*8, INTENT(INOUT) :: x(N)
  INTEGER :: I,Itmp
  REAL*8 :: xtmp
  DO I = N/2, 1, -1
    CALL HSort_siftdown(N,x,I,N)
  END DO
  DO I = N, 2, -1
    xtmp = x(1)
    x(1) = x(I)
    x(I) = xtmp
    CALL HSort_siftdown(N,x,1,I-1)
  END DO
  RETURN
    
  CONTAINS
  ! ------------------------------------------
  ! Utility function for heap sort
  PURE SUBROUTINE HSort_siftdown(N,x,Jlow,Jhigh)
    IMPLICIT NONE
    INTEGER, INTENT(IN) :: N,Jlow,Jhigh
    REAL*8, INTENT(INOUT) :: x(N)
    INTEGER :: J,Jold
    REAL*8 :: x0
    x0 = x(Jlow)
    Jold = Jlow
    J = 2*Jlow
    DO WHILE (J .LE. Jhigh)
      IF ((J .LT. Jhigh) .AND. (x(J)) .LT. x(J+1)) THEN
        J = J+1
      END IF
      IF (x0 .GE. x(J)) EXIT
      x(Jold) = x(J)
      Jold = J
      J = 2*J
    END DO
    x(Jold) = x0
    RETURN
  END SUBROUTINE HSort_siftdown
END SUBROUTINE HSort


! ----------------------------------------------------------------------
! Searches the given array, assumed to be sorted in increasing order,
! for the given value using a binary search algorithm.  Returns the
! index I such that x(I) <= x0 < x(I+1), 0 if x0 < x(1), or N if
! x(N2) <= x0.
!   N               array length
!   x               array of data to be searched (must be sorted)
!   x0              value to search for
! 
PURE FUNCTION BSearch(N,x,x0) RESULT(index)
  IMPLICIT NONE
  INTEGER :: index
  INTEGER, INTENT(IN) :: N
  REAL*8, INTENT(IN) :: x(N),x0
  INTEGER :: Ilow,Ihigh,Imid
  
  ! Check if in bounds
  IF (x0 .LT. x(1)) THEN
    index = 0
    RETURN
  ELSE IF (x0 .GE. x(N)) THEN
    index = N
    RETURN
  END IF
  
  Ilow  = 1
  Ihigh = N
  
  ! Binary search
  DO WHILE (Ihigh-Ilow .GT. 1)
    Imid = (Ilow+Ihigh)/2
    IF (x0 .GE. x(Imid)) THEN
      Ilow = Imid
    ELSE
      Ihigh = Imid
    END IF
  END DO
  index = Ilow
  
END FUNCTION BSearch



!=======================================================================
! TIME UTILITY FUNCTIONS
!=======================================================================

!-----------------------------------------------------------------------
! System time in seconds.
! 
FUNCTION GetTime() RESULT(t)
  IMPLICIT NONE
  REAL*8 :: t
  ! Resolution may vary by kind; ensure highest resolution
  INTEGER(KIND=8) :: count,count_rate
  CALL SYSTEM_CLOCK(count,count_rate)
  t = count*1d0 / count_rate
END FUNCTION


!-----------------------------------------------------------------------
! Time elapsed in seconds since TimeElapsedReset() was called.
! 
FUNCTION TimeElapsed() RESULT(t)
  IMPLICIT NONE
  REAL*8 :: t
  CALL TimeElapsedRoutine(t,.FALSE.)
END FUNCTION


!-----------------------------------------------------------------------
! Reset TimeElapsed timer.  Subroutine version of above.
! 
SUBROUTINE TimeElapsedReset()
  IMPLICIT NONE
  REAL*8 :: t
  CALL TimeElapsedRoutine(t,.TRUE.)
END SUBROUTINE


!-----------------------------------------------------------------------
! Time elapsed in seconds since the parameter 'reset' was set to true.
! Used by TimeElapsed() and TimeElapsedReset() routines, which should
! be used instead of direct calls to this routine.  Note that this is
! wall time, not CPU time.
! 
!   t           Time elpased
!   reset       Logical specifying if the counter should be
!               reset to the current cpu time.
! 
SUBROUTINE TimeElapsedRoutine(t,reset)
  IMPLICIT NONE
  REAL*8, INTENT(OUT) :: t
  LOGICAL,INTENT(IN) :: reset
  ! Resolution may vary by kind; ensure highest resolution
  INTEGER(KIND=8), SAVE :: icount = 0
  INTEGER(KIND=8) :: fcount,count_rate
  IF (reset) THEN
    CALL SYSTEM_CLOCK(icount)
    t = 0d0
    RETURN
  END IF
  CALL SYSTEM_CLOCK(fcount,count_rate)
  t = (fcount-icount)*1d0 / count_rate
END SUBROUTINE


!-----------------------------------------------------------------------
! CPU time elapsed in seconds since CPUTimeElapsedReset() was called.
! For multithreaded cases, this is the sum of time spent by all threads.
! 
FUNCTION CPUTimeElapsed() RESULT(t)
  IMPLICIT NONE
  REAL*8 :: t
  CALL CPUTimeElapsedRoutine(t,.FALSE.)
END FUNCTION


!-----------------------------------------------------------------------
! Reset CPUTimeElapsed timer.
! 
SUBROUTINE CPUTimeElapsedReset()
  IMPLICIT NONE
  REAL*8 :: t
  CALL CPUTimeElapsedRoutine(t,.TRUE.)
END SUBROUTINE


!-----------------------------------------------------------------------
! CPU time elapsed in seconds since the parameter 'reset' was set to
! true.  Used by CPUTimeElapsed() and CPUTimeElapsedReset() routines,
! which should be used instead of direct calls to this routine.
! 
!   t           Time elpased
!   reset       Logical specifying if the counter should be
!               reset to the current cpu time.
! 
SUBROUTINE CPUTimeElapsedRoutine(t,reset)
  IMPLICIT NONE
  REAL*8, INTENT(OUT) :: t
  LOGICAL,INTENT(IN) :: reset
  ! Resolution may vary by kind; ensure highest resolution
  REAL(KIND=8), SAVE :: t1
  REAL(KIND=8) :: t2
  IF (reset) THEN
    CALL CPU_TIME(t1)
    t = 0d0
  END IF
  CALL CPU_TIME(t2)
  t = t2 - t1
END SUBROUTINE



!#######################################################################
END MODULE
!#######################################################################


!#######################################################################
! PROGRAM
!#######################################################################

PROGRAM DDCalc0run
  USE DDCALC0
  IMPLICIT NONE
  CALL DDCalc0_Main()
END PROGRAM



!#######################################################################
! END OF FILE
!#######################################################################

 
