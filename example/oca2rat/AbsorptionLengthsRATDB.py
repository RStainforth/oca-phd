# abslength.py

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

#pope_wl = [380, 382.5, 385, 387.5 ,390, 392.5, 395, 397.5, 400, 402.5, 405, 407.5, 410, 412.5, 415, 417.5, 420, 422.5, 425, 427.5, 430, 432.5, 435, 437.5, 440, 442.5, 445, 447.5, 450, 452.5, 455, 457.5, 460, 462.5, 465, 467.5, 470, 472.5, 475, 477.5, 480, 482.5, 485, 487.5, 490, 492.5, 495, 497.5, 500, 502.5, 505, 507.5, 510, 512.5, 515, 517.5, 520]
#pope_abs = [0.00001137,0.00001044,	0.00000941,	0.00000917,	0.00000851,	0.00000829,	0.00000813,	0.00000775,	0.00000663,	0.00000579,	0.0000053,	0.00000503,	0.00000473,	0.00000452,	0.00000444,	0.00000442,	0.00000454,	0.00000474,	0.00000478,	0.00000482,	0.00000495,	0.00000504,	0.0000053,	0.0000058,	0.00000635,	0.00000696,	0.00000751,	0.0000083,	0.00000922,	0.00000969,	0.00000962,	0.00000957,	0.00000979,	0.00001005,	0.00001011,	0.0000102,	0.0000106,	0.0000109,	0.0000114,	0.0000121,	0.0000127,	0.0000131,	0.0000136,	0.0000144,	0.000015,	0.0000162,	0.0000173,	0.0000191,	0.0000204,	0.0000228,	0.0000256,	0.000028,	0.0000325,	0.0000372,	0.0000396,	0.0000399,	0.0000409]

#for p, item in enumerate(pope_abs):
#  pope_abs[p] = 1/pope_abs[p]

diag_scan = [0.0000305327,0.0000227798,0.0000190338,0.0000230343,0.0000431756]
diag_scan_err = [0.00000058027,0.000000568783,0.000000522112,0.000000571383,0.000000573561]

for p, item in enumerate(diag_scan):
  diag_scan_err[p] = diag_scan_err[p]/(diag_scan[p]*diag_scan[p])
  diag_scan[p] = 1/diag_scan[p]


# Gets the Rayleigh Scattering values from OPTICS.ratdb
rat_file = "/exper-sw/sno/snoplus/snoing/install/rat-5.2.2/data/OPTICS.ratdb"
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

# Rayleigh Scattering Lengths are linearly interpolated in function of wavelength
f_rs = interp1d(rs_wavelength, rsLength)

#print xnew
#plt.plot( xnew, f_rs(xnew), 'o')
#plt.show()

#######################################################################################################
#######################################################################################################
#######################################################################################################

# Opens the .ocadb files from OCA, gets the inner AV attenuation coefficients and calculates the attenuation lengths (in mm) 
att_coeff = []
coeff_err = []

att_length = []
att_length_error = []

# argv is your commandline arguments, argv[0] is your program name, so skip it
for n in sys.argv[1:]:
  print("Opening file "+n) 
  par = "inner_av_extinction_length : "
  par_err = "inner_av_extinction_length_error : "
  file_name = open("/lstore/sno/ainacio/oca-oca-snoplus/output/fits/"+n,"r")
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

syst = [1.98632e-6,2.06508e-6,2.09412e-6,2.06529e-6,2.14077e-6]

for i, item in enumerate(att_coeff):
  lgth = 1/att_coeff[i]
  att_length.append(float(lgth))

  err = lgth*lgth*(coeff_err[i]+syst[i])
  att_length_error.append(float(err))

#######################################################################################################
#######################################################################################################
#######################################################################################################

# Calculation of the absorption lengths
abs_length = []
abs_length_error = []

lb_wl = [337.0, 369.0, 385.0, 420.0, 505.0]

j = 0
for item in lb_wl:
  abs_length.append(float(att_length[j]*f_rs(item)/(f_rs(item)-0.77*att_length[j])))

  err = (att_length_error[j]*f_rs(item)/(f_rs(item)-0.77*att_length[j])) + (att_length_error[j]*att_length[j]*f_rs(item)/((f_rs(item)-0.77*att_length[j])*(f_rs(item)-0.77*att_length[j])))

  abs_length_error.append(float(err))

  j = j + 1
  
