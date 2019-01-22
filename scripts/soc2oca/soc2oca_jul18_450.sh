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
# is for use with the July 2018 data set, whose SOC files should be placed in
# $OCA_SNOPLUS_DATA/data/runs/soc/jul18/water.

# After using this script the OCARun files will be found in $OCA_SNOPLUS_DATA/data/runs/ocarun/jul18/water

# There should be 37 runs in this dataset
soc2oca -r 114977 -c 115223 -R 115059 -l 43 -d jul18/water -s jul18_450.ocadb -g water
soc2oca -r 115069 -c 114977 -R 115073 -l 43 -d jul18/water -s jul18_450.ocadb -g water
soc2oca -r 115081 -c 114977 -R 115075 -l 43 -d jul18/water -s jul18_450.ocadb -g water
soc2oca -r 115095 -c 114977 -R 115099 -l 43 -d jul18/water -s jul18_450.ocadb -g water
soc2oca -r 115105 -c 114977 -R 115101 -l 43 -d jul18/water -s jul18_450.ocadb -g water
soc2oca -r 115124 -c 114977 -R 115128 -l 43 -d jul18/water -s jul18_450.ocadb -g water
soc2oca -r 115150 -c 114977 -R 115157 -l 43 -d jul18/water -s jul18_450.ocadb -g water
soc2oca -r 115134 -c 114977 -R 115130 -l 43 -d jul18/water -s jul18_450.ocadb -g water
soc2oca -r 115163 -c 114977 -R 115159 -l 43 -d jul18/water -s jul18_450.ocadb -g water
soc2oca -r 115177 -c 114977 -R 115181 -l 43 -d jul18/water -s jul18_450.ocadb -g water
soc2oca -r 115187 -c 114977 -R 115183 -l 43 -d jul18/water -s jul18_450.ocadb -g water
soc2oca -r 115202 -c 114977 -R 115206 -l 43 -d jul18/water -s jul18_450.ocadb -g water
soc2oca -r 115053 -c 114977 -R 115057 -l 43 -d jul18/water -s jul18_450.ocadb -g water
soc2oca -r 115223 -c 114977 -R 115227 -l 43 -d jul18/water -s jul18_450.ocadb -g water
soc2oca -r 115250 -c 114977 -R 115254 -l 43 -d jul18/water -s jul18_450.ocadb -g water
soc2oca -r 115260 -c 114977 -R 115256 -l 43 -d jul18/water -s jul18_450.ocadb -g water
soc2oca -r 115279 -c 114977 -R 115285 -l 43 -d jul18/water -s jul18_450.ocadb -g water
soc2oca -r 115306 -c 114977 -R 115310 -l 43 -d jul18/water -s jul18_450.ocadb -g water
soc2oca -r 115291 -c 114977 -R 115287 -l 43 -d jul18/water -s jul18_450.ocadb -g water
soc2oca -r 115318 -c 114977 -R 115314 -l 43 -d jul18/water -s jul18_450.ocadb -g water
soc2oca -r 115333 -c 114977 -R 115337 -l 43 -d jul18/water -s jul18_450.ocadb -g water
soc2oca -r 115357 -c 114977 -R 115361 -l 43 -d jul18/water -s jul18_450.ocadb -g water
soc2oca -r 115343 -c 114977 -R 115339 -l 43 -d jul18/water -s jul18_450.ocadb -g water
soc2oca -r 115367 -c 114977 -R 115363 -l 43 -d jul18/water -s jul18_450.ocadb -g water
soc2oca -r 115381 -c 114977 -R 115385 -l 43 -d jul18/water -s jul18_450.ocadb -g water
soc2oca -r 115391 -c 114977 -R 115387 -l 43 -d jul18/water -s jul18_450.ocadb -g water
soc2oca -r 115405 -c 114977 -R 115409 -l 43 -d jul18/water -s jul18_450.ocadb -g water
soc2oca -r 115415 -c 114977 -R 115411 -l 43 -d jul18/water -s jul18_450.ocadb -g water
soc2oca -r 115429 -c 114977 -R 115433 -l 43 -d jul18/water -s jul18_450.ocadb -g water
soc2oca -r 115439 -c 114977 -R 115435 -l 43 -d jul18/water -s jul18_450.ocadb -g water
soc2oca -r 115453 -c 114977 -R 115457 -l 43 -d jul18/water -s jul18_450.ocadb -g water
soc2oca -r 115463 -c 114977 -R 115459 -l 43 -d jul18/water -s jul18_450.ocadb -g water
soc2oca -r 115477 -c 114977 -R 115481 -l 43 -d jul18/water -s jul18_450.ocadb -g water
soc2oca -r 115489 -c 114977 -R 115485 -l 43 -d jul18/water -s jul18_450.ocadb -g water
soc2oca -r 115503 -c 114977 -R 115507 -l 43 -d jul18/water -s jul18_450.ocadb -g water
soc2oca -r 115513 -c 114977 -R 115509 -l 43 -d jul18/water -s jul18_450.ocadb -g water
soc2oca -r 115527 -c 114977 -R 115531 -l 43 -d jul18/water -s jul18_450.ocadb -g water
