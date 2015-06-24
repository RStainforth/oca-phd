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

# For BIS-MSB:420nm laser source, the central laserball runs are as follows:
#
# ----------------------
# orientation   | run-id
# ----------------------
# orientation-0 | 10136
# orientation-1 | 10137
# orientation-2 | 10138
# orientation-3 | 10139
# ----------------------
#
# Source the OCA environment
source ${OCA_ROOT}/env_oca.sh

# There should be 39 runs for each wavelength in this dataset.

# Orientation: 1, Laserball Facing North (+y direction).
soc2oca -r 10125 -c 10137 -w 10164 -d jun15MC
soc2oca -r 10126 -c 10137 -w 10165 -d jun15MC
soc2oca -r 10127 -c 10137 -w 10166 -d jun15MC
soc2oca -r 10128 -c 10137 -w 10167 -d jun15MC

soc2oca -r 10129 -c 10137 -w 10168 -d jun15MC
soc2oca -r 10130 -c 10137 -w 10169 -d jun15MC
soc2oca -r 10131 -c 10137 -w 10170 -d jun15MC
soc2oca -r 10135 -c 10137 -w 10174 -d jun15MC

soc2oca -r 10142 -c 10137 -w 10181 -d jun15MC
soc2oca -r 10143 -c 10137 -w 10182 -d jun15MC
soc2oca -r 10144 -c 10137 -w 10183 -d jun15MC
soc2oca -r 10145 -c 10137 -w 10184 -d jun15MC

soc2oca -r 10146 -c 10137 -w 10185 -d jun15MC
soc2oca -r 10147 -c 10137 -w 10186 -d jun15MC
soc2oca -r 10148 -c 10137 -w 10187 -d jun15MC
soc2oca -r 10149 -c 10137 -w 10188 -d jun15MC

# Orientation: 2, Laserball Facing West (-x direction).
soc2oca -r 10118 -c 10138 -w 10157 -d jun15MC
soc2oca -r 10119 -c 10138 -w 10158 -d jun15MC
soc2oca -r 10120 -c 10138 -w 10159 -d jun15MC
soc2oca -r 10121 -c 10138 -w 10160 -d jun15MC

soc2oca -r 10122 -c 10138 -w 10161 -d jun15MC
soc2oca -r 10123 -c 10138 -w 10162 -d jun15MC
soc2oca -r 10124 -c 10138 -w 10163 -d jun15MC
soc2oca -r 10132 -c 10138 -w 10171 -d jun15MC

soc2oca -r 10133 -c 10138 -w 10172 -d jun15MC
soc2oca -r 10134 -c 10138 -w 10173 -d jun15MC
soc2oca -r 10140 -c 10138 -w 10179 -d jun15MC
soc2oca -r 10141 -c 10138 -w 10180 -d jun15MC

soc2oca -r 10150 -c 10138 -w 10189 -d jun15MC
soc2oca -r 10151 -c 10138 -w 10190 -d jun15MC
soc2oca -r 10152 -c 10138 -w 10191 -d jun15MC
soc2oca -r 10153 -c 10138 -w 10192 -d jun15MC

soc2oca -r 10154 -c 10138 -w 10193 -d jun15MC
soc2oca -r 10155 -c 10138 -w 10194 -d jun15MC
soc2oca -r 10156 -c 10138 -w 10195 -d jun15MC

# Central Runs

# Orientation 0
soc2oca -r 10136 -c 10136 -w 10175 -d jun15MC
# Orientation 1
soc2oca -r 10137 -c 10137 -w 10176 -d jun15MC
# Orientation 2
soc2oca -r 10138 -c 10138 -w 10177 -d jun15MC
# Orientation 3
soc2oca -r 10139 -c 10139 -w 10178 -d jun15MC
