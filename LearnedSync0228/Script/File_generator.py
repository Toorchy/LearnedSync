from sys import argv
import os
import time
# 获取username, 如chinaren


def getusername():
    namelist = os.popen('echo %username%').readlines()
    username = namelist[0].replace("\n", "")
    # 获取当前的username
    return username


try:
    size = argv[1]
except:
    size = input('请输入你想生成的TXT文件大小:')

if size[-2] == 'M':
    filename = size[:-2] + 'MB.txt'
    size = int(size[:-2]) * 1000000
elif size[-2] == "K":
    filename = size[:-2] + 'KB.txt'
    size = int(size[:-2]) * 1000
else:
    filename = str(size[:-1]) + 'B.txt'
    size = int(size[:-1])

print(f'文件名：{filename}')
# 设置文件保存的路径
filepath = "/home/zhouyu/dataset/"
f = open(filepath + filename, 'w')
# f.write('q')
times = int(size / 10)
fileSize = times * 10
size = str(fileSize)

t1 = int(time.time() * 1000000)
# 生成指定大小的TXT档
res = "0123456789"
res *= times
for i in range(times):
    if i % 10000000 == 0 and i >= 10000000:
        print(f'已生成{i//100000}MB数据.')
        # if i == 90000000:
        #     f.write('q')
        # res += "0123456789"
        f.write('0123456789')
    # f.write(res)

# f.write(res)
f.close()

t2 = int(time.time() * 1000000)
print(t2 - t1)

# cc = "532134"
# cc *= 2
# print(cc)

print(f'文件已生成, 文件路径：{filepath + filename}.\n')
