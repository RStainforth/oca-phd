# orientations: [ 0, PSUP-East, +X ],
# orientations: [ 1, PSUP-North, +Y ],
# orientations: [ 2, PSUP-West, -X ],
# orientations: [ 3, PSUP-South, -Y ],

# Commands are of the following forms:
# soc2oca -r 10047 -c 10059 -w 10176 -d jun15MC
# soc2oca [executable name]
# -r [number] : off-axis run id
# -c [number] : central run-id 
# -w [number] : wavelength run-id (for position fit)

# For N2:337nm laser source, the central laserball runs are as follows:
#
# ----------------------
# orientation   | run-id
# ----------------------
# orientation-0 | 10019
# orientation-1 | 10020
# orientation-2 | 10021
# orientation-3 | 10022
# ----------------------
#
# Source the OCA environment
source ${OCA_ROOT}/env_oca.sh

# There should be 39 runs for each wavelength in this dataset.

# Orientation: 1, Laserball Facing North (+y direction).
soc2oca -r 10008 -c 10020 -w 10164 -d jun15MC
soc2oca -r 10009 -c 10020 -w 10165 -d jun15MC
soc2oca -r 10010 -c 10020 -w 10166 -d jun15MC
soc2oca -r 10011 -c 10020 -w 10167 -d jun15MC

soc2oca -r 10012 -c 10020 -w 10168 -d jun15MC
soc2oca -r 10013 -c 10020 -w 10169 -d jun15MC
soc2oca -r 10014 -c 10020 -w 10170 -d jun15MC
soc2oca -r 10018 -c 10020 -w 10174 -d jun15MC

soc2oca -r 10025 -c 10020 -w 10181 -d jun15MC
soc2oca -r 10026 -c 10020 -w 10182 -d jun15MC
soc2oca -r 10027 -c 10020 -w 10183 -d jun15MC
soc2oca -r 10028 -c 10020 -w 10184 -d jun15MC

soc2oca -r 10029 -c 10020 -w 10185 -d jun15MC
soc2oca -r 10030 -c 10020 -w 10186 -d jun15MC
soc2oca -r 10031 -c 10020 -w 10187 -d jun15MC
soc2oca -r 10032 -c 10020 -w 10188 -d jun15MC

# Orientation: 2, Laserball Facing West (-x direction).
soc2oca -r 10001 -c 10021 -w 10157 -d jun15MC
soc2oca -r 10002 -c 10021 -w 10158 -d jun15MC
soc2oca -r 10003 -c 10021 -w 10159 -d jun15MC
soc2oca -r 10004 -c 10021 -w 10160 -d jun15MC

soc2oca -r 10005 -c 10021 -w 10161 -d jun15MC
soc2oca -r 10006 -c 10021 -w 10162 -d jun15MC
soc2oca -r 10007 -c 10021 -w 10163 -d jun15MC
soc2oca -r 10015 -c 10021 -w 10171 -d jun15MC

soc2oca -r 10016 -c 10021 -w 10172 -d jun15MC
soc2oca -r 10017 -c 10021 -w 10173 -d jun15MC
soc2oca -r 10023 -c 10021 -w 10179 -d jun15MC
soc2oca -r 10024 -c 10021 -w 10180 -d jun15MC

soc2oca -r 10033 -c 10021 -w 10189 -d jun15MC
soc2oca -r 10034 -c 10021 -w 10190 -d jun15MC
soc2oca -r 10035 -c 10021 -w 10191 -d jun15MC
soc2oca -r 10036 -c 10021 -w 10192 -d jun15MC

soc2oca -r 10037 -c 10021 -w 10193 -d jun15MC
soc2oca -r 10038 -c 10021 -w 10194 -d jun15MC
soc2oca -r 10039 -c 10021 -w 10195 -d jun15MC

# Central Runs

# Orientation 0
soc2oca -r 10019 -c 10019 -w 10175 -d jun15MC
# Orientation 1
soc2oca -r 10020 -c 10020 -w 10176 -d jun15MC
# Orientation 2
soc2oca -r 10021 -c 10021 -w 10177 -d jun15MC
# Orientation 3
soc2oca -r 10022 -c 10022 -w 10178 -d jun15MC
