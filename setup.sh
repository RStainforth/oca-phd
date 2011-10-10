#!/bin/bash

## This is really the only requirement (besides having ROOT previously set up)
## Edit to point to the base location of the package.
#
# This dependency must be removed soon, but there are just too many dependencies over getenv()
# to solve everything right away.
# The best option would be to integrate into RAT and use RATDB to load the necessary files from the database.
# For some applications the DQXXDIR environemnt should also be set.
#
export QOCA_ROOT=/home/sno/barros/optics/qoca
export DQXXDIR=/somewhere/to/dqqxxfiles

# These are just to avoid having to move to the bin directory.
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$QOCA_ROOT/lib
system=`uname -s`
if [ "$system" == "Darwin" ]; then
    export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$QOCA_ROOT/lib
fi
export PATH=$PATH:$QOCA_ROOT/bin
