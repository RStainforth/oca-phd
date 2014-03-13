Shadowing Utility
==========
Currently there are scripts in 'locas-plus/util/shadowing-tool' which produce the shadowing values due to the AV hold-down ropes and the enveloping AV geometry. This tool requires the current system to be connected to a batch-node system which makes use of 'qsub' - the job submission executable. To use this tool, first ensure "env_locas.sh" has been sourced.

Example usage at the command line: 

    ./ShadowingEvGen.sh 0 1000 3000 420 30 1000 123456

The ShadowingEvGen.sh script will then perform 4 simulations of photon bombs at a position (0,1000,3000) mm (1st, 2nd and 3rd arguments) in the detector with a wavelength of 420 nm (4th argument). The 4 simulations are:

	1 Run WITH the AV Hold-Down Ropes
	1 Run WITHOUT the AV Hold-Down Ropes
	1 Run WITH the enveloping AV geometry
	1 Run WITHOUT the enveloping AV geometry

Each run will consist of 30 x 1000 (=30,000) Events (5th argument x 6th argument) split up into 30 RAT runs of 1000 events each simulated on 30 batch node systems. The run specification passed at the command line will coincide with the same run whose ID is '123456' (7th argument) and whose associated SOC file is "123456_Run.root" stored in 'locas-plus/data/runs/soc'. Two files will be written with the relative shadowing values "avhd_123456.ratdb" and "geo_123456.ratdb" which will be written to 'locas-plus/data/shadowing/avhd' and 'locas-plus/data/shadowing/geo' respectively. These are the .ratdb files used by the 'db2soc' executable to insert values into the SOC file.

NOTES
==========

1) In the 'locas-plus/data/shadowing/avhd' and 'locas-plus/data/shadowing/geo' directories - there are example shadowing value files.