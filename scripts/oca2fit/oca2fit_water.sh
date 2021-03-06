# This file is used to perform the optics fit to OCARun files
#
# Commands are of the following form:
# oca2fit -f [fit-file] -c/v -b [systematic branch]
#
# Option Descriptors:
#      
#     -f The name of the fit-file found in the $OCA_SNOPLUS_DATA/fitfiles directory. This defines the model parameters.
#     -c Calculates the PMT variability function. To do this the fit is first performed without the 
#        PMT variability error contribution to the chi-square statistic. 
#        The parameters of the function can later be used as an estimator to the PMT variability in subsequent fits
#        using the -v option. The function parameters are stored in the OCAModelParameterStore object saved
#        in the $OCA_SNOPLUS_ROOT/output/fits directory.
#     -v Include the PMT variability in the chi-square statistic for the fit to the optical model.
#     -b The name of the systematic branch to fit the model to. 'nominal' is the main fit, all others
#        are fits to difference systematic instances of the run.

# This script is for use with the example data set (SNO+ MC October 2015, SOC files) provided in
# $OCA_SNOPLUS_DATA/runs/soc/oct15/water
# i.e. Before running this script do the following:
#      1) Download SOC files using the instructions in $OCA_SNOPLUS_DATA/runs/soc/oct15/water
#      2) Convert the SOC files to OCARun files using the scripts in $OCA_SNOPLUS_ROOT/scripts/soc2oca

# After using this script the result of the fits will be found in $OCA_SNOPLUS_ROOT/output/fits

source ${OCA_SNOPLUS_ROOT}/env_oca_snoplus.sh

# Oct15, Water, 337 nm [MC]
oca2fit -f oct15_water_337.ocadb -b nominal
oca2fit -f oct15_water_337.ocadb -b laserball_r_scale
oca2fit -f oct15_water_337.ocadb -b laserball_r_shift
oca2fit -f oct15_water_337.ocadb -b laserball_minus_x_shift
oca2fit -f oct15_water_337.ocadb -b laserball_plus_x_shift
oca2fit -f oct15_water_337.ocadb -b laserball_minus_y_shift
oca2fit -f oct15_water_337.ocadb -b laserball_plus_y_shift
oca2fit -f oct15_water_337.ocadb -b laserball_minus_z_shift
oca2fit -f oct15_water_337.ocadb -b laserball_plus_z_shift
oca2fit -f oct15_water_337.ocadb -b lambda_minus_shift
oca2fit -f oct15_water_337.ocadb -b lamda_plus_shift
oca2fit -f oct15_water_337.ocadb -b laserball_distribution2
oca2fit -f oct15_water_337.ocadb -b laserball_distribution_flat
oca2fit -f oct15_water_337.ocadb -b chi_square_lim_16
oca2fit -f oct15_water_337.ocadb -b chi_square_lim_9

# Oct15, Water, 369 nm [MC]
oca2fit -f oct15_water_369.ocadb -b nominal
oca2fit -f oct15_water_369.ocadb -b laserball_r_scale
oca2fit -f oct15_water_369.ocadb -b laserball_r_shift
oca2fit -f oct15_water_369.ocadb -b laserball_minus_x_shift
oca2fit -f oct15_water_369.ocadb -b laserball_plus_x_shift
oca2fit -f oct15_water_369.ocadb -b laserball_minus_y_shift
oca2fit -f oct15_water_369.ocadb -b laserball_plus_y_shift
oca2fit -f oct15_water_369.ocadb -b laserball_minus_z_shift
oca2fit -f oct15_water_369.ocadb -b laserball_plus_z_shift
oca2fit -f oct15_water_369.ocadb -b lambda_minus_shift
oca2fit -f oct15_water_369.ocadb -b lamda_plus_shift
oca2fit -f oct15_water_369.ocadb -b laserball_distribution2
oca2fit -f oct15_water_369.ocadb -b laserball_distribution_flat
oca2fit -f oct15_water_369.ocadb -b chi_square_lim_16
oca2fit -f oct15_water_369.ocadb -b chi_square_lim_9

