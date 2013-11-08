#!/bin/bash
source ${RatRoot}/env_rat-dev.sh
source ${LocasRoot}/setup.sh
cd ${LocasDataTmp}/${JobID}/${ConfigNameCore}
root -q -b ${ROOTMacroName}

