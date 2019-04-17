# -*- coding: utf-8 -*-
"""
Created on Tue Mar 26 08:48:09 2019

@author: ksouvik
"""
import numpy as np
import sys
trainfile = 'error_train.npz'
filename = 'train_dnn.txt'

def loadingBar(count, total, size):
    percent = float(count)/float(total)*100
    sys.stdout.write("\r" + "calc: " + str(int(count)).rjust(3,'0')+"/"+str(int(total)).rjust(3,'0') + " " + str(round(count*100/total, 1)) + "%" + ' [' + '='*int(percent/10)*size + ' '*(10-int(percent/10))*size + ']')


def file_lengthy(filename):
    with open(filename) as f:
        for i,l in enumerate(f):
            pass
    return i+1

print("Running data_to_npz...")

num_features = 93
num_outlabels = 2

flen = file_lengthy(filename)
featurelist = []
i = 0

with open (filename, 'r') as f:
    for line in f:
        for word in line.split():
            featurelist.append(word)
           
loadbar_count = 0
loadbar_total = 2*flen + num_features + flen*num_features
           
ydata = np.zeros((flen, num_outlabels))
xdata = np.zeros((flen, num_features))

for itr in range (flen):
    for fnum in range (num_features):
        feature_loc = (itr)*95 + fnum   
        xdata[itr][fnum] = featurelist[feature_loc]
    for lnum in range (num_outlabels):
        lloc = (itr*95) + 93 + lnum
        ydata[itr][lnum] = featurelist[lloc]

    loadingBar(loadbar_count, loadbar_total, 3)
    loadbar_count = loadbar_count + 1


file = open("td_normalization_values.txt", "w")

for fetr in range (num_features):
    min = np.min(xdata[:,fetr])
    max = np.max(xdata[:,fetr])
    file.write(str(min))
    file.write(" ")
    file.write(str(max))
    file.write("\n")
    for itr in range (flen):
        xdata[itr][fetr] = (xdata[itr,fetr] - min) / max - min) 
        
        loadingBar(loadbar_count, loadbar_total, 3)
        loadbar_count = loadbar_count + 1
           
file.close()

for itr in range (flen):
    if (ydata[itr][1] <= 0.5 ):
        ydata[itr][1] = np.ceil(40 * ydata[itr,1]/0.5)
    else :
        ydata[itr][1] = np.ceil(10 * (ydata[itr,1] - 0.5)/0.5) + 40 
    loadingBar(loadbar_count, loadbar_total, 3)
    loadbar_count = loadbar_count + 1
np.savez_compressed(trainfile, x_train = xdata, y_train = ydata[:,1])

        


            
