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

float S2_DRAM[6][14][14] = { 0 };

float S4_DRAM[16][5][5] = { 0 };

float C5_DRAM[120] = { 0 };
float F6_DRAM[84] = { 0 };
float F7_DRAM[10] = { 0 };

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

float pic_in[32][32] = {0};
//uint8_t pic1[32][32] = { 0 };;
int main()
{
	read_parameters();
	Mat img_ori = imread("..\\..\\dataset\\test0.bmp", 0);//读取原图
	Mat img_28;
	Mat img_in;
	resize(img_ori, img_28, Size(28, 28));
	copyMakeBorder(img_28, img_in, 2, 2, 2, 2, BORDER_CONSTANT,255);

	for (int row = 0; row < 32; row++)      //行
	{
		for (int col = 0; col < 32; col++)  //列
		{
			//pic1[row][col] =255- (int)(img_in.at<uchar>(row, col)); //归一化到0 ~ 1 
			pic_in[row][col] = 1- (img_in.at<uchar>(row, col)) / 255.0; //归一化到0 ~ 1 
			//cout  << setprecision(2) << (pic_in[row][col])<<' ';
		}
		//cout << endl;
	}

	conv_1_pool2(&pic_in[0][0],&W_CONV1[0][0][0][0],&S2_DRAM[0][0][0], b_conv1 );
	conv_3_pool4(&S2_DRAM[0][0][0], &W_CONV3[0][0][0][0], &S4_DRAM[0][0][0], b_conv3 );
	conv_5(&S4_DRAM[0][0][0], &W_CONV5[0][0][0][0], &C5_DRAM[0], b_conv5);
	fc_6(C5_DRAM, WFC6, F6_DRAM, b_fc6);
	fc_7(F6_DRAM, WFC7, F7_DRAM, b_fc7);

	int max_arg = 0;
	float max = -100000;
	for (int i = 0; i < 10; i++) 
	{
		if (F7_DRAM[i] > max)
		{
			max_arg = i;
			max = F7_DRAM[i];
		}
	}
	imshow("原始图", img_ori);
	imshow("padding图", img_in);
	waitKey(10);
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
void read_parameters()
{
	///////////读取weight
	read_mdl("..\\..\\python code\\parameter\\conv1.0.weight.txt", &W_CONV1[0][0][0][0]);
	read_mdl("..\\..\\python code\\parameter\\conv2.0.weight.txt", &W_CONV3[0][0][0][0]);
	read_mdl("..\\..\\python code\\parameter\\conv3.0.weight.txt", &W_CONV5[0][0][0][0]);

	read_mdl("..\\..\\python code\\parameter\\fc2.0.weight.txt", &WFC6[0]);
	read_mdl("..\\..\\python code\\parameter\\fc3.weight.txt", &WFC7[0]);

	//读取bias
	read_mdl("..\\..\\python code\\parameter\\conv1.0.bias.txt", &b_conv1[0]);
	read_mdl("..\\..\\python code\\parameter\\conv2.0.bias.txt", &b_conv3[0]);
	read_mdl("..\\..\\python code\\parameter\\conv3.0.bias.txt", &b_conv5[0]);

	read_mdl("..\\..\\python code\\parameter\\fc2.0.bias.txt", &b_fc6[0]);
	read_mdl("..\\..\\python code\\parameter\\fc3.bias.txt", &b_fc7[0]);


}

