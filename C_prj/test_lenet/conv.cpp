#include "conv.h"
#include "math.h"
//bram 280  dsp 220
static float Bias[120] = { 0 };
static float IN1[32][32] = { 0 };//2 bram
static float IN35[16][14][14] = { 0 };//16 bram
static float POOL_24[16][14][14];//16 bram
static float OUT13[16][28][28] = { 0 };//12 bram
static float OUT567[120] = { 0 };
static float IN67[120] = { 0 };
static float W1[6][5][5] = { 0 };//第一层单独占一个bram
static float W35[8][8][5][5] = { 0 };//3层和5层 8输出并行 8输入并行 占64个bram  128dsp


static float W6[2][8] = { 0 };//第6层全连接 2输出并行 8输入并行 32dsp
static float W7[10][2] = { 0 };//第7层全连接 10输出并行 2输入并行 40dsp

/////////////////////////////////////////////////////////////////////第一层卷积和第二层池化

void conv_1_pool2(float* In_DRAM, float* W_DRAM, float* Out_DRAM, float* Bias_DRAM)
{
	memcpy((void*)Bias, (const void*)(Bias_DRAM), sizeof(float) * 6);
	memcpy((void*)IN1, (const void*)(In_DRAM), sizeof(float) * 32*32);
	memcpy((void*)W1, (const void*)(W_DRAM ), sizeof(float) * 6*5*5);

	memset(OUT13,0,sizeof(float)*6*28*28);

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
						OUT13[cho][r][c] += IN1[r + kr][c + kc] * W1[cho][kr][kc];
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
				OUT13[cho][r][c] = (OUT13[cho][r][c]+ Bias[cho])>0?(OUT13[cho][r][c] + Bias[cho]):0;
			}
		}
	}
	/////////////////////////池化
	for (int r = 0; r < 14; r++)
	{
		for (int c = 0; c < 14; c++)
		{
			for (int cho = 0; cho < 6; cho++)
			{
				POOL_24[cho][r][c] = (OUT13[cho][r*2 + 0][c*2 + 0]
							+ OUT13[cho][r *2 + 0][c *2 + 1]
							+ OUT13[cho][r *2 + 1][c *2+ 0]
							+ OUT13[cho][r *2 + 1][c *2 + 1] )/4;

			}
		}
	}
	memcpy((void*)(Out_DRAM), (const void*)POOL_24, sizeof(float) * 6*14*14);
}


/////////////////////////////////////////////////////////////////////第三层卷积和第四层池化

void conv_3_pool4(float* In_DRAM, float* W_DRAM, float* Out_DRAM, float* Bias_DRAM)
{
	memcpy((void*)Bias, (const void*)(Bias_DRAM), sizeof(float) * 16);
	memcpy((void*)IN35, (const void*)(In_DRAM), sizeof(float) * 6*14 * 14);
	
	memset(OUT13, 0, sizeof(OUT13));

	for (int chl_in = 0; chl_in < 16; chl_in += 8)
	{
		for(int copy_t=0; copy_t<8; copy_t++)
			memcpy((void*)(&W35[0][0][0][0]+ copy_t*8*5*5), (const void*)(W_DRAM+ copy_t*6*5*5 + chl_in*6*5*5), sizeof(float) * 6 * 5 * 5);

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
							for (int cho = 0; cho < 8; cho++)
							{
								OUT13[cho+ chl_in][r][c] += IN35[chi][r + kr][c + kc] * W35[cho][chi][kr][kc];
							}
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
				OUT13[cho][r][c] = (OUT13[cho][r][c] + Bias[cho])>0? (OUT13[cho][r][c] + Bias[cho]):0;
			}
		}
	}
	/////////////////////////池化

	for (int r = 0; r < 5; r++)
	{
		for (int c = 0; c < 5; c++)
		{
			for (int cho = 0; cho < 16; cho++)
			{
				POOL_24[cho][r][c] = (OUT13[cho][r *2 + 0][c *2 + 0]
					+ OUT13[cho][r *2 + 0][c *2 + 1]
					+ OUT13[cho][r *2 + 1][c *2 + 0]
					+ OUT13[cho][r *2 + 1][c *2 + 1]) /4;
			}
		}
	}
	memcpy((void*)(Out_DRAM), (const void*)POOL_24, sizeof(float) * 16 * 14 * 14);
}
/////////////////////////////////////////////////////////////////////第五层