#print abs_length, abs_length_error

rat_att = []

for k, item in enumerate(rs_wavelength):
  rat_att.append(float(rsLength[k]*rat_absLength[k]/(rsLength[k]+rat_absLength[k])))

#######################################################################################################
#######################################################################################################
#######################################################################################################

ext_w = []
ext_w_err = []

param1 = 0
param2 = 0

# argv is your commandline arguments, argv[0] is your program name, so skip it
for n in sys.argv[1:]:
  print(n) #print out the filename we are currently processing
  par = "water_extinction_length : "
  par_err = "water_extinction_length_error : "
  f = open("/lstore/sno/ainacio/oca-oca-snoplus/output/fits/"+n,"r")
  for line in f:
    if par in line:
      param1 = line[line.find(par)+len(par):]
      param1 = param1.replace(',\n', '')
      ext_w.append(1./float(param1))
    if par_err in line:
      param2 = line[line.find(par_err)+len(par_err):]
      param2 = param2.replace(',\n', '')
      ext_w_err.append(float(param2)/(float(param1)*float(param1)))


#################################################################################################!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!calcular erro 

x = [337.0, 369.0, 385.0, 420.0, 505.0]

#plt.errorbar(x, ext_w,yerr=ext_w_err, marker='o',linestyle="None", label = 'External Water AttLength OCA')

# Plotting the results
plt.errorbar(lb_wl, att_length,yerr=att_length_error, marker='o',color='c',markersize=5,linestyle="-", label = 'Attenuation Length OCA')
#plt.plot(lb_wl, f_att(lb_wl), label = 'AttLength Interpolation')

plt.plot(rs_wavelength, rsLength, marker='.', linestyle="-", label = 'RScattering Length RAT',markeredgecolor='none')
#plt.plot(xnew, f_rs(xnew), label = 'RScattering RAT Interpolation')

plt.errorbar(lb_wl, abs_length,yerr=abs_length_error, marker='o',color='r',markersize=5,linestyle="-", label = 'Calculated Absorption Length')
#plt.plot(lb_wl, f_abs(xnew), label = 'Absorption Length')

plt.plot(rs_wavelength,rat_absLength,"r+", label = "RAT Absorption Length")

plt.plot(rs_wavelength,rat_att,"ko", label = "RAT Attenuation Length")
#plt.plot(pope_wl,pope_abs,"ko", label = "Pope et al.")

plt.xlabel('Wavelength [nm]')
plt.xlim(200, 565)
plt.ylabel('Length [mm]')
plt.ylim(0, 4.5e5)

plt.title("Inner AV filled with water")

# Make legend
plt.legend(loc='upper left', fancybox=True, numpoints=1)
ax = plt.gca()
ax.ticklabel_format(style='sci', scilimits=(0,0), axis='y') 

plt.show()


#######################################################################################################
#######################################################################################################
#######################################################################################################

# Plotting the results
plt.errorbar(lb_wl, att_length,yerr=att_length_error, marker='o',color='c',markersize=5,linestyle="-", label = 'Attenuation Length OCA')
#plt.plot(lb_wl, f_att(lb_wl), label = 'AttLength Interpolation')

plt.plot(rs_wavelength, rsLength, marker='.', linestyle="-", label = 'RScattering Length RAT',markeredgecolor='none')
#plt.plot(xnew, f_rs(xnew), label = 'RScattering RAT Interpolation')

plt.errorbar(lb_wl, abs_length,yerr=abs_length_error, marker='o',color='r',markersize=5,linestyle="-", label = 'Calculated Absorption Length')
#plt.plot(lb_wl, f_abs(xnew), label = 'Absorption Length')

plt.plot(rs_wavelength,rat_absLength,"r+", label = "RAT Absorption Length")

plt.plot(rs_wavelength,rat_att,"ko", label = "RAT Attenuation Length")

#plt.errorbar(x, ext_w,yerr=ext_w_err, marker='o',linestyle="None", label = 'External Water AttLength OCA')

