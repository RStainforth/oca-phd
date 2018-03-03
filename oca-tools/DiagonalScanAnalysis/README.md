Diagonal Scan Analysis
=====================

The Diagonal Scan Analysis characterizes the attenuation coefficients of the medium inside the acrylic vessel (water or scintillator) using runs with the laserball along one of the detector diagonals: x=z, x=-z, y=z or y=-z. A comprehensive description of the analysis and results using MC data can be found in DocDB [3749](https://www.snolab.ca/snoplus/private/DocDB/cgi/ShowDocument?docid=3749).

Files
---------------------

* DiagScan.cxx
* DiagScan.h
* runlist.txt - contains the list of runs to be used for each wavelenght, laserball scan and diagonal. It has the format `Scan WL Diagonal #Runs RunID1 RunID2 RunID3 RunID4 RunID5 RunID6 ...`
* runDiagScan.cxx - example code on how to run the Diagonal Scan Analysis for all wavelengths and how to costumize variable parameters such as the distance cuts.

How To Run
--------------------

1) Source env_oca_snoplus.sh
2) Open root
3) Compile DiagScan.cxx

  `.L DiagScan.cxx+`

4) Run `DiagScan( const Int_t lambda = 505, const std::string& diagonal = "xpz", const std::string& scan = "oct15", const std::string& path = getenv( "OCA_SNOPLUS_DATA" ) + (string) "/runs/soc/" )`, either inputting each of the arguments or using the defaults

or

Compile runDiagScan.cxx

`.L runDiagScan.cxx+`

and run `runDiagonalScanAnalysis()`
