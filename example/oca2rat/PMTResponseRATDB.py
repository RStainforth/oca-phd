####################################################################
### FILENAME: PMTResponseRATDB.py
###
### CLASS:  N/A
###
### BRIEF:  The main purpose of this macro is to create tables in the
###         RATDB format of PMT angular responses for the wavelength
###         range 220-710 nm (in steps of 10 nm). It uses the results
###         from OCA, interpolates them and the extrapolation uses
###         the PMTR of the last wavelength of each side of the range
###         that was measured.
###
### AUTHOR: Ana Sofia Inacio <ainacio@lip.pt>
###
### REVISION HISTORY:
###     12/2016 : Ana Inacio - First Revision, new file.
###
### DETAIL: To use this macro, type:
###         
###         python PMTResponseRATDB.py <file_name1> <file_name2> ...
###
####################################################################

import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import interp1d
import numpy.polynomial.polynomial as poly
from scipy import arange
import sys

print "***************************************"
print "****      PMT Angular Response     ****"
print "***************************************"

matrix = np.zeros(shape=(5,90))
matrix_err = np.zeros(shape=(5,90))

i = 0

# Opening the .ocadb files and extracting the relevant informations
# argv is the commandline arguments, argv[0] is this macro
for n in sys.argv[1:]:
  if n == "oct15_watersin_505":
    data_set = "MC 505 nm"
  if n == "oct15_watersin_420":
    data_set = "MC 420 nm"
  if n == "oct15_watersin_385":
    data_set = "MC 385 nm"
  if n == "oct15_watersin_369":
    data_set = "MC 369 nm"
  if n == "oct15_watersin_337":
    data_set = "MC 337 nm"

  par_list = []
  err_list = []

  print(n) #print out the filename we are currently processing
  par = "pmt_angular_response : [ "
  par_err = "pmt_angular_response_errors : [ "
  f = open("/lstore/sno/ainacio/oca-oca-snoplus/output/fits/"+n+".ocadb","r")

  for line in f:
    if par in line:
      numbers_str = line[line.find(par)+len(par):]
      numbers_str = numbers_str.replace(',', '')
      numbers_str = numbers_str.replace(']', '')
      par_list = numbers_str.split(" ")[0:-1]
    if par_err in line:
      err_str = line[line.find(par_err)+len(par_err):]
      err_str = err_str.replace(',', '')
      err_str = err_str.replace(']', '')
      err_list = err_str.split(" ")[0:-1]

  pmt_coeffs = []
  coeff_err = []
  for item in par_list:
    pmt_coeffs.append(float(item))
  for item in err_list:
    coeff_err.append(float(item))

  for j in range(90):
    matrix[i,j] = pmt_coeffs[j]
    matrix_err[i,j] = coeff_err[j]

  i = i + 1

  X = list(range(90))

  # Plots the PMT Angular response in function of the incidence angle for each laserball wavelength
  plt.errorbar(X, pmt_coeffs,yerr=coeff_err, marker='.',linestyle="None",label=data_set)

plt.xlabel("Incident Angle [degrees]")
plt.ylabel("Relative PMT Angular Response (arb. units)")
plt.title("Relative PMT Angular Response")
plt.legend(loc='upper left', fancybox=True, numpoints=1, prop={'size':12})
plt.ylim(1, 1.15)
plt.xlim([0.99,46])
plt.show()

#print matrix

# Plots the PMT Angular response in function of the incidence angle for each laserball wavelength in a color matrix
fig = plt.figure()
ax = fig.add_subplot(111)
cax = ax.imshow(matrix, interpolation='nearest', cmap="jet")
cbar = fig.colorbar(cax)
ax.matshow(matrix,extent=[0,90,0,5])
ax.xaxis.set_ticks_position('bottom')
ax.set_title("PMT Angular Response")
ax.set_aspect('auto')
plt.xlabel("Incident Angle [degrees]")
plt.yticks( arange(5), ('505', '420', '385', '369', '337') )
plt.xticks(np.arange(0,90,10))
plt.show()

#relative_response = np.zeros(shape=(50,45))
relative_response = np.zeros(shape=(50,90))

