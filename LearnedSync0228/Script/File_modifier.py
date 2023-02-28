import os
import math
import sys


def visitDir(path):
    for p in range(li):
        #pathname = os.path.join(path, "0" + str(p))
        pathname = os.path.join(path,  format(p, "0" + li_dig))
        byte = os.path.getsize(pathname)
        with open(dest + format(p, "0" + li_dig), "wb+") as fo:
            f = open(pathname, "rb")
            if delta < 0:
                fo.write(f.read(int(byte * (0.5 + delta))))
                f.seek(int(byte * (0.5 - delta)))
                fo.write(f.read())
            else:
                fo.write(f.read(int(byte * (0.5 - delta / 2))))
                # fo.write(f.read(int(byte / 2)))
                for i in range(int(byte * delta / 2)):
                    fo.write(b'01')
                if delta:
                    fo.write(b'0')
                f.seek(int(byte * (0.5 + delta / 2)))
                fo.write(f.read())
            f.close()


delta = 0.5
src = sys.argv[1]
dest = sys.argv[2]


li = int(os.popen("ls {} -l | grep 'r' | wc -l"
                  .format(src)).read()[:-1])
li_dig = str(1 + int(math.log(li - 1, 10)))

rm = "rm -r " + dest + "*"
os.system(rm)
visitDir(src)
print(delta, src.split('/')[-2], dest.split('/')[-2])
