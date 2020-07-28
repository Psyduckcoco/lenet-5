#include <iostream>  
#include <iomanip>
#include <fstream>  
#include <sstream>  
#include "stdlib.h"
#include <cstdlib>
#include<ctime>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
#include "conv.h"
using namespace std;
using namespace cv;
float input[32][32] = {0};

float POOL24_DRAM[16][14][14] = { 0 };

float C567_DRAM[120] = { 0 };

float W_CONV1[6][1][5][5] = { 0 };
float W_CONV3[16][6][5][5] = { 0 };
float W_CONV5[120][16][5][5] = { 0 };

float b_conv1[6] = { 0 };
float b_conv3[16] = { 0 };
float b_conv5[120] = { 0 };

float WFC6[120 * 84] = { 0 };
float WFC7[84 * 10] = { 0 };

float b_fc6[84] = { 0 };
float b_fc7[10] = { 0 };

void read_parameters();


float pic_in[32][32] = {0};//输入
int labels[8952];//标签，用于测试模型参数准确度

#define test_data_set  //是否使用测试数据集宏定义 
int main()
{
	read_parameters();

#ifdef test_data_set
	int result[8952] = { 0 };
	int err_cnt = 0;
	for (int i = 0; i < 8952; i++)
	{
		char str2read_func[200];//
		sprintf_s(str2read_func, "E:\\lenet\\dataset\\test\\%d.jpg", i);
		Mat img_ori = imread(str2read_func, 0);//读取原图
		Mat img_in;
		copyMakeBorder(img_ori, img_in,2, 2, 2, 2, BORDER_CONSTANT, 0);
		for (int row = 0; row < 32; row++)      //行
		{
			for (int col = 0; col < 32; col++)  //列
			{
				pic_in[row][col] = (img_in.at<uchar>(row, col)) / 255.0; //归一化到0 ~ 1 
			}
		}
		top_fun(&pic_in[0][0], &W_CONV1[0][0][0][0], &POOL24_DRAM[0][0][0], b_conv1,1);
		top_fun(&POOL24_DRAM[0][0][0], &W_CONV3[0][0][0][0], &POOL24_DRAM[0][0][0], b_conv3,2);
		top_fun(&POOL24_DRAM[0][0][0], &W_CONV5[0][0][0][0], &C567_DRAM[0], b_conv5,3);
		top_fun(C567_DRAM, WFC6, C567_DRAM, b_fc6,4);
		top_fun(C567_DRAM, WFC7, C567_DRAM, b_fc7,5);
		int max_arg = 0;
		float max = -100000;
		for (int i = 0; i < 10; i++)
		{
			if (C567_DRAM[i] > max)
			{
				max_arg = i;
				max = C567_DRAM[i];
			}
		}
		result[i] = max_arg;
		if (result[i] != labels[i])
			err_cnt++;
		if(i%1000==0)
			cout << i <<"/8952"<< endl;
	}
	cout << "err  = " << err_cnt << endl;
	cout << "Recognition rate is " << (  1-   float(err_cnt) / 8952.0  )   * 100 << '%' << endl;
	
#else
	Mat img_ori = imread("E:\\lenet\\dataset\\test5.bmp", 0);//读取原图
	Mat img_28;
	Mat img_in;
	resize(img_ori, img_28, Size(28, 28));
	copyMakeBorder(img_28, img_in, 2, 2, 2, 2, BORDER_CONSTANT, 255);

	for (int row = 0; row < 32; row++)      //行
	{
		for (int col = 0; col < 32; col++)  //列
		{
			pic_in[row][col] = 1 - (img_in.at<uchar>(row, col)) / 255.0; //归一化到0 ~ 1 
		}
	}
	top_fun(&pic_in[0][0], &W_CONV1[0][0][0][0], &POOL24_DRAM[0][0][0], b_conv1,1);
	top_fun(&POOL24_DRAM[0][0][0], &W_CONV3[0][0][0][0], &POOL24_DRAM[0][0][0], b_conv3,2);
	top_fun(&POOL24_DRAM[0][0][0], &W_CONV5[0][0][0][0], &C567_DRAM[0], b_conv5,3);
	top_fun(C567_DRAM, WFC6, C567_DRAM, b_fc6,4);
	top_fun(C567_DRAM, WFC7, C567_DRAM, b_fc7,5);

	int max_arg = 0;
	float max = -100000;
	for (int i = 0; i < 10; i++)
	{
		if (C567_DRAM[i] > max)
		{
			max_arg = i;
			max = C567_DRAM[i];
		}
	}
	cout <<"the number is " <<max_arg<< "  " << max << endl;
	imshow("原始图", img_ori);
	imshow("padding图", img_in);
	waitKey(10);
#endif
	
	while (1);
	return 0;

}


void read_mdl(const char* filename, float* para_array)
{

	FILE* fp;
	fp = fopen(filename, "r+");
	int i = 0;
	int b;
	while (!feof(fp))
	{
		b = fscanf(fp, "%f\r\n", para_array + i);
		i++;
	}
	fclose(fp);
}
void read_label(const char* filename, int* para_array)
{

	FILE* fp;
	fp = fopen(filename, "r+");
	int i = 0;
	int b;
	while (!feof(fp))
	{
		b = fscanf(fp, "%d\r\n", para_array + i);
		i++;
	}
	fclose(fp);
}
void read_parameters()
{
	///////////读取weight
	read_mdl("E:\\lenet\\python_code\\parameter\\conv1.0.weight.txt", &W_CONV1[0][0][0][0]);
	read_mdl("E:\\lenet\\python_code\\parameter\\conv2.0.weight.txt", &W_CONV3[0][0][0][0]);
	read_mdl("E:\\lenet\\python_code\\parameter\\conv3.0.weight.txt", &W_CONV5[0][0][0][0]);

	read_mdl("E:\\lenet\\python_code\\parameter\\fc2.0.weight.txt", &WFC6[0]);
	read_mdl("E:\\lenet\\python_code\\parameter\\fc3.weight.txt", &WFC7[0]);

	//读取bias
	read_mdl("E:\\lenet\\python_code\\parameter\\conv1.0.bias.txt", &b_conv1[0]);
	read_mdl("E:\\lenet\\python_code\\parameter\\conv2.0.bias.txt", &b_conv3[0]);
	read_mdl("E:\\lenet\\python_code\\parameter\\conv3.0.bias.txt", &b_conv5[0]);

	read_mdl("E:\\lenet\\python_code\\parameter\\fc2.0.bias.txt", &b_fc6[0]);
	read_mdl("E:\\lenet\\python_code\\parameter\\fc3.bias.txt", &b_fc7[0]);

	read_label("E:\\lenet\\dataset\\test.txt", &labels[0]);
}

