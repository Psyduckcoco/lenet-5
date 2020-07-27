#include "conv.h"
#include "math.h"
//float _tanh(float x) {
//	//1/(1+e^-x)
//
//	float exp_x = expf(-x);
//	float res = 1 / (1 + exp_x);
//	return res;
//
////	float exp2x = expf(2 * x) + 1;
////	return (exp2x - 2) / (exp2x);
//}

/////////////////////////////////////////////////////////////////////第一层卷积和第二层池化
static float IN1[32][32] = { 0 };
static float W1[6][1][5][5] = { 0 };
static float Bias1[6] = { 0 };
static float OUT1[6][28][28] = { 0 };

static float W2_P[24] = { 0 };
static float Bias2_P[6] = { 0 };
static float OUT2[6][14][14] = { 0 };

void conv_1_pool2(float* In_DRAM, float* W_DRAM, float* Out_DRAM, float* Bias_DRAM,float* W2_P_DRAM, float* Bias2_P_DRAM)
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
				OUT1[cho][r][c] = (OUT1[cho][r][c]+ Bias1[cho])>0?(OUT1[cho][r][c] + Bias1[cho]):0;
			}
		}
	}
	/////////////////////////池化
	memcpy((void*)Bias2_P, (const void*)(Bias2_P_DRAM), sizeof(float) * 6);
	memcpy((void*)W2_P, (const void*)(W2_P_DRAM), sizeof(float) * 24);

	for (int r = 0; r < 14; r++)
	{
		for (int c = 0; c < 14; c++)
		{
			for (int cho = 0; cho < 6; cho++)
			{
				OUT2[cho][r][c] = (OUT1[cho][r*2 + 0][c*2 + 0]
							+ OUT1[cho][r *2 + 0][c *2 + 1]
							+ OUT1[cho][r *2 + 1][c *2+ 0]
							+ OUT1[cho][r *2 + 1][c *2 + 1] )/4;

				/*float w = W2_P[cho] *0.25;
				OUT2[cho][r][c] *= w; 
				OUT2[cho][r][c] = _tanh(OUT2[cho][r][c]+ Bias2_P[cho]);*/
			}
		}
	}
	memcpy((void*)(Out_DRAM), (const void*)OUT2, sizeof(float) * 6*14*14);
}


/////////////////////////////////////////////////////////////////////第三层卷积和第四层池化
static float IN3[6][14][14] = { 0 };
static float W3[16][6][5][5] = { 0 };
static float Bias3[16] = { 0 };
static float OUT3[16][10][10] = { 0 };

static float W4_P[16*4] = { 0 };
static float Bias4_P[16] = { 0 };
static float OUT4[16][5][5] = { 0 };

void conv_3_pool4(float* In_DRAM, float* W_DRAM, float* Out_DRAM, float* Bias_DRAM, float* W2_P_DRAM, float* Bias2_P_DRAM)
{
	memcpy((void*)Bias3, (const void*)(Bias_DRAM), sizeof(float) * 16);
	memcpy((void*)IN3, (const void*)(In_DRAM), sizeof(float) * 6*14 * 14);
	memcpy((void*)W3, (const void*)(W_DRAM), sizeof(float)*16 * 6 * 5 * 5);

	memset(OUT3, 0, sizeof(OUT3));

	for (int kr = 0; kr < 5; kr++)
	{
		for (int kc = 0; kc < 5; kc++)
		{
			for (int r = 0; r < 10; r++)
			{
				for (int c = 0; c < 10; c++)
				{
					for (int chi = 0; chi < 6; chi++)
					{
						for (int cho = 0; cho < 16; cho++)
						{
							if (table[chi][cho]==1)
								OUT3[cho][r][c] += IN3[chi][r + kr][c + kc] * W3[cho][chi][kr][kc];
							else OUT3[cho][r][c] = 0;
						}
					}

				}
			}
		}
	}
	for (int r = 0; r < 10; r++)
	{
		for (int c = 0; c < 10; c++)
		{
			for (int cho = 0; cho < 16; cho++)
			{
				OUT3[cho][r][c] = (OUT3[cho][r][c] + Bias3[cho])>0? (OUT3[cho][r][c] + Bias3[cho]):0;
			}
		}
	}
	/////////////////////////池化
	memcpy((void*)Bias4_P, (const void*)(Bias2_P_DRAM), sizeof(float) * 16);
	memcpy((void*)W4_P, (const void*)(W2_P_DRAM), sizeof(float) * 16*4);

	for (int r = 0; r < 5; r++)
	{
		for (int c = 0; c < 5; c++)
		{
			for (int cho = 0; cho < 16; cho++)
			{
				OUT4[cho][r][c] = (OUT3[cho][r *2 + 0][c *2 + 0]
					+ OUT3[cho][r *2 + 0][c *2 + 1]
					+ OUT3[cho][r *2 + 1][c *2 + 0]
					+ OUT3[cho][r *2 + 1][c *2 + 1]) /4;

				/*float w = W4_P[cho] * 0.25;
				OUT4[cho][r][c] *= w; 
				OUT4[cho][r][c] = _tanh(OUT4[cho][r][c] + Bias4_P[cho]);*/
			}
		}
	}
	memcpy((void*)(Out_DRAM), (const void*)OUT4, sizeof(float) * 16 * 5 * 5);
}
/////////////////////////////////////////////////////////////////////第五层
static float IN5[16][5][5] = { 0 };
static float W5[120][16][5][5] = { 0 };
static float Bias5[120] = { 0 };
static float OUT5[120] = { 0 };

