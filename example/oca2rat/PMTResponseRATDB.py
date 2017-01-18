# pmtresponse.py

import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import interp1d
import numpy.polynomial.polynomial as poly
from scipy import interpolate
from numpy import pi, r_
from scipy import optimize
from scipy import arange, array, exp
from matplotlib import cm


import sys

print "***************************************"
print "****      PMT Angular Response     ****"
print "***************************************"

matrix = np.zeros(shape=(5,90))
matrix_err = np.zeros(shape=(5,90))

matrix_sys = np.matrix([[0,0.00186736, 0.00146445, 0.00140754, 0.00193626, 0.00260369, 0.00189529, 0.00208158, 0.00186305, 0.00233566, 0.00225869, 0.00242107, 0.00257348, 0.00288728, 0.00270905, 0.00296523, 0.00375206, 0.00364505, 0.00453092, 0.00477369, 0.00495751, 0.00556234, 0.00617982, 0.00592406, 0.00605194, 0.00707734, 0.00667703, 0.00861832, 0.00968784, 0.0106162, 0.0110885, 0.0107862, 0.0121136, 0.0124131, 0.0121455, 0.0113913, 0.010609, 0.0107754, 0.00989418, 0.0100891, 0.0100061, 0.0103288, 0.0113295, 0.0125452, 0.0113065, 0.01327,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],
              [0,0.00201989, 0.00157936, 0.0017244, 0.00240077, 0.00244128, 0.00178572, 0.00177036, 0.00221765, 0.0022411, 0.00255748, 0.00252637, 0.00253391, 0.00295173, 0.00280282, 0.00339731, 0.00393779, 0.00422463, 0.00451123, 0.00514919, 0.00536114, 0.00610879, 0.00644439, 0.00640922, 0.00676816, 0.0076659, 0.00770208, 0.0102446, 0.0110332, 0.0126561, 0.0127521, 0.0133823, 0.0142563, 0.013764, 0.0133318, 0.0132101, 0.0122389, 0.01222, 0.0112426, 0.011758, 0.0111025, 0.0120834, 0.0125133, 0.0131305, 0.0136234, 0.019581,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],
              [0,0.00182299, 0.00148455, 0.00146831, 0.00219701, 0.00245323, 0.00184803, 0.0020752, 0.00235997, 0.00265037, 0.0026524, 0.00272654, 0.00271558, 0.00283836, 0.00336517, 0.0036058, 0.00419395, 0.00448454, 0.0050988, 0.0058805, 0.00628492, 0.0066474, 0.00669554, 0.00712517, 0.00800136, 0.00867493, 0.009283, 0.0111758, 0.0124967, 0.0131189, 0.0135406, 0.0148019, 0.0150311, 0.0140909, 0.0145648, 0.0140644, 0.0132635, 0.0126604, 0.01243, 0.0120441, 0.0123326, 0.013304, 0.0136008, 0.0150223, 0.0140958, 0.0223931,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],
              [0,0.00191705, 0.00159452, 0.00156537, 0.00208137, 0.00291205, 0.00200085, 0.00240798, 0.00230646, 0.00263587, 0.00265711, 0.00272914, 0.00301872, 0.00299786, 0.00316721, 0.00346219, 0.00431851, 0.00473325, 0.00519863, 0.00596952, 0.00664745, 0.00662908, 0.00692448, 0.00759762, 0.00717464, 0.00859507, 0.00919015, 0.0114774, 0.0124006, 0.0135112, 0.0135639, 0.0141518, 0.0150547, 0.0147458, 0.0143315, 0.0141797, 0.0125487, 0.0130769, 0.0126804, 0.0121346, 0.012116, 0.0127739, 0.0139925, 0.0152885, 0.0137126, 0.015778,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],
              [0,0.00181183, 0.00171821, 0.00143635, 0.001903, 0.00296506, 0.00229676, 0.0019582, 0.00271583, 0.00239423, 0.00252441, 0.00282262, 0.00297015, 0.00329899, 0.00329227, 0.00362621, 0.00429313, 0.00461594, 0.00514328, 0.00621183, 0.00634008, 0.00717348, 0.00766945, 0.00763018, 0.00815892, 0.0087563, 0.00909037, 0.0122251, 0.0132481, 0.0142081, 0.0145435, 0.0149663, 0.0161169, 0.0154213, 0.0152988, 0.0149672, 0.0130725, 0.0129286, 0.0124406, 0.0123659, 0.0124154, 0.0133546, 0.0142646, 0.0141935, 0.0151257, 0.0155923,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]])

i = 0

# Opening the .ocadb files
# argv is your commandline arguments, argv[0] is your program name, so skip it
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
    matrix_err[i,j] = coeff_err[j] + float(matrix_sys[i,j])

  i = i + 1

  X = list(range(90))

  plt.errorbar(X, pmt_coeffs,yerr=coeff_err, marker='.',linestyle="None",label=data_set)

plt.xlabel("Incident Angle [degrees]")
plt.ylabel("Relative PMT Angular Response (arb. units)")

plt.title("Relative PMT Angular Response")

plt.legend(loc='upper left', fancybox=True, numpoints=1, prop={'size':12})

plt.ylim(1, 1.15)
plt.xlim([0.99,46])

plt.show()

print matrix

fig = plt.figure()
ax = fig.add_subplot(111)

cax = ax.imshow(matrix, interpolation='nearest', cmap="jet")

# Add colorbar, make sure to specify tick locations to match desired ticklabels
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

  plt.errorbar(xAxis, thetaBin,yerr=thetaBin_err, marker='o',linestyle="None",label=string)
  plt.plot( xx ,function(xx))

  plt.legend(loc='upper left', fancybox=True, numpoints=1, prop={'size':12})
  plt.xlabel("Wavelength [nm]")
  plt.ylabel("Relative PMT Angular Response (arb. units)")
  name = ("Incidence Angle Bin "+str(j))
  plt.title(name)
  plt.show()

#  z = np.polyfit(xAxis, thetaBin, 3)
#  f = np.poly1d(z)

  # calculate new x's and y's
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

  num = 0
  for i in range(50):
    if (220 + i*10) < 337:
      relative_response[i,j] = function(337)
    if (220 + i*10) > 505:
      relative_response[i,j] = function(505)
    elif ((220 + i*10) > 337) and ((220 + i*10) < 505):
      relative_response[i,j] = function(340+num*10) 
      num = num + 1
      
plt.show()

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

# Output
output_file = open('pmtresponses_snoplus.ratdb', 'w')
print ("Creating Output File: pmtresponses_snoplus.ratdb\n")

output_file.write("{\n")
output_file.write("type: \"...................\",\n")
output_file.write("version: 1,\n")
output_file.write("index: \"...................\",\n")
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


