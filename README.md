OCA (SNO+ Edition)
==========

About
==========
This is an in-development version of OCA for SNO+. OCA is designed to process SOC data files and perform a statistical fit which characterises the optical response of the SNO+ detector.

SOC files are ROOT files with a specific data structure designed for use by the SNO+ optics group. Optical calibration is, in part, sought through laserball runs; periods of time for which pulses of laser light is emitted (near) isotropically throughout the detector. A single SOC file will represent a single laserball run and will contain information pertaining to the run itself; e.g. the run ID, the laser wavelength used, the position of the laserball in the detector as well as the relevant PMT information for that run. 

OCA is intended to perform the following tasks:

    1) Process data from laserball runs i.e. the SOC files.
    2) Output OCARun files (a repackaged SOC file with additional information used by the fit).
    3) Using various OCARun files as input - perform the optical fit of the detector response.
Current Capability
==========
OCA is currently able to perform tasks 1), 2) and 3) in the list above. OCA interfaces both with RAT and the SOC file format and outputs this information to OCARun files. These OCARun files are intended to hold the relevant run specific information held by the original SOC file (run ID, laserball position, PMT information etc.) as well additional information for the individual PMTs; such as the corrected occupancy, time of flight, solid angle and Fresnel transmission coefficient. It is these OCARun files which are used as input to the optics fit (task 3 in the above list).

Prerequisites
==========
OCA requires a local installation of RAT. It is intended that OCA will work alongside future releases of RAT. However at this stage OCA only works alongside a specific development branch of RAT; the 'oca-plus' branch of RStainforth/rat.git on GitHub.com. To clone into this branch type the following at the command line (assuming 'git' is pre-installed):

     git clone -b oca-plus git@github.com:RStainforth/rat.git
Then type:

     ./configure 

in the 'rat/' directory - this will create a file 'rat/env.sh'. You should then create a new environment file for this version of RAT i.e. a new version of the 'env_rat-dev.sh' file that you may be familiar with if you have previously used 'snoing'-installed version of RAT. Your new version of 'env_rat-dev.sh' should be almost identical to the 'snoing' version, except this one sources the newly created 'rat/env.sh' file. Source this new file at the command line (suppose I called my .env file 'env_rat-dev-oca.sh'):

     source env_rat-dev-oca.sh
Now that you are in the RAT environment for this specific branch, build and compile RAT using the following command:

     scons
in the '/rat' directory.

Installation
==========
To install OCA, first, in the top directory of oca-plus ('oca-plus/') type: 

    ./configure 
at the command line. The script will ask you for three pieces of information:

   1)  The full system path to your newly created RAT environment file (see last section).
   2) The full system path to a directory you would like OCA utilities to temporarily store data (this is used for AV hold-down rope shadowing calculations). This temporary directory should be a 'scratch' disk with plenty of space.
   3) The machine names of the computers on your local computer cluster. For example, at Liverpool this could be 'medium64', 'all64', 'short64' or at QMUL it is 'snoplusSL6'.

The configure script will create a new OCA environment file 'env_oca.sh' in the top directory. When you source this new file, i.e.:

    source env_oca.sh

at the command line, it will also source your RAT envrionment file automatically from the location you specified as part of the configure script. Now move to the 'oca-plus/src' directory and type the following commands:

    make clean
    make
    make install 
respectively to compile OCA and the library shared object file 'libOCA.so'.

Finally, replace the 'rootInit.C' file provided here with the one featured in your RAT installation top directory. This file merely ensures that RAT AND OCA libraries are loaded into ROOT.

Usage
==========
OCA currently compiles four executables 'soc2soc', 'db2soc', 'soc2oca', 'oca2marquart', 'oca2minuit' and 'oca2debug'.

soc2soc
==========
This executable is a utility similar to 'hadd' which comes with ROOT. 'soc2soc' adds the PMT information from many SOC files together to create one single SOC file. For example, using a local batch system, you may have simulated 5 x 2000 event SOC files for the laserball at the centre of the detector. It would be convenient to put all these SOC files together into a single 10,000 event SOC file for use in OCA or some other analysis. To do this, the example usage at the command line would be:

    soc2soc final_soc.root soc_1.root soc_2.root soc_3.root soc_4.root soc_5.root
where 'final_soc.root' is the name of the single SOC file you would like to create from the five individual SOC files entitled soc_1.root, soc_2.root, ..., soc_5.root.

On each SOC file, each of the single SOCPMT objects and their respective fields are summed over as follows:

   1) TACs - Every TAC from the same channel across all files are added together.
   2) QHSs and QHLs - Every QHS and QHL from the same channel across all files are added together.
   3) Prompt Counts - Every prompt count on the same channel across all files are added together.
   4) Calculated TAC and RMS - These are summed over all files for the same channel and averaged.
   5) Time of Flight (TOF) - These are all summed and averaged.

'soc2soc' was created because 'hadd' includes each PMT from a SOC file as an individual entry. For example, in the above scenario, instead of approximately 9,000 PMT entries on the SOC file (representing the 9000 individual PMTs), there would actually be 5 x 9,000 entries. This was the main problem with 'hadd' and the motivation for writing this executable.

