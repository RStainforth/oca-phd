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
# is for use with the example data set (SNO October 2003, RDT files) provided in
# $OCA_SNOPLUS_DATA/data/runs/rdt/oct03. Note: the conversion from RDT files to 
# SOC files through 'rdt2soc' must first be used on original SNO RDT files.
# Once this conversion has happened, the lines below assume that the SOC
# files are stored in $OCA_SNOPLUS_DATA/runs/soc/oct03

# i.e. Before running this script do the following:
#      1) Download SNO files using the instructions in $OCA_SNOPLUS_DATA/runs/rdt/oct03
#      2) Convert the RDT files to SOC files using $OCA_SNOPLUS_ROOT/scripts/rdt2soc/rdt2soc_oct03.sh

# After using this script the OCARun files will be found in $OCA_SNOPLUS_DATA/data/runs/ocarun/oct03

soc2oca -r 34764 -c 34762 -R 34764 -l 43 -d oct03 -s d2o_505.ocadb -g sno
soc2oca -r 34778 -c 34762 -R 34778 -l 43 -d oct03 -s d2o_505.ocadb -g sno
soc2oca -r 34796 -c 34762 -R 34796 -l 43 -d oct03 -s d2o_505.ocadb -g sno
soc2oca -r 34804 -c 34762 -R 34804 -l 43 -d oct03 -s d2o_505.ocadb -g sno

soc2oca -r 34918 -c 34762 -R 34918 -l 43 -d oct03 -s d2o_505.ocadb -g sno
soc2oca -r 34922 -c 34762 -R 34922 -l 43 -d oct03 -s d2o_505.ocadb -g sno
soc2oca -r 34952 -c 34762 -R 34952 -l 43 -d oct03 -s d2o_505.ocadb -g sno
soc2oca -r 34954 -c 34762 -R 34954 -l 43 -d oct03 -s d2o_505.ocadb -g sno

soc2oca -r 35008 -c 34762 -R 35008 -l 43 -d oct03 -s d2o_505.ocadb -g sno
soc2oca -r 34822 -c 34762 -R 34822 -l 43 -d oct03 -s d2o_505.ocadb -g sno
soc2oca -r 34897 -c 34762 -R 34897 -l 43 -d oct03 -s d2o_505.ocadb -g sno
soc2oca -r 34992 -c 34762 -R 34992 -l 43 -d oct03 -s d2o_505.ocadb -g sno

soc2oca -r 34964 -c 34762 -R 34964 -l 43 -d oct03 -s d2o_505.ocadb -g sno
soc2oca -r 34970 -c 34762 -R 34970 -l 43 -d oct03 -s d2o_505.ocadb -g sno
soc2oca -r 34891 -c 34762 -R 34891 -l 43 -d oct03 -s d2o_505.ocadb -g sno
soc2oca -r 34873 -c 34762 -R 34873 -l 43 -d oct03 -s d2o_505.ocadb -g sno

soc2oca -r 34865 -c 34762 -R 34865 -l 43 -d oct03 -s d2o_505.ocadb -g sno
soc2oca -r 34837 -c 34762 -R 34837 -l 43 -d oct03 -s d2o_505.ocadb -g sno
soc2oca -r 35150 -c 34762 -R 35150 -l 43 -d oct03 -s d2o_505.ocadb -g sno
soc2oca -r 35067 -c 34762 -R 35067 -l 43 -d oct03 -s d2o_505.ocadb -g sno

soc2oca -r 35132 -c 34762 -R 35132 -l 43 -d oct03 -s d2o_505.ocadb -g sno
soc2oca -r 35061 -c 34762 -R 35061 -l 43 -d oct03 -s d2o_505.ocadb -g sno
soc2oca -r 35069 -c 34762 -R 35069 -l 43 -d oct03 -s d2o_505.ocadb -g sno
soc2oca -r 35084 -c 34762 -R 35084 -l 43 -d oct03 -s d2o_505.ocadb -g sno

soc2oca -r 35193 -c 34762 -R 35193 -l 43 -d oct03 -s d2o_505.ocadb -g sno
soc2oca -r 35183 -c 34762 -R 35183 -l 43 -d oct03 -s d2o_505.ocadb -g sno
soc2oca -r 35239 -c 34762 -R 35239 -l 43 -d oct03 -s d2o_505.ocadb -g sno
soc2oca -r 35171 -c 34762 -R 35171 -l 43 -d oct03 -s d2o_505.ocadb -g sno

soc2oca -r 35201 -c 34762 -R 35201 -l 43 -d oct03 -s d2o_505.ocadb -g sno
soc2oca -r 35153 -c 34762 -R 35153 -l 43 -d oct03 -s d2o_505.ocadb -g sno
soc2oca -r 35199 -c 34762 -R 35199 -l 43 -d oct03 -s d2o_505.ocadb -g sno
soc2oca -r 35108 -c 34762 -R 35108 -l 43 -d oct03 -s d2o_505.ocadb -g sno

soc2oca -r 35191 -c 34762 -R 35191 -l 43 -d oct03 -s d2o_505.ocadb -g sno
soc2oca -r 35110 -c 34762 -R 35110 -l 43 -d oct03 -s d2o_505.ocadb -g sno
soc2oca -r 35222 -c 34762 -R 35222 -l 43 -d oct03 -s d2o_505.ocadb -g sno
soc2oca -r 35208 -c 34762 -R 35208 -l 43 -d oct03 -s d2o_505.ocadb -g sno

soc2oca -r 35243 -c 34762 -R 35243 -l 43 -d oct03 -s d2o_505.ocadb -g sno
soc2oca -r 34723 -c 34762 -R 34723 -l 43 -d oct03 -s d2o_505.ocadb -g sno
soc2oca -r 34705 -c 34762 -R 34705 -l 43 -d oct03 -s d2o_505.ocadb -g sno
soc2oca -r 34743 -c 34762 -R 34743 -l 43 -d oct03 -s d2o_505.ocadb -g sno

