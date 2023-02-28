import torch
import numpy as np
from torch.autograd import Variable
import torch.nn.functional as F

class Net(torch.nn.Module):
    def __init__(self, n_input, n_hidden, n_output):
        super(Net, self).__init__()
        self.hidden1 = torch.nn.Linear(n_input, n_hidden)
        self.hidden2 = torch.nn.Linear(n_hidden, n_hidden)
        # self.hidden3 = torch.nn.Linear(n_hidden, n_hidden)
        self.predict = torch.nn.Linear(n_hidden, n_output)

    def forward(self, input):
        out = self.hidden1(input)
        out = torch.sigmoid(out)
        out = self.hidden2(out)
        out = torch.sigmoid(out)
        # out = self.hidden3(out)
        # out = torch.sigmoid(out)
        out = self.predict(out)
        # out = F.softmax(out)
        return out

x, y = [], []
lines_num = 0

unlimit = 15000
with open("/root/code/region/Verified_Sample", "r+") as f:
    for line in f:
        sample = list(map(lambda x: eval(x), line.split()))
        x.append(sample[:5])
        # print(lines_num)
        y.append(sample[-1])
        lines_num += 1
        if lines_num == unlimit:
            unlimit += 7
            break
print(lines_num)

# print(x, y)

x = torch.tensor(x)
y = torch.tensor(y)

x, y = Variable(x), Variable(y)
# print("\n", x, y)

# Build neural network
try:
    net = torch.load('net.pkl')
except:
    net = Net(5, 20, 3)
print(net)

# Training regression function
optimizer = torch.optim.SGD(net.parameters(), lr=0.2)  # lr is learning rate.
loss_func = torch.nn.CrossEntropyLoss()

# plt.ion()
# plt.show()
t = 0
accuracy = 0
# while True:
for t in range(1000):
    if (t + 1) % 500 == 0:
        print(t + 1, accuracy)
    out = net(x)
    # print(prediction)
    loss = loss_func(out, y)

    optimizer.zero_grad()
    loss.backward()
    optimizer.step()

    if t % 2 == 0:
        # The maximum probability after passing a softmax excitation function is the predicted value.
        prediction = torch.max(F.softmax(out, dim=1), 1)[1]
        pred_y = prediction.numpy().squeeze()
        accuracy = sum(pred_y == y.numpy()) / lines_num


torch.save(net, 'net.pkl')

# print(x, net(x))
# print(net(torch.tensor([[0.8, 0.6, 0.5, 0.4, 2]])))
