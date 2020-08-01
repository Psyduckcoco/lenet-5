# 2020年新⼯科联盟-Xilinx暑期学校（Summer School）项⽬

## 项⽬概要

手写识别是常见的图像识别任务。计算机通过手写体图片来识别出图片中的字，与印刷字体不同的是，不同人的手写体风格迥异，大小不一， 造成了计算机对手写识别任务的一些困难。本次任务目标是在FPGA上部署手写识别网络lenet-5,以其实现更高效率更低功耗的设计。

本次项目在pynq-z2板卡上实现lenet-5网络，并在裸机和jupyter上分别编写代码测试。

#### 使用工具版本：

vivado2018.3

Vivado HLS 2018.3

Visual Studio 2019

PyCharm Community Edition 2020.1.3 x64

#### 模型部署环境：

pytoch1.0

cuda10.0

#### 板卡型号

pynq-z2

#### 外设列表

SD卡16GB

#### 仓库⽬录介绍

Sourcecode/C_prj：用于网络复现的C代码

Sourcecode/HLS_prj：HLS代码

Sourcecode/jupyter_files：jupyter端pyhton文件

Sourcecode/python_code：模型部署py文件

Sourcecode/vivado_prj：包含裸机SDK代码、block design 的tcl文件

ExecutableFiles：包含bit流文件

#### 作品照片

![image-20200801113440838](https://gitee.com/t2w1997/img-bed/raw/master/img/20200801113440.png)

![Snipaste_2020-08-01_11-35-36](https://gitee.com/t2w1997/img-bed/raw/master/img/20200801113548.png)

关于各种文件代码的具体使用方法请看各文件夹下的readme文件。

