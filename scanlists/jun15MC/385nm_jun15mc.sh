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

# For BBQ:385nm laser source, the central laserball runs are as follows:
#
# ----------------------
# orientation   | run-id
# ----------------------
# orientation-0 | 10097
# orientation-1 | 10098
# orientation-2 | 10099
# orientation-3 | 10100
# ----------------------
#
# Source the OCA environment
source ${OCA_ROOT}/env_oca.sh

# There should be 39 runs for each wavelength in this dataset.

# Orientation: 1, Laserball Facing North (+y direction).
soc2oca -r 10086 -c 10098 -w 10164 -d jun15MC
soc2oca -r 10087 -c 10098 -w 10165 -d jun15MC
soc2oca -r 10088 -c 10098 -w 10166 -d jun15MC
soc2oca -r 10089 -c 10098 -w 10167 -d jun15MC

soc2oca -r 10090 -c 10098 -w 10168 -d jun15MC
soc2oca -r 10091 -c 10098 -w 10169 -d jun15MC
soc2oca -r 10092 -c 10098 -w 10170 -d jun15MC
soc2oca -r 10096 -c 10098 -w 10174 -d jun15MC

soc2oca -r 10103 -c 10098 -w 10181 -d jun15MC
soc2oca -r 10104 -c 10098 -w 10182 -d jun15MC
soc2oca -r 10105 -c 10098 -w 10183 -d jun15MC
soc2oca -r 10106 -c 10098 -w 10184 -d jun15MC

soc2oca -r 10107 -c 10098 -w 10185 -d jun15MC
soc2oca -r 10108 -c 10098 -w 10186 -d jun15MC
soc2oca -r 10109 -c 10098 -w 10187 -d jun15MC
soc2oca -r 10110 -c 10098 -w 10188 -d jun15MC

# Orientation: 2, Laserball Facing West (-x direction).
soc2oca -r 10079 -c 10099 -w 10157 -d jun15MC
soc2oca -r 10080 -c 10099 -w 10158 -d jun15MC
soc2oca -r 10081 -c 10099 -w 10159 -d jun15MC
soc2oca -r 10082 -c 10099 -w 10160 -d jun15MC

soc2oca -r 10083 -c 10099 -w 10161 -d jun15MC
soc2oca -r 10084 -c 10099 -w 10162 -d jun15MC
soc2oca -r 10085 -c 10099 -w 10163 -d jun15MC
soc2oca -r 10093 -c 10099 -w 10171 -d jun15MC

soc2oca -r 10094 -c 10099 -w 10172 -d jun15MC
soc2oca -r 10095 -c 10099 -w 10173 -d jun15MC
soc2oca -r 10101 -c 10099 -w 10179 -d jun15MC
soc2oca -r 10102 -c 10099 -w 10180 -d jun15MC

soc2oca -r 10111 -c 10099 -w 10189 -d jun15MC
soc2oca -r 10112 -c 10099 -w 10190 -d jun15MC
soc2oca -r 10113 -c 10099 -w 10191 -d jun15MC
soc2oca -r 10114 -c 10099 -w 10192 -d jun15MC

soc2oca -r 10115 -c 10099 -w 10193 -d jun15MC
soc2oca -r 10116 -c 10099 -w 10194 -d jun15MC
soc2oca -r 10117 -c 10099 -w 10195 -d jun15MC

# Central Runs

# Orientation 0
soc2oca -r 10097 -c 10097 -w 10175 -d jun15MC
# Orientation 1
soc2oca -r 10098 -c 10098 -w 10176 -d jun15MC
# Orientation 2
soc2oca -r 10099 -c 10099 -w 10177 -d jun15MC
# Orientation 3
soc2oca -r 10100 -c 10100 -w 10178 -d jun15MC
