####################################################################
### FILENAME: GreyDiscParametersRATDB.py
###
### CLASS:  N/A
###
### BRIEF:  The main purpose of this macro is to create tables in the
###         RATDB format of grey disc parameters for the wavelength
###         range 220-710 nm (in steps of 10 nm). It uses as input the
###         table of PMT Angular Responses created by PMTResponseRATDB.py
###
### AUTHOR: Ana Sofia Inacio <ainacio@lip.pt>
###
### REVISION HISTORY:
###     09/2018 : Ana Inacio - First Revision, new file.
###
### DETAIL: To use this macro, type:
###         
###         python GreyDiscParametersRATDB.py
###
####################################################################

import numpy as np
import scipy
import matplotlib.pyplot as plt
from scipy.interpolate import interp1d
from decimal import Decimal

greydisc_params = []

################################################################################### READING FILES
#--------------- SIMULATION PARAMETERS
name = "rat/data/GREY_DISC_PARAMETERS.ratdb"

input_file = open(name,"r")
print ("Opening Input File: "+name+"\n")

par = "absorption_probability: [ "
par_list = []

absorption_probability = []

for line in input_file:
  if par in line:
    numbers_str = line[line.find(par)+len(par):]
    numbers_str = numbers_str.replace(',', '')
    numbers_str = numbers_str.replace(']', '')
    par_list = numbers_str.split(" ")[0:-1]
 
    for item in par_list:
      absorption_probability.append(float(item))

input_file.close()

#--------------- OCA PARAMETERS
name1 = "oca/example/oca2rat/pmtresponses_snoplus.ratdb"

ocadb_file = open(name1,"r")
print ("Opening PMT Responses File: "+name1+"\n")

par = "pmt_relative_response: ["
par_list = []

pmt_response = []

for line in ocadb_file:
  if par in line:
    numbers_str = line[line.find(par)+len(par):]
    numbers_str = numbers_str.replace(',', '')
    numbers_str = numbers_str.replace(']', '')
    par_list = numbers_str.split(" ")[0:-1]
 
    for item in par_list:
      pmt_response.append(float(item))

ocadb_file.close()

initial_wl = 220
step = 10

theta_bin = range(90)

greydisc_params = []

for i in range(50):

  old = []
  new = []

  for j in range(90):
    if (j<44):
      greydisc_params.append('%.5e' % Decimal(str(absorption_probability[i*90]*pmt_response[i*90+j])))
      new.append(absorption_probability[i*90]*pmt_response[i*90+j])
    else:
      greydisc_params.append('%.5e' % Decimal(str(absorption_probability[i*90+j])))
      new.append(absorption_probability[i*90+j])

    old.append(absorption_probability[i*90+j])

  plt.plot(theta_bin, old,"ko",linestyle='-',label="old")
  plt.plot(theta_bin, new,"ro",linestyle='-',label="new")
  plt.show()

print greydisc_params
print len(greydisc_params)
