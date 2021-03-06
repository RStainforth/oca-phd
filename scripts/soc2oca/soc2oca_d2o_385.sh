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
# is for use with the example data set (SNO October 2003, RDT files) provided in
# $OCA_SNOPLUS_DATA/data/runs/rdt/oct03. Note: the conversion from RDT files to 
# SOC files through 'rdt2soc' must first be used on original SNO RDT files.
# Once this conversion has happened, the lines below assume that the SOC
# files are stored in $OCA_SNOPLUS_DATA/runs/soc/oct03

# i.e. Before running this script do the following:
#      1) Download SNO files using the instructions in $OCA_SNOPLUS_DATA/runs/rdt/oct03
#      2) Convert the RDT files to SOC files using $OCA_SNOPLUS_ROOT/scripts/rdt2soc/rdt2soc_oct03.sh

# After using this script the OCARun files will be found in $OCA_SNOPLUS_DATA/data/runs/ocarun/oct03

soc2oca -r 34768 -c 34758 -R 34764 -l 43 -d oct03 -s d2o_385.ocadb -g sno
soc2oca -r 34782 -c 34758 -R 34778 -l 43 -d oct03 -s d2o_385.ocadb -g sno
soc2oca -r 34792 -c 34758 -R 34796 -l 43 -d oct03 -s d2o_385.ocadb -g sno
soc2oca -r 34808 -c 34758 -R 34804 -l 43 -d oct03 -s d2o_385.ocadb -g sno

soc2oca -r 34914 -c 34758 -R 34918 -l 43 -d oct03 -s d2o_385.ocadb -g sno
soc2oca -r 34926 -c 34758 -R 34922 -l 43 -d oct03 -s d2o_385.ocadb -g sno
soc2oca -r 34948 -c 34758 -R 34952 -l 43 -d oct03 -s d2o_385.ocadb -g sno
soc2oca -r 34958 -c 34758 -R 34954 -l 43 -d oct03 -s d2o_385.ocadb -g sno

soc2oca -r 34818 -c 34758 -R 34822 -l 43 -d oct03 -s d2o_385.ocadb -g sno
soc2oca -r 34901 -c 34758 -R 34897 -l 43 -d oct03 -s d2o_385.ocadb -g sno
soc2oca -r 34988 -c 34758 -R 34992 -l 43 -d oct03 -s d2o_385.ocadb -g sno
soc2oca -r 34976 -c 34758 -R 34970 -l 43 -d oct03 -s d2o_385.ocadb -g sno

soc2oca -r 35015 -c 34758 -R 35010 -l 43 -d oct03 -s d2o_385.ocadb -g sno
soc2oca -r 34887 -c 34758 -R 34891 -l 43 -d oct03 -s d2o_385.ocadb -g sno
soc2oca -r 34877 -c 34758 -R 34873 -l 43 -d oct03 -s d2o_385.ocadb -g sno
soc2oca -r 34833 -c 34758 -R 34837 -l 43 -d oct03 -s d2o_385.ocadb -g sno

soc2oca -r 35146 -c 34758 -R 35150 -l 43 -d oct03 -s d2o_385.ocadb -g sno
soc2oca -r 35079 -c 34758 -R 35069 -l 43 -d oct03 -s d2o_385.ocadb -g sno
soc2oca -r 35088 -c 34758 -R 35084 -l 43 -d oct03 -s d2o_385.ocadb -g sno
soc2oca -r 35167 -c 34758 -R 35171 -l 43 -d oct03 -s d2o_385.ocadb -g sno

soc2oca -r 35157 -c 34758 -R 35153 -l 43 -d oct03 -s d2o_385.ocadb -g sno
soc2oca -r 35104 -c 34758 -R 35108 -l 43 -d oct03 -s d2o_385.ocadb -g sno
soc2oca -r 35114 -c 34758 -R 35110 -l 43 -d oct03 -s d2o_385.ocadb -g sno
soc2oca -r 35226 -c 34758 -R 35222 -l 43 -d oct03 -s d2o_385.ocadb -g sno

soc2oca -r 35247 -c 34758 -R 35243 -l 43 -d oct03 -s d2o_385.ocadb -g sno
soc2oca -r 34727 -c 34758 -R 34723 -l 43 -d oct03 -s d2o_385.ocadb -g sno
soc2oca -r 34709 -c 34758 -R 34705 -l 43 -d oct03 -s d2o_385.ocadb -g sno
soc2oca -r 34739 -c 34758 -R 34743 -l 43 -d oct03 -s d2o_385.ocadb -g sno

