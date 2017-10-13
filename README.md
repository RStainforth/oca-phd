OCA (SNO+ Edition)
==========

Note: Full OCA documentation detailed on DocDB in SNO+ Document 3706

About
==========
This is OCA for SNO+. OCA (Optical CAlibration) is designed to perform a statistical fit of laserball data to a model that characterises the optical response of the SNO+ detector. Primarily, this optical model quantifies the attenuation of the three main detector region materials; the inner AV region (water or scintillator), the acrylic of the AV and the outer AV water region. 

OCA uses laserball data in the form of SOC (SNO+ Optical Calibration) files. SOC files summarise the run-level information at each PMT from a detector run (be it with the laserball or another source) e.g. the total number of hits at the PMT in the run. This is in contrast to the standard event-by-event information found in a standard SNO+ data file. The data on a SOC file can contain laserball data either produced in RAT by MC or collected from the actual deployment of the laserball in the detector. 

Note: OCA is also compatible with laserball data collected from the SNO experiment in the form of RDT (ROOT Delta Time) files.

In addition to the run-level PMT information, the SOC files also contain information pertaining to the run itself; e.g. the run ID, the laser wavelength used, number of pulses, the intensity, the position and orientation of the laserball in the detector.

The detector optical response is a function of wavelength (amongst other variables). Therefore, an optical fit is typically performed for laserball data at a given wavelength. A single laserball run features the laserball in a given position and orientation pulsing light throughout the detector. To sufficiently probe the angular response and attenuation coefficients many laserball runs in different locations and at different orientations are required for the fit to work correctly. The collection of laserball runs at a given wavelength (typically around ~40 runs) is called a laserball 'scan'. A collection of different wavelength 'scans' forms a complete data set. In OCA, the data is organised by data sets; ~40 runs at each of 337 nm, 369 nm, 385 nm, 420 nm and 505 nm (= 40 * 5 = 200 SOC files per data set)

For SNO+ SOC files, OCA uses a two stage process to perform the optical fit:

    1. SOC2OCA: OCA converts each SOC file into an OCARun file. The OCARun file is 
                a special file format that contains further information required by the optical 
                model in the fit e.g. path lengths through the detector regions, 
                incident angles at the PMTs and initial direction vectors of light etc.
    2. OCA2FIT: OCA uses the OCARun files (each OCARun file a representation of a SOC file i.e. a 
                laserball run) to perform a statistical fit to the optical model.

For SNO data there is an additional step which is first required:

    0. RDT2SOC: Conversion of original SNO RDT files into the SNO+ SOC file format.


OCARun Files
==========
OCA interfaces with the RAT libraries in order to read SOC files. In converting the SOC files to OCARun files, the original SOC files are kept. The OCA libraries outline the data structure of the OCARun files. The OCARun files are intended to hold the relevant run specific information held by the original SOC file (run ID, laserball position, PMT information etc.) as well additional information for the individual PMTs such as the path lengths, incident angles, corrected occupancies, times of flight, solid angles and Fresnel transmission coefficients.

Prerequisites
==========
OCA requires a local installation of RAT. OCA is only compatible with RAT versions after 6.2.11.

Installation
==========
To begin you will want to clone this repository. Assuming 'git' is installed on your computer, this is done by typing the following:
   
   ```git clone -b oca-snoplus git@github.com:snoplus/oca.git```

To install OCA, in the top directory of ('oca/') type: 

    ./configure 
at the command line. The script will ask you for two pieces of information:

   1. The full system path to your RAT environment file.
   2. Your SNOLAB username (if you have one)

Note: By 'full system path' I mean no use of '~' to denote your home directory. The username is simply required to download the example data sets. These are currently stored at science1.snolab.ca. 

The configure script will create a new OCA environment file 'env_oca_snoplus.sh' in the top directory. When you source this new file, i.e.:

    source env_oca_snoplus.sh

