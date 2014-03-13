High-Stat Tool
==========
This is a remporary solution to producing large SOC files. The script 'EventGen.sh' is used as follows at the command line:

     ./EventGen mac/example_soc.mac 10 10000 123456

where 'example_soc.mac' is the name of the RAT macro file located in the 'mac' sub-directory. The second argument is the number of jobs to be submitted on the local batch system, the thirs argument being the number of events per job and the fourth argument the run ID to be associated with the final SOC file. 

NOTES
==========
The user should create their RAT macro in-line with the 'example_soc.mac' macro file. i.e. the name of the SOC file and the number of events should not be specified directly in the RAT macro file (they are parsed by ProduceJobScripts.py) from the command line (above).
