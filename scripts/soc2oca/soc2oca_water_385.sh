# Commands are of the following form:
# soc2oca -r [run-id] -c [c-run-id]
#         -R [wavelength-run-id] -C [wavelength-central-run-id]
#         -l [laserball-position-code]
#         -d [MMYY-material-directory]
#         -s [systematic-file-path]
#         -g [geometry-option]
#
# Option Descriptors:
#     Note: When specfying runs, 'soc2oca' searches in $OCA_SNOPLUS_DATA/runs/soc/[MMYY-material-directory]
#
#     -r The run ID of the off-axis laserball run SOC file of form "<run-id>_Run.root"
#     -c The run ID of the central laserball run SOC file of form "<c-run-id>_Run.root"
#     -R The run ID of the off-axis laserball run SOC file from the wavelength run of form "<wavelength-run-id>_Run.root"
#     -C The run ID of the central laserball run SOC file from the wavelength run of form "<wavelength-central-run-id>_Run.root"
#     -l The laserball position to be used. Option is of form 'XY' X: off-axis run, Y: central run. Several options are available:
#                   X : Off-Axis run laserball position.
#                     X = 1 : Off-Axis manipulator laserball position 
#                     X = 2 : Off-Axis camera laserball position [placeholder, not currently in]
#                     X = 3 : Off-Axis fitted laserball position
#                     X = 4 : Off-Axis fitted laserball position from wavelength run (-R option)
#
#                   Y : Central run laserball position.
#                     Y = 1 : Central manipulator laserball position
#                     Y = 2 : Central camera laserball position [placeholder, not currently in]
#                     Y = 3 : Central fitted laserball position
#                     Y = 4 : Central fitted laserball position from wavelength run (-C option)
#
#     -d The name of the directory (usually ordered by date) in $OCA_SNOPLUS_ROOT/data/runs/soc from where to find the SOC files to be converted.
#     -s The name of the systematic file (from which the systematic variations are applied) found in $OCA_SNOPLUS_DATA/systematics
#     -g The geometry option. Throughout SNO and SNO+ the PMT positions or orientations may have changed. Therefore, this needs to be specfied 
#        such that the correct detector state is used. In addition, calculation of shadowing requires knowledge of the detector geometry. 
#        For example, in SNO, there were no AV hold-down ropes but in SNO+ there are. The geometry option also determines the inner AV 
#        material such that the correct group velocties are used. The current options are as follows:
#            sno - The SNO detector geometry (no AV hold-down ropes)
#            water - SNO+ detector with inner AV filled with water
#            scintillator - SNO+ detector with inner AV filled with the default LABPPO
#            te-loaded - SNO+ detector with inner AV filled with the default cocktail for the Te-loaded phase
#        Note: the default materials will depend on the RAT version used to run OCA

# Each execution line of 'soc2oca' below creates an OCARun file for the run associated
# with that specified for the (-r) option (i.e. the off-axis run). This script
# is for use with the example data set (SNO+ MC October 2015, SOC files) provided in
# $OCA_SNOPLUS_DATA/data/runs/soc/oct15/water.

# After using this script the OCARun files will be found in $OCA_SNOPLUS_DATA/data/runs/ocarun/oct15/water

# There should be 39 runs in this dataset
soc2oca -r 238501 -c 2385039 -R 250501 -C 2505039 -l 44 -d oct15/water -s water_385.ocadb -g water 
soc2oca -r 238502 -c 2385039 -R 250502 -C 2505039 -l 44 -d oct15/water -s water_385.ocadb -g water 
soc2oca -r 238503 -c 2385039 -R 250503 -C 2505039 -l 44 -d oct15/water -s water_385.ocadb -g water 
soc2oca -r 238504 -c 2385039 -R 250504 -C 2505039 -l 44 -d oct15/water -s water_385.ocadb -g water 

soc2oca -r 238505 -c 2385039 -R 250505 -C 2505039 -l 44 -d oct15/water -s water_385.ocadb -g water 
soc2oca -r 238506 -c 2385039 -R 250506 -C 2505039 -l 44 -d oct15/water -s water_385.ocadb -g water 
soc2oca -r 238507 -c 2385039 -R 250507 -C 2505039 -l 44 -d oct15/water -s water_385.ocadb -g water 
soc2oca -r 238508 -c 2385036 -R 250508 -C 2505036 -l 44 -d oct15/water -s water_385.ocadb -g water 

