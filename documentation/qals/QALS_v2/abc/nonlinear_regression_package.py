# Importing packages
import pandas as pd
import matplotlib.pyplot as plt
import sys
from numpy import *
from sklearn.metrics import mean_squared_error
from sklearn.model_selection import train_test_split
from math import sqrt

# Loading data from csv file
with open('final_result.txt') as f:
      y = f.read().splitlines()

y = list(map(int, y))
x=list()
# Generate list x
for i in range(len(y)):
   x.append(i+1)

# Train the model to fit the polynomial of degree 3
coefficients = polyfit(x, y, 10)

# The coefficients
print('Coefficients: \n',coefficients)

# Write coefficients to a file
fp = open('reg_coeff.txt', 'w')
for item in coefficients:
   fp.write("%s\n" % item)
fp.close()
