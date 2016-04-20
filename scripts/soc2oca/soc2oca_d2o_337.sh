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

soc2oca -r 34772 -c 34752 -R 34764 -l 43 -d oct03 -s d2o_337.ocadb -g sno
soc2oca -r 34786 -c 34752 -R 34778 -l 43 -d oct03 -s d2o_337.ocadb -g sno
soc2oca -r 34788 -c 34752 -R 34796 -l 43 -d oct03 -s d2o_337.ocadb -g sno
soc2oca -r 34812 -c 34752 -R 34804 -l 43 -d oct03 -s d2o_337.ocadb -g sno

soc2oca -r 34907 -c 34752 -R 34918 -l 43 -d oct03 -s d2o_337.ocadb -g sno
soc2oca -r 34930 -c 34752 -R 34922 -l 43 -d oct03 -s d2o_337.ocadb -g sno
soc2oca -r 34934 -c 34752 -R 34952 -l 43 -d oct03 -s d2o_337.ocadb -g sno
soc2oca -r 35003 -c 34752 -R 35008 -l 43 -d oct03 -s d2o_337.ocadb -g sno

soc2oca -r 34814 -c 34752 -R 34822 -l 43 -d oct03 -s d2o_337.ocadb -g sno
soc2oca -r 34905 -c 34752 -R 34897 -l 43 -d oct03 -s d2o_337.ocadb -g sno
soc2oca -r 34984 -c 34752 -R 34992 -l 43 -d oct03 -s d2o_337.ocadb -g sno
soc2oca -r 34982 -c 34752 -R 34970 -l 43 -d oct03 -s d2o_337.ocadb -g sno

soc2oca -r 35019 -c 34752 -R 35010 -l 43 -d oct03 -s d2o_337.ocadb -g sno
soc2oca -r 34883 -c 34752 -R 34891 -l 43 -d oct03 -s d2o_337.ocadb -g sno
soc2oca -r 34881 -c 34752 -R 34873 -l 43 -d oct03 -s d2o_337.ocadb -g sno
soc2oca -r 34867 -c 34752 -R 34865 -l 43 -d oct03 -s d2o_337.ocadb -g sno

soc2oca -r 34830 -c 34752 -R 34837 -l 43 -d oct03 -s d2o_337.ocadb -g sno
soc2oca -r 35142 -c 34752 -R 35150 -l 43 -d oct03 -s d2o_337.ocadb -g sno
soc2oca -r 35140 -c 34752 -R 35132 -l 43 -d oct03 -s d2o_337.ocadb -g sno
soc2oca -r 35077 -c 34752 -R 35069 -l 43 -d oct03 -s d2o_337.ocadb -g sno

soc2oca -r 35098 -c 34752 -R 35084 -l 43 -d oct03 -s d2o_337.ocadb -g sno
soc2oca -r 35232 -c 34752 -R 35239 -l 43 -d oct03 -s d2o_337.ocadb -g sno
soc2oca -r 35163 -c 34752 -R 35171 -l 43 -d oct03 -s d2o_337.ocadb -g sno
soc2oca -r 35161 -c 34752 -R 35153 -l 43 -d oct03 -s d2o_337.ocadb -g sno

soc2oca -r 35100 -c 34752 -R 35108 -l 43 -d oct03 -s d2o_337.ocadb -g sno
soc2oca -r 35118 -c 34752 -R 35110 -l 43 -d oct03 -s d2o_337.ocadb -g sno
soc2oca -r 35230 -c 34752 -R 35222 -l 43 -d oct03 -s d2o_337.ocadb -g sno
soc2oca -r 34721 -c 34752 -R 34723 -l 43 -d oct03 -s d2o_337.ocadb -g sno

soc2oca -r 34713 -c 34752 -R 34705 -l 43 -d oct03 -s d2o_337.ocadb -g sno
soc2oca -r 34737 -c 34752 -R 34743 -l 43 -d oct03 -s d2o_337.ocadb -g sno
