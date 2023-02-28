import os
import sys

min_size = 1000000
max_size = 0
min_name = ""
max_name = ""
src = sys.argv[1]

num0 = 0
num1 = 0
num2 = 0
num3 = 0
num4 = 0
num5 = 0
for root, dirs, files in os.walk(src, topdown=False):
    for name in files:
        size = os.stat(root + "/" + name).st_size
        if max_size < size:
            max_size = size
            max_name = root + "/" + name
        if min_size > size:
            min_size = size
            min_name = root + "/" + name
        if size < 1000:
            num0 += 1
        elif size < 10000:
            num1 += 1
        elif size < 100000:
            num2 += 1
        elif size < 1000000:
            num3 += 1
        elif size < 10000000:
            num4 += 1
        else:
            num5 += 1

print(num0, "less than 1KB,")
print(num1, "less than 10KB,")
print(num2, "less than 100KB,")
print(num3, "less than 1MB,")
print(num4, "less than 10MB,")
print(num5, "more than 10MB.")
os.system("ls {} -lR | grep 'r' | wc -l".format(src))
print(max_size, max_name)
print(min_size, min_name)
os.system("du -shb {}".format(src))
# for name in dirs:
#     print(os.path.join(root, name))
