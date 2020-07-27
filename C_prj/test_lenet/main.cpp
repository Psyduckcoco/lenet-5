#include <iostream>  
#include <fstream>  
#include <sstream>  
#include "stdlib.h"
#include <cstdlib>
#include<ctime>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
using namespace std;
using namespace cv;
float input[32][32] = {0};
float C1_DRAM[6][28][28] = { 0 };
float S2_DRAM[6][14][14] = { 0 };
float C3_DRAM[16][10][10] = { 0 };
float S4_DRAM[16][5][5] = { 0 };
float C5_DRAM[120] = { 0 };
float F6_DRAM[84] = { 0 };
float F7_DRAM[10] = { 0 };

float W_CONV1[6][1][5][5] = { 0 };
float W_POOL2[6 * 4] = { 0 };
float W_CONV3[16][6][5][5] = { 0 };
float W_POOL4[16 * 4] = { 0 };
float W_CONV5[120][16][5][5] = { 0 };

float WFC6[120 * 84] = { 0 };
float WFC7[84 * 10] = { 0 };

float b_conv1[6] = { 0 };
float b_conv3[16] = { 0 };
float b_conv5[120] = { 0 };

float b_pool2[6] = { 0 };
float b_pool4[16] = { 0 };

float b_fc6[84] = { 0 };
float b_fc7[10] = { 0 };

void read_parameters();

int main()
{
	read_parameters();


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
	///////////∂¡»°weight
	read_mdl("E:\\lenet\\filter\\Wconv1.mdl", &W_CONV1[0][0][0][0]);
	read_mdl("E:\\lenet\\filter\\Wconv3_modify.mdl", &W_CONV3[0][0][0][0]);
	read_mdl("E:\\lenet\\filter\\Wconv5.mdl", &W_CONV5[0][0][0][0]);

	read_mdl("E:\\lenet\\filter\\Wfc1.mdl", &WFC6[0]);
	read_mdl("E:\\lenet\\filter\\Wfc2.mdl", &WFC7[0]);

	read_mdl("E:\\lenet\\filter\\Wpool1.mdl", &W_POOL2[0]);
	read_mdl("E:\\lenet\\filter\\Wpool2.mdl", &W_POOL4[0]);

	//∂¡»°bias
	read_mdl("E:\\lenet\\filter\\bconv1.mdl", &b_conv1[0]);
	read_mdl("E:\\lenet\\filter\\bconv3.mdl", &b_conv3[0]);
	read_mdl("E:\\lenet\\filter\\bconv5.mdl", &b_conv5[0]);

	read_mdl("E:\\lenet\\filter\\bfc1.mdl", &b_fc6[0]);
	read_mdl("E:\\lenet\\filter\\bfc2.mdl", &b_fc7[0]);

	read_mdl("E:\\lenet\\filter\\bpool1.mdl", &b_pool2[0]);
	read_mdl("E:\\lenet\\filter\\bpool2.mdl", &b_pool4[0]);
}