# Gets the values of PMTR for all wavelengths, for each incidence angle bin, and linearly interpolates the data
for j in range(90):
  thetaBin = []
  thetaBin_err = []

  for i in range(5):
    thetaBin.append(float(matrix[i,j]))
    thetaBin_err.append(float(matrix_err[i,j]))

  string = ("Incident Angle %d" % (j))

  xAxis = [337.0,369.0,385.0,420.0,505.0]
  xx = np.arange(337,506,1)

  function = interp1d(xAxis, thetaBin)

# Plots the relative angular response in function of wavelength for each incidence angle
#  
#  plt.errorbar(xAxis, thetaBin,yerr=thetaBin_err, marker='o',linestyle="None",label=string)
#  plt.plot( xx ,function(xx))
#
#  plt.legend(loc='upper left', fancybox=True, numpoints=1, prop={'size':12})
#  plt.xlabel("Wavelength [nm]")
#  plt.ylabel("Relative PMT Angular Response (arb. units)")
#  name = ("Incidence Angle Bin "+str(j))
#  plt.title(name)
#  plt.show()

# Method that interpolates/extrapolates the results with a polynomial
#
#  z = np.polyfit(xAxis, thetaBin, 3)
#  f = np.poly1d(z)
#  calculate new x's and y's
#  x_new = np.linspace(220, 711, 10)
#  y_new = f(x_new)
#  plt.errorbar(xAxis, thetaBin,yerr=thetaBin_err, marker='o',linestyle="None",label=string)
#  plt.plot(x_new, y_new)
#  plt.legend(loc='upper left', fancybox=True, numpoints=1, prop={'size':12})
#  plt.xlabel("Wavelength [nm]")
#  plt.ylabel("Relative PMT Angular Response (arb. units)")
#  name = ("Incidence Angle Bin "+str(j))
#  plt.title(name)
#  plt.show()

  # For the extrapolation outside the laserball wavelength range, it uses the last angular response obtained for each side
  # i.e., for wavelengths smaller/larger that 337/505 nm it uses the angular response measured for 337/505 nm.
  num = 0
  for i in range(50):
    if (220 + i*10) < 337:
      relative_response[i,j] = function(337)
    if (220 + i*10) > 505:
      relative_response[i,j] = function(505)
    elif ((220 + i*10) > 337) and ((220 + i*10) < 505):
      relative_response[i,j] = function(340+num*10) 
      num = num + 1

# Plots the interpolated/extrapolated PMT Angular responses in function of the incidence angle for 
# each laserball wavelength in a color matrix      
fig = plt.figure()
ax = fig.add_subplot(111)
cax = ax.imshow(relative_response, interpolation='nearest', cmap="jet")
# Add colorbar, make sure to specify tick locations to match desired ticklabels
cbar = fig.colorbar(cax)
ax.matshow(relative_response,extent=[0,90,0,50])
ax.xaxis.set_ticks_position('bottom')
ax.set_title("PMT Angular Response")
ax.set_aspect('auto')
plt.xlabel("Incident Angle [degrees]")
plt.yticks( arange(50), ('710',' ','690',' ','670',' ','650',' ','630',' ','610',' ','590',' ','570',' ','550',' ','530',' ','510',' ','490',' ','470',' ','450',' ','430',' ','410',' ','390',' ','370',' ','350',' ','330',' ','310',' ','290',' ','270',' ','250',' ','230',' ') )
plt.xticks(np.arange(0,90,10))
plt.show()

ff1 = []
for i in range(50):
  for j in range(90):
    ff1.append(float(relative_response[i,j]))

plt.plot(range(4500), ff1)
plt.show()

# Creates the output file and saves the parameters
output_file = open('pmtresponses_snoplus.ratdb', 'w')
print ("Creating Output File: pmtresponses_snoplus.ratdb\n")

output_file.write("{\n")
output_file.write("type: \"\",\n")
output_file.write("version: 1,\n")
output_file.write("index: \"\",\n")
output_file.write("run_range: [0, 0],\n")
output_file.write("pass: 0,\n")
output_file.write("comment: "",\n")
output_file.write("timestamp: "",\n")

output_file.write("pmt_relative_response: [")
for i in range(855):
  output_file.write("%.4f, " % ff1[i])
output_file.write("]\n")

output_file.write("PROPERTY_LIST: [\"pmt_relative_response\"]\n")
output_file.write("}\n")

output_file.close()


