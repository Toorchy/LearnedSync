from sys import argv
import os
import time


def getusername():
    namelist = os.popen('echo %username%').readlines()
    username = namelist[0].replace("\n", "")
    return username


try:
    size = argv[1]
except:
    size = input('Please enter the size of TXT file you want to generate:')

if size[-2] == 'M':
    filename = size[:-2] + 'MB.txt'
    size = int(size[:-2]) * 1000000
elif size[-2] == "K":
    filename = size[:-2] + 'KB.txt'
    size = int(size[:-2]) * 1000
else:
    filename = str(size[:-1]) + 'B.txt'
    size = int(size[:-1])

print(f'file：{filename}')
# Set the path to save the file
filepath = ""
f = open(filepath + filename, 'w')
# f.write('q')
times = int(size / 10)
fileSize = times * 10
size = str(fileSize)

t1 = int(time.time() * 1000000)
res = "0123456789"
res *= times
for i in range(times):
    if i % 10000000 == 0 and i >= 10000000:
        print(f'{i//100000}MB.')
        f.write('0123456789')

f.close()

t2 = int(time.time() * 1000000)
print(t2 - t1)

# cc = "532134"
# cc *= 2
# print(cc)

print(f'File generated, file path:{filepath + filename}.\n')
