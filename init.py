import shutil
import os


pathTest = r"./step_gene"
if os.path.exists(pathTest):
    if len(os.listdir(pathTest)) != 0:
        shutil.rmtree(pathTest)
if not os.path.exists(pathTest):
    os.mkdir("step_gene")

path = './step.txt'
f = open(path, 'w+')
f.write("0")

path = './nums.txt'
f = open(path, 'w+')
f.write("0")