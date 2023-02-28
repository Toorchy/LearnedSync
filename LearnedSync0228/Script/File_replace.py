# Usage:
#     python3 ./Scripter/File_replace.py

import os
from sys import argv
from os import path

src = ""
dst = ""
delta = 0
index = 0

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
