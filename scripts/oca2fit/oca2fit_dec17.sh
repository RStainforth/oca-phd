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

# This script is for use with the SNO+ December 2017 data set

# After using this script the result of the fits will be found in $OCA_SNOPLUS_ROOT/output/fits

source ${OCA_SNOPLUS_ROOT}/env_oca_snoplus.sh

# Dec17, Water, 337 nm
oca2fit -f dec17_water_337.ocadb -c -b nominal
oca2fit -f dec17_water_337.ocadb -v -b nominal
oca2fit -f dec17_water_337.ocadb -v -b laserball_r_scale
oca2fit -f dec17_water_337.ocadb -v -b laserball_r_shift
oca2fit -f dec17_water_337.ocadb -v -b laserball_minus_x_shift
oca2fit -f dec17_water_337.ocadb -v -b laserball_plus_x_shift
oca2fit -f dec17_water_337.ocadb -v -b laserball_minus_y_shift
oca2fit -f dec17_water_337.ocadb -v -b laserball_plus_y_shift
oca2fit -f dec17_water_337.ocadb -v -b laserball_minus_z_shift
oca2fit -f dec17_water_337.ocadb -v -b laserball_plus_z_shift
oca2fit -f dec17_water_337.ocadb -v -b lambda_minus_shift
oca2fit -f dec17_water_337.ocadb -v -b lambda_plus_shift
oca2fit -f dec17_water_337.ocadb -v -b laserball_distribution2
oca2fit -f dec17_water_337.ocadb -v -b laserball_distribution_flat
oca2fit -f dec17_water_337.ocadb -v -b chi_square_lim_16
oca2fit -f dec17_water_337.ocadb -v -b chi_square_lim_9
oca2fit -f dec17_water_337.ocadb -v -b distance_to_pmt
oca2fit -f dec17_water_337.ocadb -b pmt_to_pmt_variability

# Dec17, Water, 365 nm
oca2fit -f dec17_water_365.ocadb -c -b nominal
oca2fit -f dec17_water_365.ocadb -v -b nominal
oca2fit -f dec17_water_365.ocadb -v -b laserball_r_scale
oca2fit -f dec17_water_365.ocadb -v -b laserball_r_shift
oca2fit -f dec17_water_365.ocadb -v -b laserball_minus_x_shift
oca2fit -f dec17_water_365.ocadb -v -b laserball_plus_x_shift
oca2fit -f dec17_water_365.ocadb -v -b laserball_minus_y_shift
oca2fit -f dec17_water_365.ocadb -v -b laserball_plus_y_shift
oca2fit -f dec17_water_365.ocadb -v -b laserball_minus_z_shift
oca2fit -f dec17_water_365.ocadb -v -b laserball_plus_z_shift
oca2fit -f dec17_water_365.ocadb -v -b lambda_minus_shift
oca2fit -f dec17_water_365.ocadb -v -b lambda_plus_shift
oca2fit -f dec17_water_365.ocadb -v -b laserball_distribution2
oca2fit -f dec17_water_365.ocadb -v -b laserball_distribution_flat
oca2fit -f dec17_water_365.ocadb -v -b chi_square_lim_16
oca2fit -f dec17_water_365.ocadb -v -b chi_square_lim_9
oca2fit -f dec17_water_365.ocadb -v -b distance_to_pmt
oca2fit -f dec17_water_365.ocadb -b pmt_to_pmt_variability

# Dec17, Water, 385 nm
oca2fit -f dec17_water_385.ocadb -c -b nominal
oca2fit -f dec17_water_385.ocadb -v -b nominal
oca2fit -f dec17_water_385.ocadb -v -b laserball_r_scale
oca2fit -f dec17_water_385.ocadb -v -b laserball_r_shift
oca2fit -f dec17_water_385.ocadb -v -b laserball_minus_x_shift
oca2fit -f dec17_water_385.ocadb -v -b laserball_plus_x_shift
oca2fit -f dec17_water_385.ocadb -v -b laserball_minus_y_shift
oca2fit -f dec17_water_385.ocadb -v -b laserball_plus_y_shift
oca2fit -f dec17_water_385.ocadb -v -b laserball_minus_z_shift
oca2fit -f dec17_water_385.ocadb -v -b laserball_plus_z_shift
oca2fit -f dec17_water_385.ocadb -v -b lambda_minus_shift
oca2fit -f dec17_water_385.ocadb -v -b lambda_plus_shift
oca2fit -f dec17_water_385.ocadb -v -b laserball_distribution2
oca2fit -f dec17_water_385.ocadb -v -b laserball_distribution_flat
oca2fit -f dec17_water_385.ocadb -v -b chi_square_lim_16
oca2fit -f dec17_water_385.ocadb -v -b chi_square_lim_9
oca2fit -f dec17_water_385.ocadb -v -b distance_to_pmt
oca2fit -f dec17_water_385.ocadb -b pmt_to_pmt_variability

