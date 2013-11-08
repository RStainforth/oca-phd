#!/bin/bash
export RNDM=$(($RANDOM + $RANDOM + $RANDOM*$RANDOM))
source %{RatRoot}/env_rat-dev.sh
source %{LocasRoot}/setup.sh
cd %{LocasDataTmp}/%{JobID}/%{JobDesc}
rat -q -s ${RNDM} %{MacroName}
