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

# Orientation: 1, Laserball Facing North (+y direction).
soc2oca -r 10086 -c 10098 -w 10176 -d jun15MC
soc2oca -r 10087 -c 10098 -w 10176 -d jun15MC
soc2oca -r 10088 -c 10098 -w 10176 -d jun15MC
soc2oca -r 10089 -c 10098 -w 10176 -d jun15MC

soc2oca -r 10090 -c 10098 -w 10176 -d jun15MC
soc2oca -r 10091 -c 10098 -w 10176 -d jun15MC
soc2oca -r 10092 -c 10098 -w 10176 -d jun15MC
soc2oca -r 10096 -c 10098 -w 10176 -d jun15MC

# Orientation: 2, Laserball Facing West (-x direction).
soc2oca -r 10040 -c 10099 -w 10177 -d jun15MC
soc2oca -r 10041 -c 10099 -w 10177 -d jun15MC
soc2oca -r 10042 -c 10099 -w 10177 -d jun15MC
soc2oca -r 10043 -c 10099 -w 10177 -d jun15MC
