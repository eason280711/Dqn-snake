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
    ret = np.expand_dims(ret, axis=3)
    return ret

def getBatch(memory, model):
    batch_size = min(len(memory), 60)
    experience = np.array(random.sample(memory, batch_size), dtype=object)

    states = list(experience[:, 0])
    states = np.array(states)
    actions = np.cast['int'](experience[:, 1])
    states_ = list(experience[:, 2])
    states_ = np.array(states_)
    rewards = experience[:, 3].repeat(4).reshape((batch_size, 4, 1))
    ends = experience[:, 4].repeat(4).reshape((batch_size, 4, 1))

    x = np.concatenate([states, states_], axis=0)
    x = x.reshape(batch_size*8, 18, 18, 1)
    y = model(x)
    y = np.array(y).reshape(batch_size*2, 4, 4)
    Q_next = np.max(y[batch_size:], axis=2).repeat(
        4).reshape((batch_size, 4, 4))

    delta = np.zeros((batch_size, 4, 4))
    delta[np.arange(batch_size), :, actions] = 1

    target = (1 - delta) * y[:batch_size] + \
        delta*(rewards + 0.9*(1-ends)*Q_next)

    states = states.reshape(batch_size*4,18,18,1)
    target = target.reshape(batch_size*4,4)
    #return states, target
    #print(y.shape)
    #print(np.max(y, axis=1).repeat(4).reshape(batch_size*8,4))
    return states.astype(np.float32), target.astype(np.float32)


def modelCreate():
    input_shape = (4, 18, 18, 1)
    model = Sequential()
    model.add(layers.Conv2D(16, kernel_size=(3, 3), strides=(1, 1),
              activation='relu', dilation_rate=2, input_shape=input_shape[1:]))
    model.add(layers.Conv2D(32, kernel_size=(3, 3), strides=(1, 1),
              activation='relu', dilation_rate=2, input_shape=input_shape[1:]))
    model.add(layers.Flatten())
    model.add(layers.Dense(256, activation='relu'))
    model.add(layers.Dense(4))
    model.summary()
    model.compile(optimizers.RMSprop(), 'MSE')

    return model


def job():
    os.system("start cmd /k sn.exe")
    #os.system("sn.exe")


def choose_action(st, agent,rate,last_frame):
    step = [72, 80, 75, 77]
    path = 'step.txt'
    f = open(path, 'w')
    if np.random.uniform() < rate:
        q = agent(last_frame)
        f.write(str(step[np.argmax(q[0])]))
    else:
        f.write(str(step[random.randint(0, 3)]))
        
    f.close()

def normalize(v):
    norm = np.linalg.norm(v)
    if norm == 0: 
        return v
    return v / norm


def run_snake():
    memory = list()
    agent = modelCreate()
    step = 1
    path = 'nums.txt'
    rate = 0.65
    ep = 3000
    cnt = 1
    for i in range(ep):
        loss = 0.0
        txtpath = 'step_gene'
        frames = None
        last_frame = None
        last_reward = float(0.0)
        rew = float(0.0)
        while True:
            while True:
                try:
                    with open('step.txt', 'r') as f:
                        a = f.readline()
                        if int(a)  == 0:
                            break
                except:
                    time.sleep(0.01)
                    continue
                time.sleep(0.01)

            filepath = txtpath + '/step_' + str(step-1) + '.txt'
            f = open(filepath, 'r')
            line = f.readlines()
            cur = line[1:19]
            reward = float(line[19])
            rew += reward
            action = np.array(list(map(int, re.split(',|\n', line[20][:-1]))))
            snake_len = int(line[21])
            ret = list()
            for i in range(len(cur)):
                ret.append(list(map(int, re.split(',|\n', cur[i])[:-1])))
            ret = np.array(ret)
            ret = normalize(ret)
            if frames == None:
                frames = collections.deque([ret]*4)
            else:
                frames.append(ret)
                frames.popleft()

            if last_frame == None or last_reward == -10.0:
                last_frame = frames.copy()
            else:
                memory.append([getFormDeque(last_frame),action.argmax(),getFormDeque(frames),reward,(reward == -10.0)])
                if len(memory) > 2000:
                    memory.pop(0)
                last_frame = frames.copy()

            if len(memory) > 0 :
                #print(memory[-1])
                pass
            last_reward = np.copy(reward)
            f.close()
            choose_action(step, agent,rate,getFormDeque(last_frame))
            step += 1
            fp = 'step_gene/*.txt'
            T = False
            f = open(filepath, 'r')
            line = f.readlines()
            if float(line[19]) == -10.0:
                T = True
            if T == True:
                break

            if step > 60 and step % 5 == 0:
                inputs, targets = getBatch(memory, agent)
                loss += float(agent.train_on_batch(inputs, targets))
            if step > 100 and step % 50 == 0 and rate < 1.0:
                rate += 0.02
            f.close()
        print()
        print('--------------INFO--------------------')
        summary = 'Episode {:5d}/{:5d} | Loss {:8.4f} | Rate {:.2f} | Snake Len {:4d} | Reward {:8.4f}'
        print(summary.format(
            ep, cnt, loss, rate, snake_len , rew
        ))
        print('--------------------------------------')
        print()
        cnt += 1
    agent.save('model/Final_model')
        
print('continue')
print("Num GPUs Available: ", len(tf.config.experimental.list_physical_devices('GPU')))
print('--------------------------------------')
time.sleep(2)
t = threading.Thread(target=job)
t.start()
run_snake()
os.system('taskkill /f /im sn.exe')

input("Press Enter to continue...")
