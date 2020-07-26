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
float Layer3_DRAM[384][61][61] = { 0 };
float Layer4_DRAM[384][61][61] = { 0 };
float Layer5_DRAM[256][fifth_R][fifth_R] = { 0 };