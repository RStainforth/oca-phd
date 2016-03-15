The systematics configuration files are stored in this directory
==========

As part of the standard OCA fit routine the statistical errors are extracted from the covariance matrix. In order to calculate the systematic errors a different 'representation' of the run needs to be considered and used in the fit. For example, one such 'representation' could be where the laserball is shifted in the x-, y- or z-direction by 5 cm, or where the assumed wavelength is +/- X nm eitherside of the mean detected wavelength at the PMT (wavelength dependent). To consider these systematics, the 'soc2oca' executable needs to create an OCARun representation of the laserball run for each scenario. All these representations are then stored on different branches on the OCARun file for the respective run.


The files in this directory therefore define the systematics applied to the respective laserball scan at each wavelength to be specified when 'soc2oca' is used.

Systematics explained
==========

The applied systematics are as follows:

     laserball_r_scale - Scaling (in %) of the laserball position radius by value specified.
     laserball_r_shift - Shift outwards (in mm) of the laserball position radius by value specified.
     
     laserball_plus_x_shift - Positive laserball x-direction (in mm) shift by value specified.
     laserball_minus_x_shift - Negative laserball x-direction (in mm) shift by value specified.

     laserball_plus_y_shift - Positive laserball y-direction (in mm) shift by value specified.
     laserball_minus_y_shift - Negative laserball y-direction (in mm) shift by value specified.

     laserball_plus_z_shift - Positive laserball z-direction (in mm) shift by value specified.
     laserball_minus_z_shift - Negative laserball z-direction (in mm) shift by value specified.

     lambda_plus_shift - Positive shift in wavelength (in nm) by value specified. 
     lambda_minus_shift - Negative shift in wavelength (in nm) by value specified.
     lambda_at_pmt - The wavelength used to calculate the light path from the laserball to the PMT.
      
     lb_distribution_flat - Flat angular (not mask, angular only) laserball distribution.
     lb_distribution_squared - Squared angular (not mask, angular only) laserball distribution.

Two other systematics are applied also, but are not related to the OCARuns themselves, but rather the fitting procedure. These systematics where two different final chisquare limits are used. See the example .sh scripts in the 'oca/scripts/oca2fit' directory.