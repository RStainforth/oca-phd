#!/bin/bash
source ${RatRoot}
source ${LocasRoot}/setup.sh
cd ${LocasDataTmp}/${JobID}/${ConfigName}/root_files
root -q -b ${ROOTMacroName}

