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
2) Open ROOT
3) Compile LBOrientation.C

  `.L LBOrientation.C+`

4) Run `LBOrientation( const Int_t lambda = 505, const std::string& scan = "oct15", const std::string& path = getenv( "OCA_SNOPLUS_DATA" ) + (string) "/runs/soc/" )`, either inputting each of the arguments or using the defaults