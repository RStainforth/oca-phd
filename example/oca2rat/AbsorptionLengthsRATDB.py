####################################################################
### FILENAME: AbsorptionLengthsRATDB.py
###
### CLASS:  N/A
###
### BRIEF:  The main purpose of this macro is to create tables in the
###         RATDB format of the water absorption lengths for the
###         wavelength range 200-800 nm (in steps of 20 nm). It uses 
###         the attenuations from OCA, calculates absorption lengths
###         using SMELLIE results, and the ratio of the measured values
###         by the ones in RAT. From the fit of the ratio, a scaling
###         function that varies with wavelength is defined and used to
###         scale the absorption length parameters in RAT.
###
### AUTHOR: Ana Sofia Inacio <ainacio@lip.pt>
###
### REVISION HISTORY:
###     12/2016 : Ana Inacio - First Revision, new file.   
###
### DETAIL: To use this macro, type:
###         
###         python AbsorptionLengthsRATDB.py <file_name1> <file_name2> ...
###
####################################################################

import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import interp1d
import numpy.polynomial as poly
from scipy import interpolate
from scipy import arange, array, exp

import sys

print "***************************************"
print "**** Calculating Absorption Length ****"
print "***************************************"

# Gets the Rayleigh Scattering values from OPTICS.ratdb
rat_file = "rat/data/OPTICS.ratdb"
rat = open(rat_file,"r")
print "Opening file ", rat_file

wl_rat = []
rs_rat = []
rat_abs = []

line_rat = "index: \"lightwater_sno\""
wl_str = "RSLENGTH0_value1: ["
rs_str = "RSLENGTH0_value2: ["
rat_abs_str = "ABSLENGTH0_value2: ["

for line in rat:
  if line_rat in line:
    for n in range(20):
      if wl_str in line:
        value = line[line.find(wl_str)+len(wl_str):]
        value = value.replace(',\n', '')
        value = value.replace(',', '')
        value = value.replace(']', '')
        wl_rat = value.split(" ")[0:-1]
      if rs_str in line:
        value = line[line.find(rs_str)+len(rs_str):]
        value = value.replace(',\n', '')
        value = value.replace(',', '')
        value = value.replace(']', '')
        rs_rat = value.split(" ")[0:-1]
      if rat_abs_str in line:
        value = line[line.find(rat_abs_str)+len(rat_abs_str):]
        value = value.replace(',\n', '')
        value = value.replace(',', '')
        value = value.replace(']', '')
        rat_abs = value.split(" ")[0:-1]
      line=rat.next().strip()
rat.close()

rs_wavelength = []
rsLength = []
rat_absLength = []

for item in wl_rat:
  rs_wavelength.append(float(item))
for item in rs_rat:
  rsLength.append(float(item))
for item in rat_abs:
  rat_absLength.append(float(item))

# Rayleigh Scattering Lengths are linearly interpolated as a function of wavelength
f_rs = interp1d(rs_wavelength, rsLength)

#######################################################################################################
#######################################################################################################
#######################################################################################################

# Opens the output files from OCA, gets the inner AV attenuation coefficients (in mm-1)
att_coeff = []
coeff_err = []

wl = []

for n in sys.argv[1:]:
  print("Opening file "+n) 
  par = "inner_av_extinction_length : "
  par_err = "inner_av_extinction_length_error : "
  file_name = open("oca/output/fits/"+n+".ocadb","r")
  for line in file_name:
    if par in line:
      param = line[line.find(par)+len(par):]
      param = param.replace(',\n', '')
      att_coeff.append(float(param))
    if par_err in line:
      param = line[line.find(par_err)+len(par_err):]
      param = param.replace(',\n', '')
      coeff_err.append(float(param))
    
  file_name.close()
  wl.append(float(n[-3:]))

k = 0.82

abs_length = []
abs_coeff = []
abs_length_error = []

j = 0
for item in wl:
  abs_length.append(float(1./(att_coeff[j]-k*(1./float(f_rs(item))))))

  abs_length_error.append(float( coeff_err[j] / ( (att_coeff[j]-k*(1./float(f_rs(item))))*(att_coeff[j]-k*(1./float(f_rs(item)))) ) ))

  j = j + 1

#######################################################################################################
#######################################################################################################
#######################################################################################################

# Plotting the results
plt.errorbar(wl, abs_length,yerr=abs_length_error, marker='o',color='r',markersize=10,linestyle="--", label = 'OCA Absorption Length')

plt.plot(rs_wavelength, rsLength, "m+",markersize=10, label = 'RAT RScattering Length')
plt.plot(rs_wavelength, rat_absLength,"r+",markersize=10, label = "RAT Absorption Length")

plt.xlabel('Wavelength [nm]')
plt.xlim(200, 565)
plt.ylabel('Length [mm]')
plt.ylim(0, 4.5e5)

