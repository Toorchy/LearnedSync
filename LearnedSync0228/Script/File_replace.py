# Usage:
#     python3 ./Scripter/File_replace.py

import os
from sys import argv
from os import path

src = "/home/zhouyu/test_dataset/500MBfile"
dst = "/home/zhouyu/test_dataset/500MBfile_20r"
delta = 0.2
index = 0.8

with open(dst, "wb+") as newfile:
    oldfile = open(src, "rb")
    byte = path.getsize(src)
    byte -= byte % 10
    print(byte)
    newfile.write(oldfile.read(int(index * byte)))
    for i in range(int(byte * delta / 10)):
        newfile.write(b'AFKPUafkpu')
    # newfile.write(b' ')
    oldfile.seek(int((index + delta) * byte))
    newfile.write(oldfile.read())
    oldfile.close()