at the command line, it will also source your RAT environment file automatically from the location you specified as part of the configure script. Now move ('cd') to the 'oca/src' directory and type the following commands:

    make clean
    make
    make install 
respectively to compile OCA and the library shared object files 'libOCA_SNO.so' and 'libOCA_SNOPLUS.so'. OCA contains the original classes used for the SNO LOCAS code (LOCAS was the SNO equivalent to this code). This allows for backwards compatibility, and these classes are in the 'libOCA_SNO.so' shared object file. The OCA class files used for SNO+ are in the 'libOCA_SNOPLUS.so' shared object file.

Finally, replace the 'rootInit.C' file provided here with the one featured in your RAT installation top directory ('rat/'). This file merely ensures that RAT and OCA libraries are loaded into ROOT if you want to use ROOT's C/C++ interpreter.

Usage
==========
OCA compiles three executables 'rdt2soc', 'soc2oca', 'oca2fit'. These are now described.

rdt2soc
==========

Laserball data from SNO remains accessible in the form of RDT files: one RDT file per run. These RDT files can be converted to the SOC file format by use of the 'rdt2soc' executable. This is a simple executable that takes two arguments:

    -r: The run ID of the RDT file. 
    -d: The directory name in $OCA_SNOPLUS_DATA/runs/rdt in which to find the RDT files.

NOTE: RDT files should use the following naming convention:

      sno_<Run-ID>_p0.rdt
      sno_<Run-ID>_p1.rdt

where ```<Run-ID>``` is the run ID of the laserball run. Some files will have both a ```_p0.rdt``` and ```_p1.rdt``` version. This denotes different passes on the data of the same run. In such cases the ```_p1.rdt``` version is used by rdt2soc to convert the data. For an example of rdt2soc in action see the example script in the $OCA_SNOPLUS_ROOT/scripts/rdt2soc directory.

soc2oca
==========

The optical fit performs a chi-square test between an observed and model-predicted variable; the occupancy ratio. For a single PMT, the occupancy ratio is the ratio of the occupancy from an off-axis run divided by the occupancy observed at the same PMT from a central run. Therefore, to produce an OCARun file, soc2oca requires the run IDs of the off-axis (-r) and central (-c) runs. In general, the laserball position is best determined from laserball runs from the same off-axis position, but at longer wavelengths. Hence, the 'wavelength' (-w) run ID is required also to retrieve the best estimate of the fitted laserball position.

NOTE: It is assumed the user knows that the position in the off-axis (-r) run file and the wavelength (-w) file are the same.

The approach is as follows:

    main-run file +             ----> soc2oca ----> OCARun File
    central-run file +          ----> ^
    wavelength-run file +       ----> ^
    central wavelength-run file ----> ^

The OCARun file contains all the required PMT information and corrections required for the fit for the main-run file ONLY.

Commands are of the following form:

    soc2oca -r [run-id] -c [c-run-id]
            -R [wavelength-run-id] -C [wavelength-central-run-id]
            -l [laserball-position-code]
            -d [MMYY-material-directory]
            -s [systematic-file-path]
            -g [geometry-option]

