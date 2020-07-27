#ifndef conv_h
#define conv_h
#include "conv.h"
#include "assert.h"
#include <stdint.h>
#include <string.h> 
void conv_1_pool2(float* In_DRAM, float* W_DRAM, float* Out_DRAM, float* Bias_DRAM, float* W2_P_DRAM, float* Bias2_P_DRAM);
void conv_3_pool4(float* In_DRAM, float* W_DRAM, float* Out_DRAM, float* Bias_DRAM, float* W2_P_DRAM, float* Bias2_P_DRAM);
void conv_5(float* In_DRAM, float* W_DRAM, float* Out_DRAM, float* Bias_DRAM);
void fc_6(float* In_DRAM, float* W_DRAM, float* Out_DRAM, float* Bias_DRAM);
void fc_7(float* In_DRAM, float* W_DRAM, float* Out_DRAM, float* Bias_DRAM);

static const int table[6][16] = {
			1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1,
			1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1,
			1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1,
			0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1, 1,
			0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1,
			0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1
};
#endif
