#!/bin/tcsh

## This is really the only requirement (besides having ROOT previously set up)
## Edit to point to the base location of the package.
#
#
#
#
# This dependency must be removed soon, but there are just too many dependencies over getenv()
# to solve everything right away.
# The best option would be to integrate into RAT and use RATDB to load the necessary files from the database.
# For some applications the DQXXDIR environemnt should also be set.
#
setenv QOCA_ROOT /home/sno/barros/optics/qoca
# This directory is what is used if "default" is set in the QOCA options file for
# the location of the SQXX banks
setenv DQXXDIR /lustre/lip.pt/data/sno/barros/snodb/hepdb/titles

# These are just to avoid having to move to the bin directory.
setenv LD_LIBRARY_PATH "${LD_LIBRARY_PATH}:${QOCA_ROOT}/lib"
set system=`uname -s`

if ( "$system" == "Darwin" ) then
    setenv DYLD_LIBRARY_PATH "${DYLD_LIBRARY_PATH}:${QOCA_ROOT}/lib"
endif
setenv PATH "${PATH}:${QOCA_ROOT}/bin"
