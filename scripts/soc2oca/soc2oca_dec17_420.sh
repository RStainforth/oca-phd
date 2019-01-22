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
# is for use with the December 2017 data set, whose SOC files should be placed in 
# $OCA_SNOPLUS_DATA/data/runs/soc/dec17/water.

# After using this script the OCARun files will be found in $OCA_SNOPLUS_DATA/data/runs/ocarun/dec17/water

# There should be 30 runs in this dataset
soc2oca -r 107696 -c 107789 -l 33 -d dec17/water -s dec17_420.ocadb -g water
soc2oca -r 107718 -c 107696 -l 33 -d dec17/water -s dec17_420.ocadb -g water
soc2oca -r 107724 -c 107696 -l 33 -d dec17/water -s dec17_420.ocadb -g water
soc2oca -r 107783 -c 107696 -l 33 -d dec17/water -s dec17_420.ocadb -g water
soc2oca -r 107762 -c 107696 -l 33 -d dec17/water -s dec17_420.ocadb -g water

soc2oca -r 107754 -c 107696 -l 33 -d dec17/water -s dec17_420.ocadb -g water
soc2oca -r 107742 -c 107696 -l 33 -d dec17/water -s dec17_420.ocadb -g water
soc2oca -r 108011 -c 107696 -l 33 -d dec17/water -s dec17_420.ocadb -g water
soc2oca -r 107789 -c 107696 -l 33 -d dec17/water -s dec17_420.ocadb -g water
soc2oca -r 107807 -c 107696 -l 33 -d dec17/water -s dec17_420.ocadb -g water

soc2oca -r 107815 -c 107696 -l 33 -d dec17/water -s dec17_420.ocadb -g water
soc2oca -r 107835 -c 107696 -l 33 -d dec17/water -s dec17_420.ocadb -g water
soc2oca -r 107858 -c 107696 -l 33 -d dec17/water -s dec17_420.ocadb -g water
soc2oca -r 107864 -c 107696 -l 33 -d dec17/water -s dec17_420.ocadb -g water
soc2oca -r 107886 -c 107696 -l 33 -d dec17/water -s dec17_420.ocadb -g water

soc2oca -r 107882 -c 107696 -l 33 -d dec17/water -s dec17_420.ocadb -g water
soc2oca -r 107917 -c 107696 -l 33 -d dec17/water -s dec17_420.ocadb -g water
soc2oca -r 107915 -c 107696 -l 33 -d dec17/water -s dec17_420.ocadb -g water
soc2oca -r 108058 -c 107696 -l 33 -d dec17/water -s dec17_420.ocadb -g water
soc2oca -r 108064 -c 107696 -l 33 -d dec17/water -s dec17_420.ocadb -g water

soc2oca -r 108082 -c 107696 -l 33 -d dec17/water -s dec17_420.ocadb -g water
soc2oca -r 108088 -c 107696 -l 33 -d dec17/water -s dec17_420.ocadb -g water
soc2oca -r 107954 -c 107696 -l 33 -d dec17/water -s dec17_420.ocadb -g water
soc2oca -r 107950 -c 107696 -l 33 -d dec17/water -s dec17_420.ocadb -g water
soc2oca -r 108113 -c 107696 -l 33 -d dec17/water -s dec17_420.ocadb -g water

soc2oca -r 108106 -c 107696 -l 33 -d dec17/water -s dec17_420.ocadb -g water
soc2oca -r 108134 -c 107696 -l 33 -d dec17/water -s dec17_420.ocadb -g water
soc2oca -r 107962 -c 107696 -l 33 -d dec17/water -s dec17_420.ocadb -g water
soc2oca -r 107980 -c 107696 -l 33 -d dec17/water -s dec17_420.ocadb -g water
soc2oca -r 108005 -c 107696 -l 33 -d dec17/water -s dec17_420.ocadb -g water
