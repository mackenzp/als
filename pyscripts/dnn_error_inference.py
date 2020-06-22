# -*- coding: utf-8 -*-
"""
Created on Sun Apr  7 16:09:56 2019

@author: Souvik Kundu
"""
#
# Modifications to set fixed seed to numpy:
# herrerab@usc.edu check changes marked MH. June 21 2020.

#from sklearn.preprocessing import LabelEncoder

#from keras.layers import Dense, Dropout, Activation
#from keras.layers.normalization import BatchNormalization
#from keras.regularizers import l2
#from keras.optimizers import Adam
#from keras import backend as K
#from keras.models import Model
#from keras.callbacks import ModelCheckpoint
#from sklearn.utils import shuffle
#from math import ceil
#from keras.utils import np_utils
#from keras import optimizers

import numpy as np
# MH
np.random.seed(1001)
#
from keras.models import Sequential
from keras.models import load_model


# provide the test data_path where you have the x_data i,e, the input vectors
dataset_path = '<provide name>'

loaded = np.load(dataset_path)

xpred = loaded['x_train'][:]




del loaded

print(xpred.shape)
model = Sequential()

model = load_model('model_data_error_train.h5')

#keep the batch_size same as that of the trainiing code
ypred = model.predict(xpred, batch_size = 256, verbose = 0)

#ypred will produce a matrix of size the no. of vectors x 51.
#Now np.argmax(ypred[i]) will provide the actual label/class of the predicted max error, 
#but keep in mind it will be any value from 0 to 50. Its our job to re convert that to 
#any valu between 0 and 1.
