#!/bin/bash
source ${RatRoot}
source ${LocasRoot}/setup.sh
cd ${LocasDataTmp}/${JobID}/${ConfigNameCore}
root -q -b ${ROOTMacroName}

