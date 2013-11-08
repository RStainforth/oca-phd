#!/bin/bash

## This is really the only requirement (besides having ROOT previously set up)
## Edit to point to the base location of the package.
#
# This dependency must be removed soon, but there are just too many dependencies over getenv()
# to solve everything right away.
# The best option would be to integrate into RAT and use RATDB to load the necessary files from the database.
# For some applications the DQXXDIR environemnt should also be set.
#
export LOCAS_ROOT=/user2/rpfs/Desktop/LOCAS/locas-plus
#export LOCAS_ROOT=/home/rpfs/Desktop/locas_step1_copy/read_soc
#export LOCAS_ROOT=/Users/rob/Desktop/read_soc

export LOCAS_DATA=/user2/rpfs/Desktop/LOCAS/locas-plus/data
#export LOCAS_DATA=/home/rpfs/Desktop/locas_step1_copy/read_soc/data
#export LOCAS_DATA=/Users/rob/Desktop/read_soc/data

export LOCAS_DATA_TMP=/scratch/stainforth

export RATENVDIR=/user2/rpfs/Desktop/LOCAS

# These are just to avoid having to move to the bin directory.
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$LOCAS_ROOT/lib
system=`uname -s`
if [ "$system" == "Darwin" ]; then
    export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$LOCAS_ROOT/lib
fi
export PATH=$PATH:$LOCAS_ROOT/bin