void conv_5(float* In_DRAM, float* W_DRAM, float* Out_DRAM, float* Bias_DRAM )
{
	memcpy((void*)Bias, (const void*)(Bias_DRAM), sizeof(float) * 120);
	memcpy((void*)IN35, (const void*)(In_DRAM), sizeof(float) * 16 * 14 * 14);
	

	memset(OUT567, 0, sizeof(OUT567));
	
	for (int chl_o = 0; chl_o < 120; chl_o += 8)
	{
		for (int chl_in = 0; chl_in < 16; chl_in += 8)
		{
			for (int copy_t = 0; copy_t < 8; copy_t++)
				memcpy((void*)(&W35[0][0][0][0] + copy_t * 8 * 5 * 5), (const void*)(W_DRAM + copy_t * 16 * 5 * 5 + chl_in * 5 * 5 + chl_o*16*5*5), sizeof(float) * 8 * 5 * 5);

			for (int kr = 0; kr < 5; kr++)
			{
				for (int kc = 0; kc < 5; kc++)
				{
					for (int chi = 0; chi < 8; chi++)
					{
						for (int cho = 0; cho < 8; cho++)
						{
							OUT567[cho+ chl_o] += IN35[chi+ chl_in][kr][kc] * W35[cho][chi][kr][kc];
						}
					}
				}
			}
		}
	}
	
	for (int cho = 0; cho < 120; cho++)
	{
		OUT567[cho]  = (OUT567[cho] + Bias[cho])>0? (OUT567[cho] + Bias[cho]):0;
	}
	memcpy((void*)(Out_DRAM), (const void*)OUT567, sizeof(float) * 120);
}

/////////////////////////////////////////////////////////////////////FC6层

void fc_6(float* In_DRAM, float* W_DRAM, float* Out_DRAM, float* Bias_DRAM)
{
	memcpy((void*)Bias, (const void*)(Bias_DRAM), sizeof(float) * 84);
	memcpy((void*)IN67, (const void*)(In_DRAM), sizeof(float) * 120); 

	memset(OUT567, 0, sizeof(OUT567));

	for (int chl_o = 0; chl_o < 84; chl_o += 2)
	{
		for (int chl_in = 0; chl_in < 120; chl_in += 8)
		{
			for (int copy_t = 0; copy_t < 8; copy_t++)
				memcpy((void*)(&W6[0][0] + copy_t*8), (const void*)(W_DRAM + copy_t * 120  + chl_in  + chl_o * 120 ), sizeof(float) * 8 );
		
			for (int cho = 0; cho < 2; cho++)
			{
				for (int chi = 0; chi < 8; chi++)
				{
					OUT567[cho+ chl_o] += IN67[chi+ chl_in] * W6[cho][chi];
				}
			}
		}
	}

	for (int cho = 0; cho < 84; cho++)
	{
		OUT567[cho] = (OUT567[cho] + Bias[cho])>0? (OUT567[cho] + Bias[cho]):0;
	}

	memcpy((void*)(Out_DRAM), (const void*)OUT567, sizeof(float) * 84);
}
/////////////////////////////////////////////////////////////////////FC7层

