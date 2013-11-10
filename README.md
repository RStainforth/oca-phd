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

In addition, at this current development stage, 'db2soc' can also insert values passed at the command line into the SOC file. This is useful for debugging of LOCAS. For instance at the command line:

	'db2soc -r 123456 -s 500 -x 2000 -y 2000 -z 0'

Forces the SourceID entry on the SOC file "123456_Run.root" to be 500 and the laserball manipulator position to be (2000, 2000, 0) mm. In practice these entries should be done at the SOC processing level i.e. when the SOC file is written in the first place. To find out which entries on the SOC file can be forced type 'db2soc --help' at the command line to view the current available options.

soc2locas: Once entries on the SOC file have been filled 'soc2locas' will then process information from the SOC files to LOCASRun files. 

Example usage at the command line: 'soc2locas -r 123456 -c 654321 -w 987654'

Here, the '-r' argument is the main-run (SOC) file to be processed ("123456_Run.root"), '-c' is the associated central-run ("654321_Run.root") and '-w' an optional associated run at a different wavelength (in SNO, this was classically at 500 nm). The central- and wavelength- runs provide information which can be used to calculate the corrections required for the main run file. 'soc2locas' then writes a LOCASRun file, "123456_LOCASRun.root" to the data/runs/locasrun directory.

LOCAS Utilities
==========

The 'util/' directory stores various scripts which could be useful for LOCAS. Currently there are scripts in 'util/shadowing-tool/' which produce the shadowing values due to the AVHD ropes and the enveloping AV geometry. This tool requires the current system to be connected to ta batch system which makes use of 'qsub' - the job submission executable used on UNIX systems. To use this tool, first ensure "env_locas.sh".

Example usage at the command line: './ShadowingEvGen.sh 0 1000 3000 420 30 1000 123456'

The ShadowingEvGen.sh script will then perform 4 simulations:

	x 1 Run WITH the AV Hold-Down Ropes
	x 1 Run WITHOUT the AV Hold-Down Ropes
	x 1 Run WITH the enveloping AV geometry
	x 1 Run WITHOUT the enveloping AV geometry

These four runs will simulate photon bombs at a position (0,1000,3000) mm in the detector with a wavelength of 420 nm. Each run will consist of 30 x 1000 Event RAT runs simulated on 30 batch node systems. These run parameters will pertain to the same run with ID '123456' and whose associated SOC file is "123456_Run.root" stored in 'data/runs/soc'. Two files will be written with the relative shadowing values "avhd_123456.ratdb" and "geo_123456.ratdb" which will be written to 'data/shadowing/avhd' and 'data/shadowing/geo' respectively. These are the .ratdb files used by the 'db2soc' executable to insert values into the SOC file.
