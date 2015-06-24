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

# For PBD:369nm laser source, the central laserball runs are as follows:
#
# ----------------------
# orientation   | run-id
# ----------------------
# orientation-0 | 10058
# orientation-1 | 10059
# orientation-2 | 10060
# orientation-3 | 10061
# ----------------------
#
# Source the OCA environment
source ${OCA_ROOT}/env_oca.sh

# There should be 39 runs for each wavelength in this dataset.

# Orientation: 1, Laserball Facing North (+y direction).
soc2oca -r 10047 -c 10059 -w 10164 -d jun15MC
soc2oca -r 10048 -c 10059 -w 10165 -d jun15MC
soc2oca -r 10049 -c 10059 -w 10166 -d jun15MC
soc2oca -r 10050 -c 10059 -w 10167 -d jun15MC

soc2oca -r 10051 -c 10059 -w 10168 -d jun15MC
soc2oca -r 10052 -c 10059 -w 10169 -d jun15MC
soc2oca -r 10053 -c 10059 -w 10170 -d jun15MC
soc2oca -r 10057 -c 10059 -w 10174 -d jun15MC

soc2oca -r 10064 -c 10059 -w 10181 -d jun15MC
soc2oca -r 10065 -c 10059 -w 10182 -d jun15MC
soc2oca -r 10066 -c 10059 -w 10183 -d jun15MC
soc2oca -r 10067 -c 10059 -w 10184 -d jun15MC

soc2oca -r 10068 -c 10059 -w 10185 -d jun15MC
soc2oca -r 10069 -c 10059 -w 10186 -d jun15MC
soc2oca -r 10070 -c 10059 -w 10187 -d jun15MC
soc2oca -r 10071 -c 10059 -w 10188 -d jun15MC

# Orientation: 2, Laserball Facing West (-x direction).
soc2oca -r 10040 -c 10060 -w 10157 -d jun15MC
soc2oca -r 10041 -c 10060 -w 10158 -d jun15MC
soc2oca -r 10042 -c 10060 -w 10159 -d jun15MC
soc2oca -r 10043 -c 10060 -w 10160 -d jun15MC

soc2oca -r 10044 -c 10060 -w 10161 -d jun15MC
soc2oca -r 10045 -c 10060 -w 10162 -d jun15MC
soc2oca -r 10046 -c 10060 -w 10163 -d jun15MC
soc2oca -r 10054 -c 10060 -w 10171 -d jun15MC

soc2oca -r 10055 -c 10060 -w 10172 -d jun15MC
soc2oca -r 10056 -c 10060 -w 10173 -d jun15MC
soc2oca -r 10062 -c 10060 -w 10179 -d jun15MC
soc2oca -r 10063 -c 10060 -w 10180 -d jun15MC

soc2oca -r 10072 -c 10060 -w 10189 -d jun15MC
soc2oca -r 10073 -c 10060 -w 10190 -d jun15MC
soc2oca -r 10074 -c 10060 -w 10191 -d jun15MC
soc2oca -r 10075 -c 10060 -w 10192 -d jun15MC

soc2oca -r 10076 -c 10060 -w 10193 -d jun15MC
soc2oca -r 10077 -c 10060 -w 10194 -d jun15MC
soc2oca -r 10078 -c 10060 -w 10195 -d jun15MC

# Central Runs

# Orientation 0
soc2oca -r 10058 -c 10058 -w 10175 -d jun15MC
# Orientation 1
soc2oca -r 10059 -c 10059 -w 10176 -d jun15MC
# Orientation 2
soc2oca -r 10060 -c 10060 -w 10177 -d jun15MC
# Orientation 3
soc2oca -r 10061 -c 10061 -w 10178 -d jun15MC
