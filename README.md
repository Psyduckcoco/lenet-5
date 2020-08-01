# lenet-5
包含C代码、HLS代码、vivado工程、SDK裸机代码、jupyter代码、相应bit流文件、TCL文件、hwh文件、验证集、测试图像等。



SDK裸机代码执行结果如下：

![image-20200801100830626](https://gitee.com/t2w1997/img-bed/raw/master/img/20200801100830.png)

跑完验证集一共消耗31s左右，总时间帧率在285帧左右（验证集在一个二进制文件里），由于模型更改了部分结构，不再是最原本的lenet5，所以准确率有所上升。

jupyter端执行代码结果如下：

![jupyter端结果1](https://gitee.com/t2w1997/img-bed/raw/master/img/20200801101053.png)

![jupyter端结果2](https://gitee.com/t2w1997/img-bed/raw/master/img/20200801101056.png)

![jupyter端结果3](https://gitee.com/t2w1997/img-bed/raw/master/img/20200801101100.png)



关于各种文件代码的具体使用方法请看各文件夹下的readme文件。

