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
#            labppo - SNO+ detector with inner AV filled with LABPPO
#            labppote0p3perylene - SNO+ detector with inner AV filled with LABPPO+0.3%Te+Perylene
#            labppote0p3bismsb - SNO+ detector with inner AV filled with LABPPO+0.3%Te+Bis-MSB

# Each execution line of 'soc2oca' below creates an OCARun file for the run associated
# with that specified for the (-r) option (i.e. the off-axis run). This script
# is for use with the July 2018 data set, whose SOC files should be placed in
# $OCA_SNOPLUS_DATA/data/runs/soc/jul18/water.

# After using this script the OCARun files will be found in $OCA_SNOPLUS_DATA/data/runs/ocarun/jul18/water

# There should be 40 runs in this dataset
soc2oca -r 114981 -c 115219 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115065 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115085 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115091 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115112 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115120 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115146 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115140 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115167 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115173 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115191 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115198 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115210 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115219 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115246 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115266 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115273 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115301 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115295 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115322 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115328 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115353 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115347 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115371 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115377 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115395 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115401 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115419 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115425 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115443 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115449 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115467 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115473 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115493 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115499 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115517 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115523 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115541 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115237 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
soc2oca -r 115547 -c 114981 -l 33 -d jul18/water -s jul18_337.ocadb -g water
