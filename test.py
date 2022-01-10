import os
import threading
import random
import time
import glob
import numpy as np
import re
import collections
import tensorflow as tf
from tensorflow.keras.models import Sequential
from tensorflow.keras import layers
from tensorflow.keras import optimizers

def getFormDeque(dq):
    ret = list()
    for i in range(len(dq)):
        tmp = np.array(dq[i])
        ret.append(tmp)
    ret = np.array(ret)
    ret = np.expand_dims(ret,axis=3)
    return ret

def memoryCreate(memory):
    path = 'step_gene/*.txt'
    frames = None
    last_frame = None
    last_reward = float(0.0)
    for filepath in sorted(glob.iglob(path), key=lambda x: int(x[15:-4])):
        print(filepath)
        f = open(filepath, 'r')
        line = f.readlines()
        cur = line[1:19]
        reward = float(line[19])
        action = np.array(list(map(int, re.split(',|\n', line[20][:-1]))))
        snake_len = int(line[21])
        ret = list()
        for i in range(len(cur)):
            ret.append(list(map(int, re.split(',|\n', cur[i])[:-1])))
        ret = np.array(ret)

        if frames == None:
            frames = collections.deque([ret]*4)
        else:
            frames.append(ret)
            frames.popleft()
        
        if last_frame == None or last_reward == -0.8:
            last_frame = frames.copy()
        else:
            memory.append([getFormDeque(last_frame),action.argmax(),getFormDeque(frames),reward,(reward == -0.8)])
            if len(memory) > 200:
                memory.pop(0)
            last_frame = frames.copy()

        if len(memory) > 0 :
            #print(memory[-1])
            pass
        last_reward = np.copy(reward)
    #print(len(memory))
        
def getBatch(memory,model):
    batch_size = min(len(memory),2)
    experience = np.array(random.sample(memory, batch_size),dtype=object)
    
    states = list(experience[:,0])
    states = np.array(states)
    actions = np.cast['int'](experience[:,1])
    states_ = list(experience[:,2])
    states_ = np.array(states_)
    rewards = experience[:,3].repeat(4).reshape((batch_size, 4,1))
    ends = experience[:,4].repeat(4).reshape((batch_size, 4,1))

    print(states.shape)
    x = np.concatenate([states,states_],axis = 0)
    x = x.reshape(batch_size*8,18,18,1)
    y = model(x)
    y = np.array(y).reshape(batch_size*2,4,4)
    Q_next = np.max(y[batch_size:],axis=2).repeat(4).reshape((batch_size,4,4))
    print(Q_next)

    delta = np.zeros((batch_size, 4,4))
    delta[np.arange(batch_size), :,actions] = 1
    states = states.reshape(batch_size*4,18,18,1)
    target = (1 - delta) * y[:batch_size] + delta*(rewards + 0.9*(1-ends)*Q_next)
    target = target.reshape(batch_size*4,4)
    print(model.input_shape)
    print(states.shape)
    print(target.shape)
    return states.astype(np.float32), target.astype(np.float32)
    #print(y.shape)
    #print(np.max(y, axis=1).repeat(4).reshape(batch_size*8,4))
    

def modelCreate():
    input_shape = (4, 18, 18, 1)
    model = Sequential()
    model.add(layers.Conv2D(16, kernel_size=(3, 3), strides=(1, 1),
              activation='relu', dilation_rate=2, input_shape=input_shape[1:]))
    model.add(layers.Conv2D(32, kernel_size=(3, 3), strides=(1, 1),
              activation='relu', dilation_rate=2, input_shape=input_shape[1:]))
    model.add(layers.Flatten())
    model.add(layers.Dense(256,activation='relu'))
    model.add(layers.Dense(4))
    model.summary()
    model.compile(optimizers.RMSprop(), 'MSE')

    return model


memory = list()
test = modelCreate()
print(test.output_shape)
memoryCreate(memory)
print(len(memory))
getBatch(memory,test)

inputs, targets = getBatch(memory, test)
test.train_on_batch(inputs, targets)

