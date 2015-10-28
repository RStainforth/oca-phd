The Fit Configuration File are Stored in this Directory
==========

These files are used by the 'oca2fit' executable to perform the optics fit.

oca2fit fitfile
==========

When fitting data, the options for the fit are provided in the fit files contained here. The entries on the fit file are as follows:

     fit_name - Name of the Fit
     fit_title - Title of the FIt
     
     runs_ids - A list of the run IDs to include in the fit

     lb_dist_n_theta_bins - Number of Theta bins for the Laserball distribution
     lb_dist_n_phi_bins - Number of Phi bins for the laserball distribution

     ang_resp_n_bins - Number of angular response bins (0 - 90 degrees)

     lb_dist_min_n_pmts - Minimum number of entries per laserball distribution bin
     ang_resp_min_n_pmts - Minimum number of entires per angular response distribution bin

     cut_chisq_max - Initial maximum value of single PMT chisquare to cut on
     cut_chisq_min - Initial minimum value of single PMT chisquare to cut on

     cut_n_sigma - Number of sigma PMT must be within mean occupancy for it's respective run
     cut_n_chisquare - Initial number of total reduced chisquare PMT must be within

     cut_n_occupancy - Minimum occupancy PMT must have in both off-axis and central runs

     cut_avhd_sh_min - Minimum relative shadowing value for PMT from AVHD ropes
     cut_avhd_sh_max - Maximum relative shadowing value for PMT from AVHD ropes

     cut_geo_sh_min - Minimum relative shadowing value for PMT from enveloping detector geometry around AV
     cut_geo_sh_max - Maximum relative shadowing value for PMT from enveloping detector geometry around AV

     cut_chs_flag - Value of CHS flag to cut on (0 or 1)
     cut_css_flag - Value of CSS flag to cut on (0 or 1)

     n_pmts_skip - Number of PMTs to skip over in final data set to speed up minimisation

     scint_vary - Whether the scintillator region material attenuation length should vary in the fit (0 or 1)
     scint_init - Initial value for (1/attenuation) length [1/mm]

     av_vary - Whether the AV region material attenuation length should vary in the fit (0 or 1)
     av_init - Initial value for (1/attenuation) length [1/mm]

     water_vary - Whether the water region material attenuation length should vary in the fit (0 or 1)
     water_init - Initial value for (1/attenuation) length [1/mm]

     scint_rs_vary - Whether the scintillator region material Rayleigh scattering length should vary in the fit (0 or 1)
     scint_rs_init - Initial value for (1/Rayleigh scattering) length [1/mm]

     av_rs_vary - Whether the AV region material Rayleigh scattering length should vary in the fit (0 or 1)
     av_rs_init - Initial value for (1/Rayleigh scattering) length [1/mm]

     water_rs_vary - Whether the water region material Rayleigh scattering length should vary in the fit (0 or 1)
     water_rs_init - Initial value for (1/Rayleigh scattering) length [1/mm]

     lb_dist_vary - Whether the laserball distribution should vary in the fit
     lb_dist_init - The inital values for the laserball distribution bins

     ang_resp_vary - Whether the angular response distribution should vary in the fit
     ang_resp_init - The initial values for the angular response distribution bins

     run_norm_vary - Whether the individual run normalisations should in the fit
     run_norm_init - The initial values for all normalisations

