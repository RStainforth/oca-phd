OCA (SNO+ Edition)
==========

About
==========
This is OCA for SNO+. OCA (Optical CAlibration) is designed to perform a statistical fit of laserball data to a model that characterises the optical response of the SNO+ detector. Primarily, this optical model quantifies the attenuation of the three main detector region materials; the inner AV region (water or scintillator), the acrylic of the AV and the outer water AV region. 

OCA uses laserball data in the form of SOC (SNO+ Optical Calibration) files. SOC files summarise the run-level information at each PMT from a detector run (be it with the laserball or another source) e.g. the total number of hits at the PMT in the run. This is in contrast to the standard event-by-event information found in a standard SNO+ data file. The data on a SOC file can contain laserball data either produced in RAT by MC or collected from the actual deployment of the laserball in the detector. 

Note: OCA is also compatible with laserball data collected from the SNO experiment in the form of RDT (ROOT Delta Time) files.

In addition to the run-level PMT information, the SOC files also contain information pertaining to the run itself; e.g. the run ID, the laser wavelength used, number of pulses, the intensity, the position and orientation of the laserball in the detector.

The detector optical response is a function of wavelength (amongst other variables). Therefore, an optical fit is typically performed for laserball data at a given wavelength. A single laserball run features the laserball in a given position and orientation pulsing light throughout the detector. To sufficiently probe the angular response and attenuation coefficients many laserball runs in different locations and at different orientations are required for the fit to work correctly. The collection of laserball runs at a given wavelength (typically around ~40 runs) is called a laserball 'scan'. A collection of different wavelength 'scans' forms a complete data set. In OCA, the data is organised by data sets; 40 runs at each of 337 nm, 369 nm, 385 nm, 420 nm and 505 nm (= 40 * 5 = 200 SOC files!)

For SNO+ SOC files, OCA uses a two stage process to perform the optical fit:

    1) SOC2OCA: OCA converts each SOC file into an OCARun file. The OCARun file is a special file format that contains further information required by the optical model in the fit e.g. path lengths through the detector regions, incident angles at the PMTs, initial direction vectors of light etc.
    2) OCA2FIT: OCA uses the OCARun files (each OCARun file a representation of a SOC file i.e. a laserball run) to perform a statistical fit to the optical model.

For SNO data there is an additional step which is first required:

    0) RDT2SOC: Conversion of original SNO RDT files into the SNO+ SOC file format.


OCARun Files
==========
OCA interfaces with the RAT libraries in order to read SOC files. In converting the SOC files to OCARun files, the original SOC files are kept. The OCA libraries outline the data structure of the OCARun files. The OCARun files are intended to hold the relevant run specific information held by the original SOC file (run ID, laserball position, PMT information etc.) as well additional information for the individual PMTs such as the path lengths, incident angles, corrected occupancies, times of flight, solid angles and Fresnel transmission coefficients.

Prerequisites
==========
OCA requires a local installation of RAT. The most recent version of RAT for which OCA has found to be compatible is RAT 5.3.1

Installation
==========
To begin you will want to clone this repository. Assuming 'git' is installed on your computer, this is done by typing the following:
   
   ```git clone -b oca-snoplus git@github.com:snoplus/oca.git```

To install OCA, in the top directory of ('oca/') type: 

    ./configure 
at the command line. The script will ask you for two pieces of information:

   1) The full system path to your RAT environment file.
   2) The full system path to a directory you would like OCA to temporarily store data.

Note: By 'full system path' I mean no use of '~' to denote your home directory. 

The configure script will create a new OCA environment file 'env_oca_snoplus.sh' in the top directory. When you source this new file, i.e.:

    source env_oca_snoplus.sh

at the command line, it will also source your RAT environment file automatically from the location you specified as part of the configure script. Now move ('cd') to the 'oca-plus/src' directory and type the following commands:

    make clean
    make
    make install 
respectively to compile OCA and the library shared object file 'libOCA_SNO.so' and 'libOCA_SNOPLUS.so'. OCA contains the original classes used for the SNO LOCAS code. This allows for backwards compatibility, and these classes are in the 'libOCA_SNO.so' shared object file. The OCA class files used for SNO+ are in the 'libOCA_SNOPLUS.so' shared object file.

Finally, replace the 'rootInit.C' file provided here with the one featured in your RAT installation top directory ('rat/'). This file merely ensures that RAT and OCA libraries are loaded into ROOT if you want to use ROOT's C/C++ interpreter.

Usage
==========
OCA compiles three executables 'rdt2soc', 'soc2oca', 'oca2fit'. These are now described.

rdt2soc
==========
*To be written*

soc2oca
==========

The optical fit performs a chi-square test between an observed and model-predicted variable; the occupancy ratio. For a single PMT, the occupancy ratio is the ratio of the occupancy from an off-axis run divided by the occupancy observed at the same PMT from a central run. Therefore, to produce an OCARun file, soc2oca requires the run IDs of the off-axis (-r) and central (-c) runs. In general, the laserball position is best determined from laserball runs from the same off-axis positio, but at longer wavelengths. Hence, the 'wavelength' (-w) run ID is required also to retrieve the best estimate of the laserball position.

NOTE: It is assumed the user knows that the position in the off-axis (-r) run file and the wavelength (-w) file are the same.

Example usage at the command line: 

    soc2oca -r 123456 -c 654321 -w 987654

To summarise: The '-r' argument is the off-axis SOC file to be processed ("123456_Run.root"), '-c' is the associated central-run SOC file ("654321_Run.root") and '-w' an optional associated run at a different wavelength (in SNO, this was usually at 500 nm).

'soc2oca' writes a OCARun file, "123456_OCARun.root" to the 'oca-plus/data/runs/ocarun' directory.

oca2fit
==========
The 'oca2marquardt' and 'oca2minuit' executables are responsible for processing the OCARun files and implementing them into a fit of the optical response of the detector. either by using the Levenberg-Marquardt algorithm (a-la SNO) or by using ROOT's implementation of Minuit. To perform the fit, all the parameters and cut crieria are specified by a 'fitfile' which are found in 'oca-plus/data/fitfiles'. The format of the fitfiles is currently different depending on whether 'oca2mardquardt' or 'oca2minuit' is being used. For information on the format of these fitfiles, see the README.md file in the 'oca-plus/data/fitfiles' directory.

Example usage of these fitting executables at the command line is as follows:

    oca2marquardt /path/to/marquardt_fit_file.ratdb
    oca2minuit /path/to/minuit_fit_file.ratdb

Note: This executable is currently in development, if you are reading this note then it is not quite yet complete and it is likely you don't have appropriate data to fit to.

OCA Utilities
==========
The 'oca-plus/util/' directory stores various scripts which could be useful for OCA. See the individual README.md files in each of the aforementioned subdirectories for more information.


NOTES
==========

1) Currently, by default OCA will search for SOC files in the 'oca-plus/data/runs/soc' directory. The SOC files should be named by run ID, i.e. for the laserball run with run id '123456', the associated SOC file should be named '123456_Run.root' and located in the 'oca-plus/data/runs/soc' directory. Once processed, OCA will output the OCARun files to 'oca-plus/data/runs/ocarun' and will write filenames of the form '123456_OCARun.root'.