void conv_5(float* In_DRAM, float* W_DRAM, float* Out_DRAM, float* Bias_DRAM )
{
	memcpy((void*)Bias5, (const void*)(Bias_DRAM), sizeof(float) * 120);
	memcpy((void*)IN5, (const void*)(In_DRAM), sizeof(float) * 16 * 5 * 5);
	memcpy((void*)W5, (const void*)(W_DRAM), sizeof(float) * 120 * 16 * 5 * 5);

	memset(OUT5, 0, sizeof(OUT5));

	for (int kr = 0; kr < 5; kr++)
	{
		for (int kc = 0; kc < 5; kc++)
		{
			for (int chi = 0; chi < 16; chi++)
			{
				for (int cho = 0; cho < 120; cho++)
				{
					OUT5[cho]  += IN5[chi][kr][kc] * W5[cho][chi][kr][kc];
				}
			}
		}
	}

	for (int cho = 0; cho < 120; cho++)
	{
		OUT5[cho]  = (OUT5[cho] +Bias5[cho])>0? (OUT5[cho] + Bias5[cho]):0;
	}
	memcpy((void*)(Out_DRAM), (const void*)OUT5, sizeof(float) * 120);
}

/////////////////////////////////////////////////////////////////////FC6层
static float IN6[120] = { 0 };
static float W6[120][84] = { 0 };
static float Bias6[84] = { 0 };
static float OUT6[84] = { 0 };

void fc_6(float* In_DRAM, float* W_DRAM, float* Out_DRAM, float* Bias_DRAM)
{
	memcpy((void*)Bias6, (const void*)(Bias_DRAM), sizeof(float) * 84);
	memcpy((void*)IN6, (const void*)(In_DRAM), sizeof(float) * 120);
	memcpy((void*)W6, (const void*)(W_DRAM), sizeof(float) * 120 *84);

	memset(OUT6, 0, sizeof(OUT6));


	for (int chi = 0; chi < 120; chi++)
	{
		for (int cho = 0; cho < 84; cho++)
		{
			OUT6[cho] += IN6[chi]  * W6[chi][cho] ;
		}
	}

	for (int cho = 0; cho < 84; cho++)
	{
		OUT6[cho] = (OUT6[cho] + Bias6[cho])>0? (OUT6[cho] + Bias6[cho]):0;
	}

	memcpy((void*)(Out_DRAM), (const void*)OUT6, sizeof(float) * 84);
}
/////////////////////////////////////////////////////////////////////FC7层
static float IN7[84] = { 0 };
static float W7[84][10] = { 0 };
static float Bias7[10] = { 0 };
static float OUT7[10] = { 0 };

void fc_7(float* In_DRAM, float* W_DRAM, float* Out_DRAM, float* Bias_DRAM)
{
	memcpy((void*)Bias7, (const void*)(Bias_DRAM), sizeof(float) * 10);
	memcpy((void*)IN7, (const void*)(In_DRAM), sizeof(float) * 84);
	memcpy((void*)W7, (const void*)(W_DRAM), sizeof(float) * 10 * 84);

	memset(OUT7, 0, sizeof(OUT7));


	for (int chi = 0; chi < 84; chi++)
	{
		for (int cho = 0; cho < 10; cho++)
		{
			OUT7[cho] += IN7[chi] * W7[chi][cho];
		}
	}

	memcpy((void*)(Out_DRAM), (const void*)OUT7, sizeof(float) * 10);
}