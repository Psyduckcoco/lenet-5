#include "conv.h"
#include "math.h"



static float IN1[32][32];
static float W1[6][1][5][5];
static float Bias1[6];
static float OUT1[6][28][28]; 

static float W2_P[24];
static float Bias2_P[6];
static float OUT2[6][14][14];

void conv_1(float* In_DRAM, float* W_DRAM, float* Out_DRAM, float* Bias_DRAM,float* W2_P_DRAM, float* Bias2_P_DRAM)
{
	memcpy((void*)Bias1, (const void*)(Bias_DRAM), sizeof(float) * 6);
	memcpy((void*)IN1, (const void*)(In_DRAM), sizeof(float) * 32*32);
	memcpy((void*)W1, (const void*)(W_DRAM ), sizeof(float) * 6*5*5);

	memset(OUT1,0,sizeof(OUT1));

	for (int kr = 0; kr < 5; kr++)
	{
		for (int kc = 0; kc < 5; kc++)
		{
			for (int r = 0; r < 28; r++)
			{
				for (int c = 0; c < 28; c++)
				{
					for (int cho = 0; cho < 6; cho++)
					{
						OUT1[cho][r][c] += IN1[r + kr][c + kc] * W1[cho][0][kr][kc];
					}
				}
			}
		}
	}
	for (int r = 0; r < 28; r++)
	{
		for (int c = 0; c < 28; c++)
		{
			for (int cho = 0; cho < 6; cho++)
			{
				OUT1[cho][r][c] += Bias1[cho];
			}
		}
	}

	memcpy((void*)Bias2_P, (const void*)(Bias2_P_DRAM), sizeof(float) * 6);
	memcpy((void*)W2_P, (const void*)(W2_P_DRAM), sizeof(float) * 24);

	for (int r = 0; r < 14; r++)
	{
		for (int c = 0; c < 14; c++)
		{
			for (int cho = 0; cho < 6; cho++)
			{
				OUT2[cho][r][c] = (IN1[r<<1 + 0][c<<1 + 0]
							+	IN1[r << 1 + 0][c << 1 + 1] 
							+	IN1[r << 1 + 1][c << 1 + 0] 
							+	IN1[r << 1 + 1][c << 1 + 1] )/4;

				OUT2[cho][r][c] *= W2_P[cho] ;
				OUT2[cho][r][c] += Bias2_P[cho];
				OUT2[cho][r][c] = tanf(OUT2[cho][r][c]);
			}
		}
	}
}
