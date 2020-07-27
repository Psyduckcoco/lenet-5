#!/usr/bin/env python
# coding: utf-8

# In[7]:


import torch
import torch.nn as nn
import torch.optim as optim
import torchvision


# In[8]:


# 定义是否使用GPU
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
print(device)


# In[9]:

# self, in_channels, out_channels, kernel_size, stride=1,
#                  padding=0, dilation=1, groups=1, bias=True):
class LeNet(nn.Module): #定义网络 pytorch定义网络有很多方式，推荐以下方式，结构清晰
    def __init__(self):
        super(LeNet,self).__init__()
        self.conv1 = nn.Sequential(     #input_size=(1*28*28)
            nn.Conv2d(1,6,5,1,2),       #padding=2，图片大小变为 28+2*2 = 32 (两边各加2列0)，保证输入输出尺寸相同
            nn.ReLU(),
            nn.AvgPool2d(kernel_size = 2 ,stride = 2)   #input_size=(6*28*28)，output_size=(6*14*14)
        )
 
        self.conv2 = nn.Sequential(
            nn.Conv2d(6,16,5),                          #input_size=(6*14*14)，output_size=16*10*10
            nn.ReLU(),
            nn.AvgPool2d(kernel_size = 2,stride = 2)    ##input_size=(16*10*10)，output_size=(16*5*5)
        )
        self.conv3 = nn.Sequential(
            nn.Conv2d(16, 120, 5),  # input_size=(16*5*5)，output_size=120*1*1
            nn.ReLU(),
        )
        # self.fc1 = nn.Sequential(
        #     nn.Linear(16*5*5,120),
        #     nn.ReLU()
        # )
 
        self.fc2 = nn.Sequential(
            nn.Linear(120,84),
            nn.ReLU()
        )
 
        self.fc3 = nn.Linear(84,10)
 
    #网络前向传播过程
    def forward(self,x):
        x = self.conv1(x)
        x = self.conv2(x)
        x = self.conv3(x)
        x = x.view(x.size(0), -1) #全连接层均使用的nn.Linear()线性结构，输入输出维度均为一维，故需要把数据拉为一维
        # x = self.fc1(x)

        x = self.fc2(x)
        x = self.fc3(x)
        return x


# In[10]:
#load data

transform = torchvision.transforms.ToTensor()   #定义数据预处理方式：转换 PIL.Image 成 torch.FloatTensor

train_data = torchvision.datasets.MNIST(root="./data/",    #数据目录，这里目录结构要注意。
                                        train=True,                                     #是否为训练集
                                        transform=transform,                            #加载数据预处理
                                        download=True )                                 #是否下载，这里下载偏慢
test_data = torchvision.datasets.MNIST(root="./data/",
                                        train=False,
                                        transform=transform,
                                        download=True )
 
train_loader = torch.utils.data.DataLoader(dataset = train_data,batch_size = 64,shuffle = True) #数据加载器:组合数据集和采样器
test_loader = torch.utils.data.DataLoader(dataset = test_data,batch_size = 64,shuffle = False)
 
#define loss
net = LeNet().to(device)    #实例化网络，有GPU则将网络放入GPU加速
loss_fuc = nn.CrossEntropyLoss()    #多分类问题，选择交叉熵损失函数
# optimizer = optim.SGD(net.parameters(),lr = 0.001,momentum = 0.9)   #选择SGd，学习率取0.001
optimizer = optim.Adam(net.parameters(),lr = 0.001)
#Star train
EPOCH = 20   #训练总轮数


# net.load_state_dict(torch.load('Lenet.pth'))
for epoch in range(EPOCH):
    sum_loss = 0
    # 数据读取
    for i,data in enumerate(train_loader):
        inputs,labels = data
        inputs, labels = inputs.to(device), labels.to(device)   #有GPU则将数据置入GPU加速


        # 梯度清零
        optimizer.zero_grad()

        # 传递损失 + 更新参数
        output = net(inputs)
        loss = loss_fuc(output,labels)
        loss.backward()
        optimizer.step()

        # 每训练100个batch打印一次平均loss
        sum_loss += loss.item()
        if i % 100 == 99:
            print('[Epoch:%d, batch:%d] train loss: %.05f' % (epoch + 1, i + 1, sum_loss / 100))
            sum_loss = 0.0
 
    correct = 0
    total = 0
 
    for data in test_loader:
        test_inputs, labels = data
        test_inputs, labels = test_inputs.to(device), labels.to(device)
        outputs_test = net(test_inputs)
        _, predicted = torch.max(outputs_test.data, 1)  #输出得分最高的类
        total += labels.size(0) #统计50个batch 图片的总个数
        correct += (predicted == labels).sum()  #统计50个batch 正确分类的个数
 
    print('第%d个epoch的识别准确率为：%d%%' % (epoch + 1, (100 * correct / total)))


# In[11]:


#将训练的权重进行保存
save_path = './Lenet.pth'
torch.save(net.state_dict(), save_path)


# In[ ]:




