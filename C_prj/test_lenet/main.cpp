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


float W_CONV1[6][1][5][5];
float W_POOL2[6 * 4];
float W_CONV3[16][6][5][5];
float W_POOL4[16 * 4];
float W_CONV5[120][16][5][5];
float WFC6[120 * 84];
float WFC7[84 * 10];

float b_conv1[6];
float b_conv3[16];
float b_conv5[120];

float b_pool2[6];
float b_pool4[16];

float b_fc6[84];
float b_fc7[10];


int main()
{


	return 0;

}