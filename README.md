OCA (SNO+ Edition)
==========

About
==========
This is OCA for SNO+. OCA is designed to process SOC data files and perform a statistical fit that characterises the optical response of the SNO+ detector. It is intended for use with SOC files that contain laserball run data from either RAT or real laserball data.

SOC files are ROOT files with a specific data structure designed for use by the SNO+ optics group. Optical calibration is, in part, sought through laserball runs; periods of time for which pulses of laser light is emitted (near) isotropically throughout the detector. A single SOC file will represent a single laserball run and will contain information pertaining to the run itself; e.g. the run ID, the laser wavelength used, the position and orientation of the laserball in the detector as well as the relevant PMT information for that run. 

OCA is intended to perform the following tasks:

    1) Process data from laserball runs i.e. the SOC files.
    2) Output OCARun files (a repackaged SOC file with additional information used by the fit).
    3) Using various OCARun files as input - perform the optical fit of the detector response.


OCARun Files
==========
OCA interfaces both with RAT and the SOC file format and outputs this information to OCARun files. These OCARun files are intended to hold the relevant run specific information held by the original SOC file (run ID, laserball position, PMT information etc.) as well additional information for the individual PMTs; such as the corrected occupancy, time of flight, solid angle and Fresnel transmission coefficient. It is these OCARun files which are used as input to the optics fit (task 3 in the above list).

Prerequisites
==========
OCA requires a local installation of RAT. The most recent version of RAT for which OCA has found to be compatible is RAT 5.3.1

Installation
==========
To install OCA, first, in the top directory of oca ('oca/') type: 

    ./configure 
at the command line. The script will ask you for two pieces of information:

   1) The full system path to your RAT environment file.
   2) The full system path to a directory you would like OCA utilities to temporarily store data

The configure script will create a new OCA environment file 'env_oca_snoplus.sh' in the top directory. When you source this new file, i.e.:

    source env_oca_snoplus.sh

at the command line, it will also source your RAT environment file automatically from the location you specified as part of the configure script. Now move to the 'oca-plus/src' directory and type the following commands:

    make clean
    make
    make install 
respectively to compile OCA and the library shared object file 'libOCA_SNO.so' and ‘libOCA_SNOPLUS.so’. OCA contains the original classes used for the SNO LOCAS code. This allows for backwards compatibility, and these classes are in the ‘libOCA_SNO.so’ shared object file. The OCA class files used for SNO+ are in the ‘libOCA_SNOPLUS.so’ shared object file.

Finally, replace the 'rootInit.C' file provided here with the one featured in your RAT installation top directory. This file merely ensures that RAT and OCA libraries are loaded into ROOT.

Usage
==========
OCA compiles three executables ‘rdt2soc', 'soc2oca', 'oca2fit’.

rdt2soc
==========
*To be written*

soc2oca
==========
Once entries on the SOC file have been filled, 'soc2oca' will then process information from the SOC files to OCARun files. 

Example usage at the command line: 

    soc2oca -r 123456 -c 654321 -w 987654
Here, the '-r' argument is the main-run (SOC) file to be processed ("123456_Run.root"), '-c' is the associated central-run ("654321_Run.root") and '-w' an optional associated run at a different wavelength (in SNO, this was usually at 500 nm). The central- and wavelength-runs provide information which can be used to calculate the corrections to some of the optical parameters required for the main run file. 'soc2oca' writes a OCARun file, "123456_OCARun.root" to the 'oca-plus/data/runs/ocarun' directory. The -w option is optional.

oca2fit
==========
The 'oca2marquardt' and 'oca2minuit' executables are responsible for processing the OCARun files and implementing them into a fit of the optical response of the detector. either by using the Levenberg-Marquardt algorithm (a-la SNO) or by using ROOT's implementation of Minuit. To perform the fit, all the parameters and cut crieria are specified by a 'fitfile' which are found in 'oca-plus/data/fitfiles'. The format of the fitfiles is currently different depending on whether 'oca2mardquardt' or 'oca2minuit' is being used. For information on the format of these fitfiles, see the README.md file in the 'oca-plus/data/fitfiles' directory.

Example usage of these fitting executables at the command line is as follows:

    oca2marquardt /path/to/marquardt_fit_file.ratdb
    oca2minuit /path/to/minuit_fit_file.ratdb

Note: This executable is currently in development, if you are reading this note then it is not quite yet complete and it is likely you don't have appropriate data to fit to.

OCA Utilities
==========
The 'oca-plus/util/' directory stores various scripts which could be useful for OCA. See the individual README.md files in each of the aforementioned subdirectories for more information.


NOTES
==========

1) Currently, by default OCA will search for SOC files in the 'oca-plus/data/runs/soc' directory. The SOC files should be named by run ID, i.e. for the laserball run with run id '123456', the associated SOC file should be named '123456_Run.root' and located in the 'oca-plus/data/runs/soc' directory. Once processed, OCA will output the OCARun files to 'oca-plus/data/runs/ocarun' and will write filenames of the form '123456_OCARun.root'.