Note: It is intended that the final SOC file will have a dummy RAT::DS::Run object written to a TTree, but it will be empty. This is to avoid conflict with the pre-existing SOC file format for .root files. For now this is not included - so ignore any ROOT errors about a 'runT' object not existing when using RAT methods.


db2soc 
==========
This executable is designed for inserting information held in the RAT or OCA database (files stored in .ratdb files) into a SOC file. Currently, as part of the SOC data processing, certain entries on the SOC data structure are unfilled and are required to be filled from the database (e.g. DQXX, ANXX (CHS, CSS) flags and relative shadowing values). 

Example use at the command line: 

    db2soc -r 123456 -f 123456 --
The argument passed to the '-r' option is the run ID. OCA then searches for the corresponding SOC file '123456_Run.root' in the 'oca-plus/data/runs/soc' directory. If found, it will then search for the corresponding shadowing values based on the '-f' argument, stored in the OCA data directory (data/shadowing). For the shadowing values due to the AV Hold-Down ropes OCA will search the 'oca-plus/data/shadowing/avhd' directory for a file of the form 'avhd_123456.ratdb'. The shadowing values for the other enveloping geometry which surrounds the AV will be searched for in the 'oca-plus/data/shadowing/geo' directory, specifically to check if the file 'geo_123456.ratdb' exists. The values contained within these respective .ratdb files are then inserted into the SOC file.

NOTE: ROOT doesn't actually allow for entries to be re-written on pre-existing ROOT trees. In essence, 'db2soc' actually first duplicates the SOC ROOT tree object located on the file, edits the required entries and overwrites the entire tree itself.

In addition, currently, 'db2soc' can also insert values passed at the command line into the SOC file. This is useful for debugging of OCA. For instance at the command line:

	db2soc -r 123456 -s 500 -x 2000 -y 2000 -z 0 --
Forces the SOC::SourceID entry on the SOC file "123456_Run.root" to be 500 and the laserball manipulator position to be (2000, 2000, 0) mm. In practice these entries should be written at the SOC processing level i.e. when the SOC file is written in the first place. But for now this feature is included as it is useful to test and debug various OCA processes which are subject to change. Obviously, if a SOC file contains simulated data corresponding to a run at the centre of the detector and the laserball manipulator position is forced using the '-x', '-y' and '-z' options - the data will lose any significance. 

To find out which entries on the SOC file can be forced type 'db2soc --help' at the command line to view all the current available options. It always recommended to end executable commands on the command line with the '--' termination token.

soc2oca
==========
Once entries on the SOC file have been filled, 'soc2oca' will then process information from the SOC files to OCARun files. 

Example usage at the command line: 

    soc2oca -r 123456 -c 654321 -w 987654
Here, the '-r' argument is the main-run (SOC) file to be processed ("123456_Run.root"), '-c' is the associated central-run ("654321_Run.root") and '-w' an optional associated run at a different wavelength (in SNO, this was usually at 500 nm). The central- and wavelength-runs provide information which can be used to calculate the corrections to some of the optical parameters required for the main run file. 'soc2oca' writes a OCARun file, "123456_OCARun.root" to the 'oca-plus/data/runs/ocarun' directory. The -w option is optional.

oca2marquardt & oca2minuit
==========
The 'oca2marquardt' and 'oca2minuit' executables are responsible for processing the OCARun files and implementing them into a fit of the optical response of the detector. either by using the Levenberg-Marquardt algorithm (a-la SNO) or by using ROOT's implementation of Minuit. To perform the fit, all the parameters and cut crieria are specified by a 'fitfile' which are found in 'oca-plus/data/fitfiles'. The format of the fitfiles is currently different depending on whether 'oca2mardquardt' or 'oca2minuit' is being used. For information on the format of these fitfiles, see the README.md file in the 'oca-plus/data/fitfiles' directory.

Example usage of these fitting executables at the command line is as follows:

    oca2marquardt /path/to/marquardt_fit_file.ratdb
    oca2minuit /path/to/minuit_fit_file.ratdb

Note: This executable is currently in development, if you are reading this note then it is not quite yet complete and it is likely you don't have appropriate data to fit to.

oca2debug
==========
Whilst OCA remains in development, the 'oca2debug' can be edited by more adventurous folk by modifiying oca2debug.cc and recompiling OCA using 'make clean', 'make' and 'make install'. This is useful for testing the functionality of specific aspects of the OCA class files.

OCA Utilities
==========
The 'oca-plus/util/' directory stores various scripts which could be useful for OCA. See the individual README.md files in each of the aforementioned subdirectories for more information.


NOTES
==========

1) Currently, by default OCA will search for SOC files in the 'oca-plus/data/runs/soc' directory. The SOC files should be named by run ID, i.e. for the laserball run with run id '123456', the associated SOC file should be named '123456_Run.root' and located in the 'oca-plus/data/runs/soc' directory. Once processed, OCA will output the OCARun files to 'oca-plus/data/runs/ocarun' and will write filenames of the form '123456_OCARun.root'.