#plt.errorbar(lb_wl, diag_scan,yerr=diag_scan_err, marker='p',color='g',linestyle="None", label = 'DiagScan Attenuation')

#plt.plot(pope_wl,pope_abs,"ko", label = "Pope et al.")

plt.xlabel('Wavelength [nm]')
plt.xlim(200, 565)
plt.ylabel('Length [mm]')
plt.ylim(0, 1.e5)

plt.title("Inner AV filled with water (Zoomed)")

# Make legend
plt.legend(loc='upper left', fancybox=True, numpoints=1)
ax = plt.gca()
ax.ticklabel_format(style='sci', scilimits=(0,0), axis='y') 

plt.show()


#######################################################################################################
#######################################################################################################
#######################################################################################################

plt.errorbar(lb_wl, abs_length,yerr=abs_length_error, marker='o',color='r',markersize=5,linestyle="-", label = 'Calculated Absorption Length')
f_rat = interp1d(rs_wavelength, rat_absLength)
xnew = np.arange(335,510,1)
plt.plot(xnew,f_rat(xnew),"r+", label = "RAT Absorption Length")
plt.errorbar(lb_wl, att_length,yerr=att_length_error, marker='o',color='c',markersize=5,linestyle="-", label = 'Attenuation Length OCA')
plt.plot(rs_wavelength,rat_att,"ko", label = "RAT Attenuation Length")
#plt.plot(pope_wl,pope_abs,"ko", label = "Pope et al.")
#plt.errorbar(x, ext_w,yerr=ext_w_err, marker='o',linestyle="None", label = 'External Water AttLength OCA')
#plt.errorbar(lb_wl, diag_scan,yerr=diag_scan_err, marker='p',color='g',linestyle="None", label = 'DiagScan Attenuation')
plt.xlabel('Wavelength [nm]')
plt.ylabel('Length [mm]')
plt.legend(loc='upper left', fancybox=True, numpoints=1)
ax = plt.gca()
ax.ticklabel_format(style='sci', scilimits=(0,0), axis='y') 
plt.title("Calculated Absorption Length and RAT Absorption Length")

plt.show()

ratio = []
ratio_err = []

for i,item in enumerate(lb_wl): 
  ratio.append(float(abs_length[i]/f_rat(item)))
  ratio_err.append(float(abs_length_error[i]/f_rat(item)))

plt.errorbar(lb_wl, ratio,yerr=ratio_err, marker='o',color='r',markersize=5,linestyle="-", label = 'Calculated/RAT')
plt.xlabel('Wavelength [nm]')
plt.ylabel('Calc/RAT')
plt.legend(loc='upper left', fancybox=True, numpoints=1)
ax = plt.gca()
ax.ticklabel_format(style='sci', scilimits=(0,0), axis='y') 
plt.title("Calculated Absorption Length/RAT Absorption Length")

plt.show()


#######################################################################################################
#######################################################################################################
#######################################################################################################

f1 = []
f1 = np.polyfit(lb_wl, ratio, 1)
 
xx = np.arange(337,505,1)
function1 = f1[0]*xx + f1[1]

plt.errorbar(lb_wl, ratio,yerr=ratio_err, marker='o',color='r',markersize=5,linestyle="-", label = 'Calculated/RAT')
plt.plot(xx,function1,linestyle="-",color="k")
plt.xlabel('Wavelength [nm]')
plt.ylabel('Calc/RAT')
plt.legend(loc='upper left', fancybox=True, numpoints=1)
ax = plt.gca()
ax.ticklabel_format(style='sci', scilimits=(0,0), axis='y') 
plt.title("Calculated Absorption Length/RAT Absorption Length (Linear Fit)")

plt.show()

#######################################################################################################
#######################################################################################################
#######################################################################################################


f2 = []
f2 = np.polyfit(lb_wl, ratio, 2)
 
xx = np.arange(337,505,1)
function2 = f2[0]*xx**2 + f2[1]*xx + f2[2]