plt.title("Internal Water")
# Make legend
plt.legend(loc='upper left', fancybox=True, numpoints=1)
ax = plt.gca()
ax.ticklabel_format(style='sci', scilimits=(0,0), axis='y') 
plt.show()

#######################################################################################################
#######################################################################################################
#######################################################################################################

plt.errorbar(wl, abs_length,yerr=abs_length_error, marker='o',color='r',markersize=5,linestyle="-", label = 'Calculated Absorption Length')

f_rat = interp1d(rs_wavelength, rat_absLength)
xnew = np.arange(335,510,1)
plt.plot(xnew,f_rat(xnew),"r+", label = "RAT Absorption Length")

plt.xlabel('Wavelength [nm]')
plt.ylabel('Length [mm]')
plt.legend(loc='upper left', fancybox=True, numpoints=1)
ax = plt.gca()
ax.ticklabel_format(style='sci', scilimits=(0,0), axis='y') 
plt.title("Calculated Absorption Length and RAT Absorption Length")
plt.show()

#######################################################################################################
#######################################################################################################
#######################################################################################################

ratio = []
ratio_err = []

for i,item in enumerate(wl): 
  ratio.append(float(abs_length[i]/f_rat(item)))
  ratio_err.append(float(abs_length_error[i]/f_rat(item)))

f_iterpolation = interp1d(wl, ratio)
xx = np.arange(337,500.01,0.01)
print xx
f_iterpo = []
for i,item in enumerate(xx):
  f_iterpo.append(float(f_iterpolation(item)))
plt.errorbar(wl, ratio,yerr=ratio_err, marker='o',color='r',markersize=10,linestyle="-", label = 'Measured/RAT')
plt.plot(xx,f_iterpo,linestyle="-",color="k")
plt.xlabel('Wavelength [nm]')
plt.ylabel('Measured/RAT')
plt.legend(loc='upper left', fancybox=True, numpoints=1)
ax = plt.gca()
ax.ticklabel_format(style='sci', scilimits=(0,0), axis='y')
plt.title("Measured Absorption Length/RAT Absorption Length (Linear Interpolation)")
plt.xlim(330,505)
plt.show()

#######################################################################################################
#######################################################################################################
#######################################################################################################

# Scaling the RAT parameters according to the measurements (multiplying by the fit functions)

x_axis = np.arange(337.,501.,1)
x_axis2 = np.arange(200.,801.,20)

ff = []

for i,item in enumerate(x_axis2):
  if(item <= 337.):
    ff.append(float(rat_absLength[i]*f_iterpolation(337)))
  elif(item >= 500.):
    ff.append(float(rat_absLength[i]*f_iterpolation(500)))
  else:
    ff.append(float(rat_absLength[i]*f_iterpolation(item)))

plt.plot(x_axis2,ff,marker='o',color='b',markersize=10,linestyle='None', label = "New RAT Absorption from Linear Interpolation")

plt.plot(x_axis2,f_rat(x_axis2),"k+",markersize=10,linestyle='None', label = "OLD RAT Absorption Length")
plt.errorbar(wl, abs_length,yerr=abs_length_error, marker='o',color='r',markersize=10,linestyle='None', label = 'NEW Absorption Length')
plt.xlabel('Wavelength [nm]')
plt.ylabel('Length [mm]')
plt.legend(loc='upper left', fancybox=True, numpoints=1)
ax = plt.gca()
ax.ticklabel_format(style='sci', scilimits=(0,0), axis='y') 
plt.title("Scaled RAT Absorption Lengths")
plt.xlim(150,850)
plt.ylim(0.0, 3.3e5)
plt.show()

#######################################################################################################
#######################################################################################################
#######################################################################################################

# Creating the output file and writing the parameters

output_file = open('lightwater_snoplus.ratdb', 'w')
print ("Creating Output File: lightwater_snoplus.ratdb\n")

output_file.write("{\n")
output_file.write("type: \"OPTICS\",\n")
output_file.write("version: 1,\n")
output_file.write("index: \"lightwater_snoplus\",\n")
output_file.write("run_range: [0, 0],\n")
output_file.write("pass: 0,\n")
output_file.write("comment: "",\n")
output_file.write("timestamp: "",\n")

output_file.write("ABSLENGTH0_option: \"wavelength\",\n")

output_file.write("ABSLENGTH0_value1: [")
init = 200;
for i in range(31):
  output_file.write("%s, " % (init+20*i))
output_file.write("]\n")

output_file.write("ABSLENGTH0_value2: [")
init = 200;
for i in range(31):
  output_file.write("%.1f, " % ff[i])
output_file.write("]\n")

output_file.write("ABSLENGTH_SCALING: [1.0],\n")

output_file.write("PROPERTY_LIST: [\"ABSLENGTH0\", \"ABSLENGTH_SCALING\"]\n")
output_file.write("}\n")

output_file.close()
