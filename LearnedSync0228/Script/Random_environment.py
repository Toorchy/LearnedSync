import os
import random
import time


while 1:
    Siz = round(random.uniform(0.1, 10), 3)
    Rdd = round(random.random(), 3)
    Bdw = round(random.uniform(48, 96), 1)
    RTT = round(random.uniform(10, 100), 1)
    Trd = random.randint(0, 10)

    print(Siz, Rdd, Bdw, RTT, Trd)
    os.system("python3 ./File_generator.py " + str(int(Siz)) + "MB")
    os.system("python3 ./File_replace.py " + str(Rdd))
    os.system("sudo tc qdisc change dev enp3s0 root tbf rate "
              + str(Bdw) + "mbit latency 50ms burst 15kbit")
    os.system("sudo tc qdisc change dev enp3s0 root netem delay"+ str(RTT) + "ms")
    os.system("stress -c " + str(Trd))


    t1 = time.time()
    os.system("FullSync.run {}".format(Siz))
    t2 = time.time()
    os.system("FSCSync.run {}".format(Siz))
    t3 = time.time()
    os.system("CDCSync.run {}".format(Siz))
    t4 = time.time()

    f = open("Sample.txt", "a+")

    t5, t6, t7 = t2 - t1, t3 - t2, t4 - t3
    if t5 == min([t5, t6, t7]):
        f.write("{:.3} {:.3} {:.3} {:.3} {:.3} 0\n".format(
            Siz/10, Rdd, Bdw/96, RTT/100, Trd/10))
    elif t6 < t7:
        f.write("{:.3} {:.3} {:.3} {:.3} {:.3} 1\n".format(
            Siz/10, Rdd, Bdw/96, RTT/100, Trd/10))
    else:
        f.write("{:.3} {:.3} {:.3} {:.3} {:.3} 2\n".format(
            Siz/10, Rdd, Bdw/96, RTT/100, Trd/10))
    f.close()