plt.errorbar(lb_wl, ratio,yerr=ratio_err, marker='o',color='r',markersize=5,linestyle="-", label = 'Calculated/RAT')
plt.plot(xx,function2,linestyle="-",color="k")
plt.xlabel('Wavelength [nm]')
plt.ylabel('Calc/RAT')
plt.legend(loc='upper left', fancybox=True, numpoints=1)
ax = plt.gca()
ax.ticklabel_format(style='sci', scilimits=(0,0), axis='y') 
plt.title("Calculated Absorption Length/RAT Absorption Length (PolyFit Degree 2)")

plt.show()

#######################################################################################################
#######################################################################################################
#######################################################################################################


f3 = []
f3 = np.polyfit(lb_wl, ratio, 3)
 
xx = np.arange(337,505,1)
function3 = f3[0]*xx**3 + f3[1]*xx**2 + f3[2]*xx + f3[3]

plt.errorbar(lb_wl, ratio,yerr=ratio_err, marker='o',color='r',markersize=5,linestyle="-", label = 'Calculated/RAT')
plt.plot(xx,function3,linestyle="-",color="k")
plt.xlabel('Wavelength [nm]')
plt.ylabel('Calc/RAT')
plt.legend(loc='upper left', fancybox=True, numpoints=1)
ax = plt.gca()
ax.ticklabel_format(style='sci', scilimits=(0,0), axis='y') 
plt.title("Calculated Absorption Length/RAT Absorption Length (PolyFit Degree 3)")

plt.show()


#######################################################################################################
#######################################################################################################
#######################################################################################################

x_axis = np.arange(337.,506.,1)
x_axis2 = np.arange(200.,801.,20)

print x_axis2

function1 = f1[0]*x_axis + f1[1]
function2 = f2[0]*x_axis**2 + f2[1]*x_axis + f2[2]
function3 = f3[0]*x_axis**3 + f3[1]*x_axis**2 + f3[2]*x_axis + f3[3]

ff1 = []
ff2 = []
ff3 = []

function1 = function1.tolist()
function2 = function2.tolist()
function3 = function3.tolist()

#print x_axis
for i,item in enumerate(x_axis2):
  if(item < 337.):
    ff1.append(float(rat_absLength[i]*(f1[0]*337. + f1[1])))
    ff2.append(float(rat_absLength[i]*(f2[0]*337**2 + f2[1]*337 + f2[2])))
    ff3.append(float(rat_absLength[i]*(f3[0]*337**3 + f3[1]*337**2 + f3[2]*337 + f3[3])))
  elif(item > 505.):
    ff1.append(float(rat_absLength[i]*(f1[0]*505. + f1[1])))
    ff2.append(float(rat_absLength[i]*(f2[0]*505**2 + f2[1]*505 + f2[2])))
    ff3.append(float(rat_absLength[i]*(f3[0]*505**3 + f3[1]*505**2 + f3[2]*505 + f3[3])))
  else:
    ff1.append(float(rat_absLength[i]*function1[i]))
    ff2.append(float(rat_absLength[i]*function2[i]))
    ff3.append(float(rat_absLength[i]*function3[i]))

plt.plot(x_axis2,ff1, label = "Linear")
plt.plot(x_axis2,ff2, label = "Poly2")
plt.plot(x_axis2,ff3, label = "Poly3")

plt.plot(x_axis2,f_rat(x_axis2),"k+",markersize=10, label = "RAT Absorption Length")
#plt.plot(pope_wl,pope_abs,"ko", label = "Pope et al.")
plt.xlabel('Wavelength [nm]')
plt.ylabel('Length [mm]')
plt.legend(loc='upper left', fancybox=True, numpoints=1)
ax = plt.gca()
ax.ticklabel_format(style='sci', scilimits=(0,0), axis='y') 
plt.title("Scaled RAT Absorption Lengths")

plt.show()

#######################################################################################################
#######################################################################################################
#######################################################################################################

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
  output_file.write("%.1f, " % ff1[i])
output_file.write("]\n")

output_file.write("ABSLENGTH_SCALING: [1.0],\n")

output_file.write("PROPERTY_LIST: [\"ABSLENGTH0\", \"ABSLENGTH_SCALING\"]\n")
output_file.write("}\n")

output_file.close()
