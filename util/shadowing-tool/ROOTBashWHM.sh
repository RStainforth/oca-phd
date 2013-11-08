#!/bin/bash
source ${RatRoot}/env_rat-dev.sh
source ${LocasRoot}/setup.sh
cd ${LocasDataTmp}/${JobID}/${ConfigName}/root_files
root -q -b ${ROOTMacroName}