Option Descriptors:
Note: When specfying runs, 'soc2oca' searches in $OCA_SNOPLUS_DATA/runs/soc/[MMYY-material-directory]

     -r The run ID of the off-axis laserball run SOC file of form "<run-id>_Run.root"
     -c The run ID of the central laserball run SOC file of form "<c-run-id>_Run.root"
     -R The run ID of the off-axis laserball run SOC file from the wavelength run of form "<wavelength-run-id>_Run.root"
     -C The run ID of the central laserball run SOC file from the wavelength run of form "<wavelength-central-run-id>_Run.root"
     -l The laserball position to be used. Option is of form 'XY' X: off-axis run, Y: central run. Several options are available:
                   X : Off-Axis run laserball position.
                     X = 1 : Off-Axis manipulator laserball position 
                     X = 2 : Off-Axis camera laserball position [placeholder, not currently in]
                     X = 3 : Off-Axis fitted laserball position
                     X = 4 : Off-Axis fitted laserball position from wavelength run (-R option)

                   Y : Central run laserball position.
                     Y = 1 : Central manipulator laserball position
                     Y = 2 : Central camera laserball position [placeholder, not currently in]
                     Y = 3 : Central fitted laserball position
                     Y = 4 : Central fitted laserball position from wavelength run (-C option)

     -d The name of the directory (usually ordered by date) in $OCA_SNOPLUS_ROOT/data/runs/soc from where to find the SOC files to be converted.
     -s The name of the systematic file (from which the systematic variations are applied) found in $OCA_SNOPLUS_DATA/systematics
     -g The geometry option. Throughout SNO and SNO+ the PMT positions or orientations may have changed. Therefore, this needs to be specfied 
        such that the correct detector state is used. In addition, calculation of shadowing requires knowledge of the detector geometry. 
        For example, in SNO, there were no AV hold-down ropes but in SNO+ there are. The geometry option also determines the inner AV 
        material such that the correct group velocties are used. The current options are as follows:
            sno - The SNO detector geometry (no AV hold-down ropes)
            water - SNO+ detector with inner AV filled with water
            labppo - SNO+ detector with inner AV filled with LABPPO
            labppote0p3perylene - SNO+ detector with inner AV filled with LABPPO+0.3%Te+Perylene
            labppote0p3bismsb - SNO+ detector with inner AV filled with LABPPO+0.3%Te+Bis-MSB

Example Usage (at command line):

     soc2oca -r 236901 -c 2369039 -R 250501 -C 2505039 -l 44 -d oct15/water -s water_369.ocadb -g water

The above takes the SOC run (236901)[-r] from ${OCA_SNOPLUS_DATA}/data/runs/(oct15/water)[-d] and normalises it against the central run (2369039)[-c].

The positions used for laserball in both cases is the fitted one from their respective wavelength run counterparts (44)[-l]; (250501)[-R] and (2505039)[-C] runs respectively. Systematics are added in separate branches on the OCARun ROOT tree as declared in water_369.ocadb[-s]. Light path information will use the group velocities of a water filled SNO+ detector[-g].

soc2oca will then output a file "236901_OCARun.root" to ```${OCA_SNOPLUS_DATA}/data/runs/ocarun/oct15/water``` 

Currently BOTH a main-run and central-run file is required. The wavelength run files (off-axis and central) are optional.

For an example of soc2oca in action see the example script in the $OCA_SNOPLUS_ROOT/scripts/soc2oca directory. 

oca2fit
==========

The 'oca2fit' executable is responsible for processing the necessary OCARun files for a given wavelength scan and implementing them into a fit of the optical response of the detector by using the Levenberg-Marquardt algorithm. To perform the fit, all the parameters and cut crieria are specified by a 'fitfile' which are found in the $OCA_SNOPLUS_DATA/fitfiles directory.

Commands are of the following form:

         oca2fit -f [fit-file] -c/v -b [systematic branch]

Option Descriptors:
      
     -f The name of the fit-file found in the $OCA_SNOPLUS_DATA/fitfiles directory. This defines the model parameters.
     -c Calculates the PMT variability function. To do this the fit is first performed without the 
        PMT variability error contribution to the chi-square statistic. 
        The parameters of the function can later be used as an estimator to the PMT variability in subsequent fits
        using the -v option. The function parameters are stored in the OCAModelParameterStore object saved
        in the $OCA_SNOPLUS_ROOT/output/fits directory.
     -v Include the PMT variability in the chi-square statistic for the fit to the optical model.
     -b The name of the systematic branch to fit the model to. 'nominal' is the main fit, all others
        are fits to difference systematic instances of the run.

After oca2fit has finished the fits will be found in $OCA_SNOPLUS_ROOT/output/fits. The fit must be repeated for different systematic instances of the OCARun files. For an example of oca2fit in action see the example script in the $OCA_SNOPLUS_ROOT/scripts/oca2fit directory.


