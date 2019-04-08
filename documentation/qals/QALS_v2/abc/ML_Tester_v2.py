from subprocess import *;
from random import *;
from numpy import *;
import os;
import sys;

filename=sys.argv[1]
temp=filename.split('.')
filetype=temp[1]

# Get Number of Nodes and maximum bit difference for each node
fp = open('dummy', 'w')
fp.write("read mcnc.genlib\n")
if (filetype == 'bench'):
   fp.write("read_bench "+filename+"\n")
elif (filetype == 'v'):
   fp.write("read_verilog "+filename+"\n")
elif (filetype == 'blif'):
   fp.write("read_blif "+filename+"\n")
fp.write("map -l 0")
fp.close()
call(['./abc','-f','dummy'])

# Read file nodes.txt into a list
with open('nodes.txt') as f:
   nodes = f.read().splitlines()

# Generate list x_test
x_test=list()   
for i in range(int(nodes[0])):
   x_test.append(i+1)
   
# Load coefficients from file
with open('reg_coeff.txt') as f:
      coefficients = f.read().splitlines()

coefficients = list(map(float, coefficients))
# Generate maximum hamming distance
polynomial = poly1d(coefficients)
y_pred = polynomial(x_test)

# Write coefficients to a file
fp = open('generated_result.txt', 'w')
fp.write("%s\n" % len(y_pred))
for item in y_pred:
   item = int(round(item))
   fp.write("%s\n" % item)
fp.close()

# Testing using final path 
fp = open('dummy', 'w')
fp.write("read mcnc.genlib\n")
if (filetype == 'bench'):
   fp.write("read_bench "+filename+"\n")
elif (filetype == 'v'):
   fp.write("read_verilog "+filename+"\n")
elif (filetype == 'blif'):
   fp.write("read_blif "+filename+"\n")
fp.write("map")
#fp.write("%s " % len(y_pred))
#for item in y_pred:
#   item = int(round(item))
#   fp.write("%s " % item)
fp.write("\nprint_stats")
fp.close()
call(['./abc','-f','dummy']) 

if (filetype == 'bench'):
   # Generate required conf file for error calculation
   call(['python3','graph.py',filename])

   # Error calculation
   call(['./error'])
   # Read Error from file
   with open('final_error.txt') as f:
      error = f.read().splitlines()
      error=float(error[0])   

