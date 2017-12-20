Laserball Asymmetry Analysis
=====================

The Laserball Asymmetry Analysis characterizes the angular distribution of the laserball using central runs with different orientations. A comprehensive description of the analysis and results using MC data can be found in DocDB [3952](https://www.snolab.ca/snoplus/private/DocDB/cgi/ShowDocument?docid=3952).

Files
---------------------

* LBOrientation.C
* LBOrientation.h
* runlist.txt - contains the list of flipped runs for each wavelenght and laserball scan. It has the format `DataSet Phase  WL #Runs RunID1 RunID2 RunID3 RunID4`

How To Run
--------------------

1) Source env_oca_snoplus.sh
2) Open root
3) Compile LBOrientation.C

  `.L LBOrientation.C+`

4) Run either:
  - `LBOrientation()`, that performs the analysis for the default wavelength of 505 nm and uses the default path to the oct15 MC soc files in `OCA_SNOPLUS_DATA/runs/soc/oct15/water`
  - `LBOrientation( <wavelength> )`, that performs the analysis for a user inputted wavelength and uses the default path to the oct15 MC soc files in `OCA_SNOPLUS_DATA/runs/soc/oct15/water`
  - `LBOrientation( <wavelength>, "<path_to_files>" )`, that performs the analysis for a user inputted wavelength and path to the soc files.