soc2oca -r 238509 -c 2385036 -R 250509 -C 2505036 -l 44 -d oct15/water -s water_385.ocadb -g water 
soc2oca -r 2385010 -c 2385036 -R 2505010 -C 2505036 -l 44 -d oct15/water -s water_385.ocadb -g water 
soc2oca -r 2385011 -c 2385036 -R 2505011 -C 2505036 -l 44 -d oct15/water -s water_385.ocadb -g water 
soc2oca -r 2385012 -c 2385036 -R 2505012 -C 2505036 -l 44 -d oct15/water -s water_385.ocadb -g water 

soc2oca -r 2385013 -c 2385036 -R 2505013 -C 2505036 -l 44 -d oct15/water -s water_385.ocadb -g water 
soc2oca -r 2385014 -c 2385036 -R 2505014 -C 2505036 -l 44 -d oct15/water -s water_385.ocadb -g water 
soc2oca -r 2385015 -c 2385039 -R 2505015 -C 2505039 -l 44 -d oct15/water -s water_385.ocadb -g water 
soc2oca -r 2385016 -c 2385039 -R 2505016 -C 2505039 -l 44 -d oct15/water -s water_385.ocadb -g water 

soc2oca -r 2385017 -c 2385039 -R 2505017 -C 2505039 -l 44 -d oct15/water -s water_385.ocadb -g water 
soc2oca -r 2385018 -c 2385036 -R 2505018 -C 2505036 -l 44 -d oct15/water -s water_385.ocadb -g water 
soc2oca -r 2385019 -c 2385039 -R 2505019 -C 2505039 -l 44 -d oct15/water -s water_385.ocadb -g water 
soc2oca -r 2385020 -c 2385039 -R 2505020 -C 2505039 -l 44 -d oct15/water -s water_385.ocadb -g water 

soc2oca -r 2385021 -c 2385036 -R 2505021 -C 2505036 -l 44 -d oct15/water -s water_385.ocadb -g water 
soc2oca -r 2385022 -c 2385036 -R 2505022 -C 2505036 -l 44 -d oct15/water -s water_385.ocadb -g water 
soc2oca -r 2385023 -c 2385036 -R 2505023 -C 2505036 -l 44 -d oct15/water -s water_385.ocadb -g water 
soc2oca -r 2385024 -c 2385036 -R 2505024 -C 2505036 -l 44 -d oct15/water -s water_385.ocadb -g water 

soc2oca -r 2385025 -c 2385036 -R 2505025 -C 2505036 -l 44 -d oct15/water -s water_385.ocadb -g water 
soc2oca -r 2385026 -c 2385036 -R 2505026 -C 2505036 -l 44 -d oct15/water -s water_385.ocadb -g water 
soc2oca -r 2385027 -c 2385036 -R 2505027 -C 2505036 -l 44 -d oct15/water -s water_385.ocadb -g water 
soc2oca -r 2385028 -c 2385036 -R 2505028 -C 2505036 -l 44 -d oct15/water -s water_385.ocadb -g water 

soc2oca -r 2385029 -c 2385039 -R 2505029 -C 2505039 -l 44 -d oct15/water -s water_385.ocadb -g water 
soc2oca -r 2385030 -c 2385039 -R 2505030 -C 2505039 -l 44 -d oct15/water -s water_385.ocadb -g water 
soc2oca -r 2385031 -c 2385039 -R 2505031 -C 2505039 -l 44 -d oct15/water -s water_385.ocadb -g water 
soc2oca -r 2385032 -c 2385039 -R 2505032 -C 2505039 -l 44 -d oct15/water -s water_385.ocadb -g water 

soc2oca -r 2385033 -c 2385039 -R 2505033 -C 2505039 -l 44 -d oct15/water -s water_385.ocadb -g water 
soc2oca -r 2385034 -c 2385039 -R 2505034 -C 2505039 -l 44 -d oct15/water -s water_385.ocadb -g water 
soc2oca -r 2385035 -c 2385039 -R 2505035 -C 2505039 -l 44 -d oct15/water -s water_385.ocadb -g water 
soc2oca -r 2385036 -c 2385037 -R 2505036 -C 2505037 -l 44 -d oct15/water -s water_385.ocadb -g water 

soc2oca -r 2385037 -c 2385038 -R 2505037 -C 2505038 -l 44 -d oct15/water -s water_385.ocadb -g water 
soc2oca -r 2385038 -c 2385039 -R 2505038 -C 2505039 -l 44 -d oct15/water -s water_385.ocadb -g water 
soc2oca -r 2385039 -c 2385036 -R 2505039 -C 2505036 -l 44 -d oct15/water -s water_385.ocadb -g water 
