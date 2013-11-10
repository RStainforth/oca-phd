LOCAS (SNO+ Edition)
==========

About
==========
This is an in-development version of LOCAS for SNO+ which is based on the lagacy software of the same name originally written for the SNO experiment.

Currently LOCAS is able to interface both with RAT and the SOC file format to be used by the SNO+ optics analysis group. SOC files are loaded into LOCAS, processed and output to LOCASRun files. These LOCASRun files are intended to hold the relevant run specific information, i.e. Run ID, Laserball position and the respective PMT information in the run. It is these LOCASRun files which will be used as input to the optics fit.

Installation
==========
To install this version of LOCAS, first type ./configure at the command line in the top directory to setup the evironment file "env_locas.sh". env_locas.sh is created as part of the configure script and will source your RAT installation automatically. Following this, type 'make clean', 'make' and 'make install' respectively in the 'src/' directory to compile LOCAS and the library.

Usage
==========
LOCAS compiles two executables 'db2soc' and 'soc2locas'.

db2soc: This executable is designed for inserting information held in the RAT or LOCAS database (files stored in .ratdb files) into a SOC file. Currently, as part of the SOC data processing, certain entries on the SOC data structure are unfilled and are required to be filled from the database (e.g. DQXX, ANXX (CHS, CSS) flags and relative shadowing values). 

Example use at the command line: 'db2soc -r 123456'

The argument passed to the '-r' option is the run ID. LOCAS then searches for the shadowing values stored in the LOCAS data directory (data/shadowing) for the shadowing values due to the AV Hold-Down ropes (data/shadowing/avhd) and the other enveloping geometry which surrounds the AV (data/shadowing/geo). The values contained within the respective .ratdb files are then inserted into the SOC file with name "123456_Run.root" stored in data/runs/soc.

In addition, at this current development stage, 'db2soc' can also insert values passed at the command line for debugging of code (e.g. The Run ID, Source ID, Laserball Position entries can be forced (although in practice these should be processed at the SOC level once the code is finalised)). To force entries onto the SOC file type 'db2soc --help' at the command line to view further information.

soc2locas: Once entries on the SOC file have been filled 'soc2locas' will then process information from the SOC files to LOCASRun files. 

Example usage at the command line: 'soc2locas -r 123456 -c 654321 -w 987654'

where the '-r' argument is the main-run (SOC) file to be processed, '-c' is the equivalent central-run and '-w' an optional associated run at a different wavelength (in SNO, around 500 nm). The central- and wavelength- runs provide information which can be used to calculate the corrections required for the main run file.
