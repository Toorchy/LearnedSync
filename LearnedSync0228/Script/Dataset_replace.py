# Usage:
#     python3 Dataset_replace.py

from os import path
from os import walk

src = "/home/zhouyu/dataset/"
dst = "/home/zhouyu/test_dataset/dataset_0.75r/"
delta = 0.75
index = 0.25

for root, dirs, files in walk(src, topdown=False):
    for name in files:
        with open(dst + name, "wb+") as newfile:
            oldfile = open(root + name, "rb")
            byte = path.getsize(root + name)
            newfile.write(oldfile.read(int(index * byte)))
            for i in range(int(byte * delta / 10)):
                newfile.write(b'AFKPUafkpu')
            oldfile.seek(int((index + delta) * byte))
            newfile.write(oldfile.read())
            oldfile.close()
