import torch
from os import system

class Net(torch.nn.Module):
    def __init__(self, n_input, n_hidden, n_output):
        super(Net, self).__init__()
        self.hidden1 = torch.nn.Linear(n_input, n_hidden)
        self.hidden2 = torch.nn.Linear(n_hidden, n_hidden)
        self.predict = torch.nn.Linear(n_hidden, n_output)

    def forward(self, input):
        out = self.hidden1(input)
        out = torch.sigmoid(out)
        out = self.hidden2(out)
        out = torch.sigmoid(out)
        out = self.predict(out)
        return out

code_path = ""

region = {}
Siz = [0.01, 0.05, 0.1, 0.14, 0.18]
Rdd = [0, 0.1, 0.4, 0.6, 0.8]
Bwd = [0.125, 0.16, 0.2, 0.25, 0.33, 0.5]
RTT = [0, 0.25, 0.5, 0.75]
Trd = [0, 0.2, 0.4, 0.6, 0.8]
Env_info = [Siz, Rdd, Bwd, RTT, Trd]

# Initialize Partition
for i in Siz:
    for j in Rdd:
        for k in Bwd:
            for m in RTT:
                for n in Trd:
                    # Partitioning stores the sync count ,average time of three methods and chunk status, respectively.
                    region[(i, j, k, m, n)] = [[0, 0, 0], [0, 0, 0], [0, 0]]
                    # Partitioning stores the sync records of three methods, respectively.
                    for p in range(3):
                        with open(code_path + '_'.join(map(str, [i, j, k, m, n, p])), "w+"):
                            pass
with open(code_path + "Verified_Sample", "w"):
    pass
with open(code_path + "Label", "w"):
    pass


def locate(x):
    ans = []

    for i in range(5):
        for j in range(len(Env_info[i])):
            if x[i] < Env_info[i][j]:
                ans.append(Env_info[i][j-1])
                break
        else:
            ans.append(Env_info[i][-1])

    return tuple(ans)

system("rm net.pkl")

lines_num = 0
with open("/root/code/Sample_Collection", "r") as f:
    for line in f:

        task = list(map(lambda x: eval(x), line.split()))
        lct = region[locate(task)]
        if lct[2][0]:
            dcs = torch.argmax(net(torch.tensor(task[:5]))
        else:
            for i in range(3):
                # Verify whether it is a budding partition.
                if lct[0][i] < 10:
                    dcs = i
                    break
            else:
                # Been identified as a mature partition.
                dcs = = torch.argmax(net(torch.tensor(task[:5]))
                lct[2][0] = 1

        lct[1][dcs] = (lct[0][dcs] * lct[1][dcs] + task[5 + dcs])\
            / (lct[0][dcs] + 1)
        lct[0][dcs] += 1
        region[locate(task)] = lct
        if lct[2][0] == 2:
            with open(code_path + "Verified_Sample", "a+") as s:
                s.write(line)
        else:
            with open(code_path + '_'.join(map(str, list(locate(task)) + [dcs])), "a+") as s:
                s.write(line)
        with open(code_path + "Label", "a+") as s:
            s.write(str(dcs) + "\n")

        lines_num += 1
        # if (lines_num + 1) % 1000 == 0:
        #     print(lines_num)
        chunks = 0
        if (lines_num + 1) % 1000 == 0:
            print(lines_num + 1)
            for i in region.keys():
                if region[i][2][0] == 1:
                    rg_tm = sorted(region[i][1])
                    # print(rg_tm)
                    if rg_tm[0] < 0.9 * rg_tm[1]:
                        region[i][2][0] = 2
                        for j in range(3):
                            if rg_tm[0] == region[i][1][j]:
                                region[i][2][1] = j
                                break
                        system("cat " + code_path + '_'.join(map(str, list(locate(i)) + [region[i][2][1]]))
                               + " >> " + code_path + "Verified_Sample")
                        system("cat /dev/null > " + code_path + '_'.join(map(str,
                               list(locate(i)) + [region[i][2][1]])))
            system("python3 ./LS-Learning.py")
            # print(chunks)

print(lines_num)

print(len(region))

# Show results.
total_time = 0
for j in region.values():
    for p in range(3):
        total_time += j[0][p] * j[1][p]

print(total_time)