# Oct15, Water, 385 nm [MC]
oca2fit -f oct15_water_385.ocadb -b nominal
oca2fit -f oct15_water_385.ocadb -b laserball_r_scale
oca2fit -f oct15_water_385.ocadb -b laserball_r_shift
oca2fit -f oct15_water_385.ocadb -b laserball_minus_x_shift
oca2fit -f oct15_water_385.ocadb -b laserball_plus_x_shift
oca2fit -f oct15_water_385.ocadb -b laserball_minus_y_shift
oca2fit -f oct15_water_385.ocadb -b laserball_plus_y_shift
oca2fit -f oct15_water_385.ocadb -b laserball_minus_z_shift
oca2fit -f oct15_water_385.ocadb -b laserball_plus_z_shift
oca2fit -f oct15_water_385.ocadb -b lambda_minus_shift
oca2fit -f oct15_water_385.ocadb -b lamda_plus_shift
oca2fit -f oct15_water_385.ocadb -b laserball_distribution2
oca2fit -f oct15_water_385.ocadb -b laserball_distribution_flat
oca2fit -f oct15_water_385.ocadb -b chi_square_lim_16
oca2fit -f oct15_water_385.ocadb -b chi_square_lim_9

# Oct15, Water, 420 nm [MC]
oca2fit -f oct15_water_420.ocadb -b nominal
oca2fit -f oct15_water_420.ocadb -b laserball_r_scale
oca2fit -f oct15_water_420.ocadb -b laserball_r_shift
oca2fit -f oct15_water_420.ocadb -b laserball_minus_x_shift
oca2fit -f oct15_water_420.ocadb -b laserball_plus_x_shift
oca2fit -f oct15_water_420.ocadb -b laserball_minus_y_shift
oca2fit -f oct15_water_420.ocadb -b laserball_plus_y_shift
oca2fit -f oct15_water_420.ocadb -b laserball_minus_z_shift
oca2fit -f oct15_water_420.ocadb -b laserball_plus_z_shift
oca2fit -f oct15_water_420.ocadb -b lambda_minus_shift
oca2fit -f oct15_water_420.ocadb -b lamda_plus_shift
oca2fit -f oct15_water_420.ocadb -b laserball_distribution2
oca2fit -f oct15_water_420.ocadb -b laserball_distribution_flat
oca2fit -f oct15_water_420.ocadb -b chi_square_lim_16
oca2fit -f oct15_water_420.ocadb -b chi_square_lim_9

# Oct15, Water, 505 nm [MC]
oca2fit -f oct15_water_505.ocadb -b nominal
oca2fit -f oct15_water_505.ocadb -b laserball_r_scale
oca2fit -f oct15_water_505.ocadb -b laserball_r_shift
oca2fit -f oct15_water_505.ocadb -b laserball_minus_x_shift
oca2fit -f oct15_water_505.ocadb -b laserball_plus_x_shift
oca2fit -f oct15_water_505.ocadb -b laserball_minus_y_shift
oca2fit -f oct15_water_505.ocadb -b laserball_plus_y_shift
oca2fit -f oct15_water_505.ocadb -b laserball_minus_z_shift
oca2fit -f oct15_water_505.ocadb -b laserball_plus_z_shift
oca2fit -f oct15_water_505.ocadb -b lambda_minus_shift
oca2fit -f oct15_water_505.ocadb -b lamda_plus_shift
oca2fit -f oct15_water_505.ocadb -b laserball_distribution2
oca2fit -f oct15_water_505.ocadb -b laserball_distribution_flat
oca2fit -f oct15_water_505.ocadb -b chi_square_lim_16
oca2fit -f oct15_water_505.ocadb -b chi_square_lim_9