# Dec17, Water, 420 nm
oca2fit -f dec17_water_420.ocadb -c -b nominal
oca2fit -f dec17_water_420.ocadb -v -b nominal
oca2fit -f dec17_water_420.ocadb -v -b laserball_r_scale
oca2fit -f dec17_water_420.ocadb -v -b laserball_r_shift
oca2fit -f dec17_water_420.ocadb -v -b laserball_minus_x_shift
oca2fit -f dec17_water_420.ocadb -v -b laserball_plus_x_shift
oca2fit -f dec17_water_420.ocadb -v -b laserball_minus_y_shift
oca2fit -f dec17_water_420.ocadb -v -b laserball_plus_y_shift
oca2fit -f dec17_water_420.ocadb -v -b laserball_minus_z_shift
oca2fit -f dec17_water_420.ocadb -v -b laserball_plus_z_shift
oca2fit -f dec17_water_420.ocadb -v -b lambda_minus_shift
oca2fit -f dec17_water_420.ocadb -v -b lambda_plus_shift
oca2fit -f dec17_water_420.ocadb -v -b laserball_distribution2
oca2fit -f dec17_water_420.ocadb -v -b laserball_distribution_flat
oca2fit -f dec17_water_420.ocadb -v -b chi_square_lim_16
oca2fit -f dec17_water_420.ocadb -v -b chi_square_lim_9
oca2fit -f dec17_water_420.ocadb -v -b distance_to_pmt
oca2fit -f dec17_water_420.ocadb -b pmt_to_pmt_variability

# Dec17, Water, 450 nm
oca2fit -f dec17_water_450.ocadb -c -b nominal
oca2fit -f dec17_water_450.ocadb -v -b nominal
oca2fit -f dec17_water_450.ocadb -v -b laserball_r_scale
oca2fit -f dec17_water_450.ocadb -v -b laserball_r_shift
oca2fit -f dec17_water_450.ocadb -v -b laserball_minus_x_shift
oca2fit -f dec17_water_450.ocadb -v -b laserball_plus_x_shift
oca2fit -f dec17_water_450.ocadb -v -b laserball_minus_y_shift
oca2fit -f dec17_water_450.ocadb -v -b laserball_plus_y_shift
oca2fit -f dec17_water_450.ocadb -v -b laserball_minus_z_shift
oca2fit -f dec17_water_450.ocadb -v -b laserball_plus_z_shift
oca2fit -f dec17_water_450.ocadb -v -b lambda_minus_shift
oca2fit -f dec17_water_450.ocadb -v -b lambda_plus_shift
oca2fit -f dec17_water_450.ocadb -v -b laserball_distribution2
oca2fit -f dec17_water_450.ocadb -v -b laserball_distribution_flat
oca2fit -f dec17_water_450.ocadb -v -b chi_square_lim_16
oca2fit -f dec17_water_450.ocadb -v -b chi_square_lim_9
oca2fit -f dec17_water_450.ocadb -v -b distance_to_pmt
oca2fit -f dec17_water_450.ocadb -b pmt_to_pmt_variability

# Dec17, Water, 500 nm
oca2fit -f dec17_water_500.ocadb -c -b nominal
oca2fit -f dec17_water_500.ocadb -v -b nominal
oca2fit -f dec17_water_500.ocadb -v -b laserball_r_scale
oca2fit -f dec17_water_500.ocadb -v -b laserball_r_shift
oca2fit -f dec17_water_500.ocadb -v -b laserball_minus_x_shift
oca2fit -f dec17_water_500.ocadb -v -b laserball_plus_x_shift
oca2fit -f dec17_water_500.ocadb -v -b laserball_minus_y_shift
oca2fit -f dec17_water_500.ocadb -v -b laserball_plus_y_shift
oca2fit -f dec17_water_500.ocadb -v -b laserball_minus_z_shift
oca2fit -f dec17_water_500.ocadb -v -b laserball_plus_z_shift
oca2fit -f dec17_water_500.ocadb -v -b lambda_minus_shift
oca2fit -f dec17_water_500.ocadb -v -b lambda_plus_shift
oca2fit -f dec17_water_500.ocadb -v -b laserball_distribution2
oca2fit -f dec17_water_500.ocadb -v -b laserball_distribution_flat
oca2fit -f dec17_water_500.ocadb -v -b chi_square_lim_16
oca2fit -f dec17_water_500.ocadb -v -b chi_square_lim_9
oca2fit -f dec17_water_500.ocadb -v -b distance_to_pmt
oca2fit -f dec17_water_500.ocadb -b pmt_to_pmt_variability
