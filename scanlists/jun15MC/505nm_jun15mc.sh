# orientations: [ 0, PSUP-East, +X ],
# orientations: [ 1, PSUP-North, +Y ],
# orientations: [ 2, PSUP-West, -X ],
# orientations: [ 3, PSUP-South, -Y ],

# Commands are of the following forms:
# soc2oca -r 10047 -c 10059 -d jun15MC
# soc2oca [executable name]
# -r [number] : off-axis run id
# -c [number] : central run-id 
# NOTE: We do not need the '-w' option for the 505nm series of
# data because we use the position fit from the off-axis run
# by default.

# For COUMARIN-500:505nm laser source, the central laserball runs are as follows:
#
# ----------------------
# orientation   | run-id
# ----------------------
# orientation-0 | 10175
# orientation-1 | 10176
# orientation-2 | 10177
# orientation-3 | 10178
# ----------------------
#
# Source the OCA environment
source ${OCA_ROOT}/env_oca.sh

# There should be 39 runs for each wavelength in this dataset.
# NOTE: We do not need the '-w' option for the 505nm series of
# data because we use the position fit from the off-axis run
# by default.

# Orientation: 1, Laserball Facing North (+y direction).
soc2oca -r 10164 -c 10176 -d jun15MC
soc2oca -r 10165 -c 10176 -d jun15MC
soc2oca -r 10166 -c 10176 -d jun15MC
soc2oca -r 10167 -c 10176 -d jun15MC

soc2oca -r 10168 -c 10176 -d jun15MC
soc2oca -r 10169 -c 10176 -d jun15MC
soc2oca -r 10170 -c 10176 -d jun15MC
soc2oca -r 10174 -c 10176 -d jun15MC

soc2oca -r 10181 -c 10176 -d jun15MC
soc2oca -r 10182 -c 10176 -d jun15MC
soc2oca -r 10183 -c 10176 -d jun15MC
soc2oca -r 10184 -c 10176 -d jun15MC

soc2oca -r 10185 -c 10176 -d jun15MC
soc2oca -r 10186 -c 10176 -d jun15MC
soc2oca -r 10187 -c 10176 -d jun15MC
soc2oca -r 10188 -c 10176 -d jun15MC

# Orientation: 2, Laserball Facing West (-x direction).
soc2oca -r 10157 -c 10177 -d jun15MC
soc2oca -r 10158 -c 10177 -d jun15MC
soc2oca -r 10159 -c 10177 -d jun15MC
soc2oca -r 10160 -c 10177 -d jun15MC

soc2oca -r 10161 -c 10177 -d jun15MC
soc2oca -r 10162 -c 10177 -d jun15MC
soc2oca -r 10163 -c 10177 -d jun15MC
soc2oca -r 10164 -c 10177 -d jun15MC

soc2oca -r 10171 -c 10177 -d jun15MC
soc2oca -r 10172 -c 10177 -d jun15MC
soc2oca -r 10173 -c 10177 -d jun15MC
soc2oca -r 10179 -c 10177 -d jun15MC
soc2oca -r 10180 -c 10177 -d jun15MC

soc2oca -r 10189 -c 10177 -d jun15MC
soc2oca -r 10190 -c 10177 -d jun15MC
soc2oca -r 10191 -c 10177 -d jun15MC
soc2oca -r 10192 -c 10177 -d jun15MC

soc2oca -r 10193 -c 10177 -d jun15MC
soc2oca -r 10194 -c 10177 -d jun15MC
soc2oca -r 10195 -c 10177 -d jun15MC

# Central Runs

# Orientation 0
soc2oca -r 10175 -c 10175 -d jun15MC
# Orientation 1
soc2oca -r 10176 -c 10176 -d jun15MC
# Orientation 2
soc2oca -r 10177 -c 10177 -d jun15MC
# Orientation 3
soc2oca -r 10178 -c 10178 -d jun15MC