void fc_7(float* In_DRAM, float* W_DRAM, float* Out_DRAM, float* Bias_DRAM)
{
	memcpy((void*)Bias, (const void*)(Bias_DRAM), sizeof(float) * 10);
	memcpy((void*)IN67, (const void*)(In_DRAM), sizeof(float) * 84);

	memset(OUT567, 0, sizeof(OUT567));

	//for (int chl_o = 0; chl_o < 10; chl_o += 2)
	{
		for (int chl_in = 0; chl_in < 84; chl_in += 2)
		{
			for (int copy_t = 0; copy_t < 10; copy_t++)
				memcpy((void*)(&W7[0][0] + copy_t * 2), (const void*)(W_DRAM + copy_t * 84 + chl_in ), sizeof(float) * 2);

			for (int cho = 0; cho < 10; cho++)
			{
				for (int chi = 0; chi < 2; chi++)
				{
					OUT567[cho] += IN67[chi + chl_in] * W7[cho][chi];
				}
			}
		}
	}

	memcpy((void*)(Out_DRAM), (const void*)OUT567, sizeof(float) * 10);
}


void top_fun(float* In_DRAM, float* W_DRAM, float* Out_DRAM, float* Bias_DRAM,int8_t layer)
{
	if (layer == 1)
	{
		memcpy((void*)Bias, (const void*)(Bias_DRAM), sizeof(float) * 6);
		memcpy((void*)IN1, (const void*)(In_DRAM), sizeof(float) * 32 * 32);
		memcpy((void*)W1, (const void*)(W_DRAM), sizeof(float) * 6 * 5 * 5);

		memset(OUT13, 0, sizeof(float) * 6 * 28 * 28);

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
							OUT13[cho][r][c] += IN1[r + kr][c + kc] * W1[cho][kr][kc];
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
					OUT13[cho][r][c] = (OUT13[cho][r][c] + Bias[cho]) > 0 ? (OUT13[cho][r][c] + Bias[cho]) : 0;
				}
			}
		}
		/////////////////////////池化
		for (int r = 0; r < 14; r++)
		{
			for (int c = 0; c < 14; c++)
			{
				for (int cho = 0; cho < 6; cho++)
				{
					POOL_24[cho][r][c] = (OUT13[cho][r * 2 + 0][c * 2 + 0]
						+ OUT13[cho][r * 2 + 0][c * 2 + 1]
						+ OUT13[cho][r * 2 + 1][c * 2 + 0]
						+ OUT13[cho][r * 2 + 1][c * 2 + 1]) / 4;

				}
			}
		}
		memcpy((void*)(Out_DRAM), (const void*)POOL_24, sizeof(float) * 6 * 14 * 14);
	}
	else if (layer == 2)
	{
		memcpy((void*)Bias, (const void*)(Bias_DRAM), sizeof(float) * 16);
		memcpy((void*)IN35, (const void*)(In_DRAM), sizeof(float) * 6 * 14 * 14);

		memset(OUT13, 0, sizeof(OUT13));

		for (int chl_in = 0; chl_in < 16; chl_in += 8)
		{
			for (int copy_t = 0; copy_t < 8; copy_t++)
				memcpy((void*)(&W35[0][0][0][0] + copy_t * 8 * 5 * 5), (const void*)(W_DRAM + copy_t * 6 * 5 * 5 + chl_in * 6 * 5 * 5), sizeof(float) * 6 * 5 * 5);

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
								for (int cho = 0; cho < 8; cho++)
								{
									OUT13[cho + chl_in][r][c] += IN35[chi][r + kr][c + kc] * W35[cho][chi][kr][kc];
								}
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
					OUT13[cho][r][c] = (OUT13[cho][r][c] + Bias[cho]) > 0 ? (OUT13[cho][r][c] + Bias[cho]) : 0;
				}
			}
		}
		/////////////////////////池化

		for (int r = 0; r < 5; r++)
		{
			for (int c = 0; c < 5; c++)
			{
				for (int cho = 0; cho < 16; cho++)
				{
					POOL_24[cho][r][c] = (OUT13[cho][r * 2 + 0][c * 2 + 0]
						+ OUT13[cho][r * 2 + 0][c * 2 + 1]
						+ OUT13[cho][r * 2 + 1][c * 2 + 0]
						+ OUT13[cho][r * 2 + 1][c * 2 + 1]) / 4;
				}
			}
		}
		memcpy((void*)(Out_DRAM), (const void*)POOL_24, sizeof(float) * 16 * 14 * 14);
	}
		
	else if (layer == 3)
	{
	memcpy((void*)Bias, (const void*)(Bias_DRAM), sizeof(float) * 120);
	memcpy((void*)IN35, (const void*)(In_DRAM), sizeof(float) * 16 * 14 * 14);


	memset(OUT567, 0, sizeof(OUT567));

	for (int chl_o = 0; chl_o < 120; chl_o += 8)
	{
		for (int chl_in = 0; chl_in < 16; chl_in += 8)
		{
			for (int copy_t = 0; copy_t < 8; copy_t++)
				memcpy((void*)(&W35[0][0][0][0] + copy_t * 8 * 5 * 5), (const void*)(W_DRAM + copy_t * 16 * 5 * 5 + chl_in * 5 * 5 + chl_o * 16 * 5 * 5), sizeof(float) * 8 * 5 * 5);

			for (int kr = 0; kr < 5; kr++)
			{
				for (int kc = 0; kc < 5; kc++)
				{
					for (int chi = 0; chi < 8; chi++)
					{
						for (int cho = 0; cho < 8; cho++)
						{
							OUT567[cho + chl_o] += IN35[chi + chl_in][kr][kc] * W35[cho][chi][kr][kc];
						}
					}
				}
			}
		}
	}

	for (int cho = 0; cho < 120; cho++)
	{
		OUT567[cho] = (OUT567[cho] + Bias[cho]) > 0 ? (OUT567[cho] + Bias[cho]) : 0;
	}
	memcpy((void*)(Out_DRAM), (const void*)OUT567, sizeof(float) * 120);
	}
	else if (layer == 4)
	{
	memcpy((void*)Bias, (const void*)(Bias_DRAM), sizeof(float) * 84);
	memcpy((void*)IN67, (const void*)(In_DRAM), sizeof(float) * 120);

	memset(OUT567, 0, sizeof(OUT567));

	for (int chl_o = 0; chl_o < 84; chl_o += 2)
	{
		for (int chl_in = 0; chl_in < 120; chl_in += 8)
		{
			for (int copy_t = 0; copy_t < 8; copy_t++)
				memcpy((void*)(&W6[0][0] + copy_t * 8), (const void*)(W_DRAM + copy_t * 120 + chl_in + chl_o * 120), sizeof(float) * 8);

			for (int cho = 0; cho < 2; cho++)
			{
				for (int chi = 0; chi < 8; chi++)
				{
					OUT567[cho + chl_o] += IN67[chi + chl_in] * W6[cho][chi];
				}
			}
		}
	}

	for (int cho = 0; cho < 84; cho++)
	{
		OUT567[cho] = (OUT567[cho] + Bias[cho]) > 0 ? (OUT567[cho] + Bias[cho]) : 0;
	}

	memcpy((void*)(Out_DRAM), (const void*)OUT567, sizeof(float) * 84);
	}
	else if (layer == 5)
	{
	memcpy((void*)Bias, (const void*)(Bias_DRAM), sizeof(float) * 10);
	memcpy((void*)IN67, (const void*)(In_DRAM), sizeof(float) * 84);

	memset(OUT567, 0, sizeof(OUT567));

	//for (int chl_o = 0; chl_o < 10; chl_o += 2)
	{
		for (int chl_in = 0; chl_in < 84; chl_in += 2)
		{
			for (int copy_t = 0; copy_t < 10; copy_t++)
				memcpy((void*)(&W7[0][0] + copy_t * 2), (const void*)(W_DRAM + copy_t * 84 + chl_in), sizeof(float) * 2);

			for (int cho = 0; cho < 10; cho++)
			{
				for (int chi = 0; chi < 2; chi++)
				{
					OUT567[cho] += IN67[chi + chl_in] * W7[cho][chi];
				}
			}
		}
	}

	memcpy((void*)(Out_DRAM), (const void*)OUT567, sizeof(float) * 10);
	}



}