from pynq import Overlay
from pynq import Xlnk
import numpy as np
import cv2
from PIL import Image as PIL_Image
from PIL import ImageEnhance
from PIL import ImageOps
from scipy import misc

overlay = Overlay('lenet.bit')
top = overlay.top_fun_0

# 寄存器地址
# CTRL           0x00
# IN_DRAM_DATA      0x10
# W_DRAM_DATA       0x18
# OUT_DRAM_DATA      0x20
# BIAS_DRAM_DATA     0x28
# LAYER_DATA        0x30
def top_fun(pic_in, w_in, out, bias, layer):
    top.write(0x10,pic_in)#pic_in
    top.write(0x18,w_in)#w
    top.write(0x20,out)#out
    top.write(0x28,bias)#bias
    top.write(0x30,layer)#layer
    top.write(0x00,0x01)#写入1 将ap_start置1 启动
    while(top.read(0)!=0x04):#等待完成
        a=1
    return 0

#########################################开辟内存用来放权重 并把地址发给IP用
xlnk = Xlnk()
input_pic = xlnk.cma_array(shape=(32*32,), dtype=np.float32)
POOL24_DRAM = xlnk.cma_array(shape=(16*14*14,), dtype=np.float32)
C5_DRAM = xlnk.cma_array(shape=(120,), dtype=np.float32)
C6_DRAM = xlnk.cma_array(shape=(84,), dtype=np.float32)
C7_DRAM = xlnk.cma_array(shape=(10,), dtype=np.float32)
W_CONV1 = xlnk.cma_array(shape=(6*5*5,), dtype=np.float32)
W_CONV3 = xlnk.cma_array(shape=(16*6*5*5,), dtype=np.float32)
W_CONV5 = xlnk.cma_array(shape=(120*16*5*5,), dtype=np.float32)
b_conv1 = xlnk.cma_array(shape=(6,), dtype=np.float32)
b_conv3 = xlnk.cma_array(shape=(16,), dtype=np.float32)
b_conv5 = xlnk.cma_array(shape=(120,), dtype=np.float32)
WFC6 = xlnk.cma_array(shape=(10080,), dtype=np.float32)
WFC7 = xlnk.cma_array(shape=(840,), dtype=np.float32)
b_fc6 = xlnk.cma_array(shape=(84,), dtype=np.float32)
b_fc7 = xlnk.cma_array(shape=(10,), dtype=np.float32)

W_CONV1_buff = np.loadtxt('parameter/conv1.0.weight.txt')
W_CONV3_buff = np.loadtxt('parameter/conv2.0.weight.txt')
W_CONV5_buff = np.loadtxt('parameter/conv3.0.weight.txt')

b_conv1_buff = np.loadtxt('parameter/conv1.0.bias.txt')
b_conv3_buff = np.loadtxt('parameter/conv2.0.bias.txt')
b_conv5_buff = np.loadtxt('parameter/conv3.0.bias.txt')

WFC6_buff = np.loadtxt('parameter/fc2.0.weight.txt')
WFC7_buff = np.loadtxt('parameter/fc3.weight.txt')

b_fc6_buff = np.loadtxt('parameter/fc2.0.bias.txt') 
b_fc7_buff = np.loadtxt('parameter/fc3.bias.txt')
for i in range(6*5*5):
        W_CONV1[i] = W_CONV1_buff[i]
for i in range(16*6*5*5):
        W_CONV3[i] = W_CONV3_buff[i]
for i in range(120*16*5*5):
        W_CONV5[i] = W_CONV5_buff[i]
        
for i in range(6):
        b_conv1[i] = b_conv1_buff[i]
for i in range(16):
        b_conv3[i] = b_conv3_buff[i]        
for i in range(120):
        b_conv5[i] = b_conv5_buff[i]          
        
for i in range(10080):
        WFC6[i] = WFC6_buff[i]    
for i in range(840):
        WFC7[i] = WFC7_buff[i]              
        
for i in range(84):
        b_fc6[i] = b_fc6_buff[i]     
for i in range(10):
        b_fc7[i] = b_fc7_buff[i]     
        
        
#########################################加载图片    
img_load = PIL_Image.open('vali_data/test5.bmp').convert("L") 
img_load = img_load.resize((28, 28),PIL_Image.ANTIALIAS)

img_numpy = np.asarray(img_load)

img_numpy= np.pad(img_numpy, ((2, 2), (2, 2)), 'constant', constant_values=(255, 255))
input_pic.physical_address 
for i in range(32):
    for j in range(32):
        input_pic[i*32+j] = 1 - float(img_numpy[i][j]) / 255.0;  
        
        
#########################################开展卷积部分            
    #第一次卷积
top_fun( input_pic.physical_address, W_CONV1.physical_address, POOL24_DRAM.physical_address, b_conv1.physical_address, 1)
    #第二次卷积
top_fun( POOL24_DRAM.physical_address, W_CONV3.physical_address, POOL24_DRAM.physical_address, b_conv3.physical_address, 2)
    #第三次卷积
top_fun( POOL24_DRAM.physical_address, W_CONV5.physical_address, C5_DRAM.physical_address, b_conv5.physical_address, 3)
    #第一次全连接
top_fun( C5_DRAM.physical_address, WFC6.physical_address, C6_DRAM.physical_address, b_fc6.physical_address, 4)
    #第二次全连接
top_fun( C6_DRAM.physical_address, WFC7.physical_address, C7_DRAM.physical_address, b_fc7.physical_address, 5)

#########################################找到最大值   
max=0
max_locotion=0
for i in range(10):
    if(C7_DRAM[i]>max):
        max = C7_DRAM[i]
        max_locotion=i

print("检测结果：%d"%max_locotion)

img_load