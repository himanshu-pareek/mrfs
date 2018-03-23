# -*- coding: utf-8 -*-
'''

@author: himanshu
'''

import numpy as np
import pandas as pd
import random
import math
import matplotlib.pyplot as plt

# Reading data
data = pd.read_csv ('kc_house_data.csv');
test_size = 0.2

y = data['price']
X = data.drop (['price'], axis = 1)

num_examples = X.shape[0];
num_features = X.shape[1];

len_test = (int )(test_size * num_examples)
len_train = num_examples - len_test

# Convert to numpy arrays
X = X.as_matrix ()
y = y.as_matrix ()

# print (type (X), X[:10])
# print (type (y), y[:10])

# Train test split
l1 = random.sample(range(0,num_examples), len_train)
l2 =[]
for i in  range(num_examples):
    l2.append(i)
l3 = (list(set(l2) - set(l1)))

X_train = np.zeros (shape = (len_train, num_features), dtype = float, order = 'C')
X_test = np.zeros (shape = (len_test, num_features), dtype = float, order = 'C')
y_train = np.zeros (shape = len_train, dtype = float)
y_test = np.zeros (shape = len_test, dtype = float)

for i in range (len_train):
    X_train[i] = X[l1[i]]
    y_train[i] = y[l1[i]]
    
for i in range (len_test):
    X_test[i] = X[l3[i]]
    y_test[i] = y[l3[i]]
    
# Feature scaling on training set
mean_x = np.mean (X_train, axis = 0)
std_x = np.std (X_train, axis = 0)

# print ('X mean: ', mean_x)
# print ('X std: ', std_x)
X_train = (X_train - mean_x) / std_x
X_test = (X_test - mean_x) / std_x

# print (X_train[:10])

# Inserting column of ones at the beginning
X_train = np.concatenate((np.ones(len_train)[:, np.newaxis], X_train), axis=1)
X_test = np.concatenate((np.ones(len_test)[:, np.newaxis], X_test), axis=1)

# print ('X_train: ', X_train[:10])
# print ('X_test: ', X_test[:10])

# parameters
# theta = np.zeros (X_train.shape[1])
# learning rate
alpha = 0.005

def calculate_mse (X, y, theta):
    err = np.square (np.matmul(X, theta) - y)
    mse = np.sum (err) / (2 * X.shape[0])
    return mse

def calculate_rmse (X, y, theta):
    return math.sqrt (2 * calculate_mse (X, y, theta))

num_iterations = 5000
mses = []
lam = 2

def gradient_descent (X, y, lam, alpha):
    m = X.shape[0]
    n = X.shape[1]
    theta = np.zeros (n)
    reg_error = (lam / (2 * m)) * np.sum (np.square (theta))
    mses.append (calculate_mse (X, y, theta) + reg_error)
    for i in range (num_iterations):
        delta = (np.matmul (np.matmul (X, theta) - y, X) + lam * theta) / m
        theta = theta - alpha * delta
        reg_error = (lam / (2 * m)) * np.sum (np.square (theta)[1:])
        mses.append (calculate_mse(X, y, theta) + reg_error)
        
    return theta
        
theta = gradient_descent (X_train, y_train, 0, alpha)

mses = mses[500:]
plt.plot (range (len (mses)), mses)
plt.title ('Linear Regression without regularization')
plt.xlabel ('Number of iterations')
plt.ylabel ('Mean squared error')
plt.show ()

print ("Theta: ", theta)
print ("Mean square error: ", mses[-1])
# print (mses)

print ('')
print ('')
print ('Effect of different regularization parameters')
lams = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
rmses = []
thetas = []
for lam in lams:
    theta = gradient_descent (X_train, y_train, lam, alpha)
    rmses.append (calculate_rmse (X_test, y_test, theta))
    thetas.append (theta)

plt.plot (lams, rmses)
plt.title ('Linear Regression with regularization')
plt.xlabel ('lambda')
plt.ylabel ('Root mean square error')
plt.show ()
print ('Thetas: ', thetas)
