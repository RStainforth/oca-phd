#!/bin/bash
export RNDM=$(($RANDOM + $RANDOM + $RANDOM*$RANDOM))
source %{RatRoot}
source %{LocasRoot}/setup.sh
cd %{LocasDataTmp}/%{JobID}/%{JobDesc}
rat -q -s ${RNDM} %{MacroName}
