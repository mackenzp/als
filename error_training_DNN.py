# -*- coding: utf-8 -*-
"""
Created on Fri Apr  5 14:38:17 2019

@author: ksouv
"""
import numpy as np
from sklearn.preprocessing import LabelEncoder

from keras.models import Sequential
from keras.layers import Dense, Dropout, Activation
from keras.layers.normalization import BatchNormalization
from keras.regularizers import l2
from keras.optimizers import Adam
from keras import backend as K
from keras.models import Model
from keras.models import load_model
from keras.callbacks import ModelCheckpoint
from sklearn.utils import shuffle
from math import ceil
from keras.utils import np_utils
from keras import optimizers

dataset_path = 'error_train.npz'
loaded = np.load(dataset_path)
val_split = 0.20
total_test_vec = 66487
## this is the training dataset size, change the value 66487 as the number of test vectors present in the file
ntr = np.int(np.ceil(0.8*total_test_vec))

xtr = loaded['x_train'][:ntr] 
ytr = loaded['y_train'][:ntr]

xte = loaded['x_train'][ntr:]
yte = loaded['y_train'][ntr:] 
print(np.argmax(yte))
print(np.max(yte))

encoder_tr = LabelEncoder()
encoder_tr.fit(ytr)
encoder_Ytr = encoder_tr.transform(ytr)
ytr = np_utils.to_categorical(encoder_Ytr)

encoder_te = LabelEncoder()
encoder_te.fit(yte)
encoder_Yte = encoder_tr.transform(yte)
yte = np_utils.to_categorical(encoder_Yte)

apnd = np.zeros((13297, 2))
yte = np.concatenate((yte, apnd), axis = 1)
print(yte)


del loaded

print(xtr.shape)
print(ytr.shape)
print(yte.shape)

model = Sequential()
data_cols = xtr.shape[1]

model.add(Dense(400, activation = 'relu', input_shape=(data_cols,)))
model.add(Dense(300, activation = 'relu'))
model.add(Dense(51, activation = 'softmax'))


batch_size = 256
optimizer = Adam(lr = 0.001, decay = 1e-5)
epochs = 30

score = []
filepath = "weights_best_only.hdf5"
model.compile(optimizer = optimizer, loss = 'binary_crossentropy', metrics = ['accuracy'])
checkpoint = ModelCheckpoint(filepath, monitor = 'val_acc', verbose = 1, save_best_only = True, mode = 'max')
callbacks_list = [checkpoint]
model.fit(xtr, ytr, validation_split = val_split, epochs = epochs, batch_size = batch_size, callbacks = callbacks_list, verbose = 0, shuffle = True)
score = model.evaluate(xte, yte, batch_size = batch_size)
model.save('model_data_error_train.h5')

print(score)
