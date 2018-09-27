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
# is for use with the December 2017 data set, whose SOC files should be placed in 
# $OCA_SNOPLUS_DATA/data/runs/soc/dec17/water.

# After using this script the OCARun files will be found in $OCA_SNOPLUS_DATA/data/runs/ocarun/dec17/water

# There should be 33 runs in this dataset
soc2oca -r 107705 -c 107795 -R 107696 -C 107789 -l 44 -d dec17/water -s dec17_337.ocadb -g water
soc2oca -r 107712 -c 107705 -R 107718 -C 107696 -l 44 -d dec17/water -s dec17_337.ocadb -g water
soc2oca -r 107730 -c 107705 -R 107724 -C 107696 -l 44 -d dec17/water -s dec17_337.ocadb -g water
soc2oca -r 107777 -c 107705 -R 107783 -C 107696 -l 44 -d dec17/water -s dec17_337.ocadb -g water
soc2oca -r 107771 -c 107705 -R 107762 -C 107696 -l 44 -d dec17/water -s dec17_337.ocadb -g water

soc2oca -r 107748 -c 107705 -R 107754 -C 107696 -l 44 -d dec17/water -s dec17_337.ocadb -g water
soc2oca -r 107736 -c 107705 -R 107742 -C 107696 -l 44 -d dec17/water -s dec17_337.ocadb -g water
soc2oca -r 108018 -c 107705 -R 108011 -C 107696 -l 44 -d dec17/water -s dec17_337.ocadb -g water
soc2oca -r 107795 -c 107705 -R 107789 -C 107696 -l 44 -d dec17/water -s dec17_337.ocadb -g water
soc2oca -r 107801 -c 107705 -R 107807 -C 107696 -l 44 -d dec17/water -s dec17_337.ocadb -g water

soc2oca -r 107823 -c 107705 -R 107815 -C 107696 -l 44 -d dec17/water -s dec17_337.ocadb -g water
soc2oca -r 107829 -c 107705 -R 107835 -C 107696 -l 44 -d dec17/water -s dec17_337.ocadb -g water
soc2oca -r 107845 -c 107705 -R 107858 -C 107696 -l 44 -d dec17/water -s dec17_337.ocadb -g water
soc2oca -r 107870 -c 107705 -R 107864 -C 107696 -l 44 -d dec17/water -s dec17_337.ocadb -g water
soc2oca -r 107892 -c 107705 -R 107886 -C 107696 -l 44 -d dec17/water -s dec17_337.ocadb -g water

soc2oca -r 107876 -c 107705 -R 107882 -C 107696 -l 44 -d dec17/water -s dec17_337.ocadb -g water
# The next run does not have a wavelength run, therefore it will use it's own fitted position
soc2oca -r 107903 -c 107705 -C 107696 -l 34 -d dec17/water -s dec17_337.ocadb -g water
soc2oca -r 107907 -c 107705 -R 107915 -C 107696 -l 44 -d dec17/water -s dec17_337.ocadb -g water
soc2oca -r 108052 -c 107705 -R 108058 -C 107696 -l 44 -d dec17/water -s dec17_337.ocadb -g water
soc2oca -r 108070 -c 107705 -R 108064 -C 107696 -l 44 -d dec17/water -s dec17_337.ocadb -g water

soc2oca -r 108076 -c 107705 -R 108082 -C 107696 -l 44 -d dec17/water -s dec17_337.ocadb -g water
soc2oca -r 108094 -c 107705 -R 108088 -C 107696 -l 44 -d dec17/water -s dec17_337.ocadb -g water
soc2oca -r 107929 -c 107705 -R 107954 -C 107696 -l 44 -d dec17/water -s dec17_337.ocadb -g water
soc2oca -r 107944 -c 107705 -R 107950 -C 107696 -l 44 -d dec17/water -s dec17_337.ocadb -g water
soc2oca -r 108119 -c 107705 -R 108113 -C 107696 -l 44 -d dec17/water -s dec17_337.ocadb -g water

soc2oca -r 108100 -c 107705 -R 108106 -C 107696 -l 44 -d dec17/water -s dec17_337.ocadb -g water
soc2oca -r 108046 -c 107705 -R 108040 -C 107696 -l 44 -d dec17/water -s dec17_337.ocadb -g water
soc2oca -r 108125 -c 107705 -R 108131 -C 107696 -l 44 -d dec17/water -s dec17_337.ocadb -g water
soc2oca -r 108024 -c 107705 -R 108030 -C 107696 -l 44 -d dec17/water -s dec17_337.ocadb -g water
soc2oca -r 107968 -c 107705 -R 107962 -C 107696 -l 44 -d dec17/water -s dec17_337.ocadb -g water

soc2oca -r 107974 -c 107705 -R 107980 -C 107696 -l 44 -d dec17/water -s dec17_337.ocadb -g water
soc2oca -r 107998 -c 107705 -R 108005 -C 107696 -l 44 -d dec17/water -s dec17_337.ocadb -g water
soc2oca -r 107992 -c 107705 -R 107986 -C 107696 -l 44 -d dec17/water -s dec17_337.ocadb -g water