from os import system as ss
import time
import argv

dataset_num = int(os.popen("ls {} -lR | grep 'rw' | wc -l"
                           .format(src)).read()[:-1])
dataset_dig = ":0" + str(1 + int(math.log(dataset_num - 1, 10)))

t1 = int(time.time() * 1000000)

for i in range(dataset_num):
    ss("cat ./linux-5.8.13.0/{dataset_dig} >> test1".format(i))

t2 = int(time.time() * 1000000)
print(t2 - t1)
