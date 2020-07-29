/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "SD.h"
#include "xparameters.h"
#include "xtop_fun.h"

#include "xil_cache.h"
#include "ff.h"
#include "xtime_l.h"
#include "sleep.h"
float input[32][32] = {0};

float POOL24_DRAM[16][14][14] = { 0 };

float C5_DRAM[120] = { 0 };
float C6_DRAM[84] = { 0 };
float C7_DRAM[10] = { 0 };

float W_CONV1[6][1][5][5] = { 0 };
float W_CONV3[16][6][5][5] = { 0 };
float W_CONV5[120][16][5][5] = { 0 };

float b_conv1[6] = { 0 };
float b_conv3[16] = { 0 };
float b_conv5[120] = { 0 };

float WFC6[10080+2000] = { 0 };
float WFC7[840+2000] = { 0 };

float b_fc6[84] = { 0 };
float b_fc7[10] = { 0 };

float pic_in[8952+1000][32*32] = {0};//输入
int8_t label[8952]= {0};//标签，用于测试模型参数准确度

XTop_fun XTop_fun_inst;
int Init_Xtop_fun()
{
	// Initialize the Device
	int status=0;
	status = XTop_fun_Initialize(&XTop_fun_inst, XPAR_TOP_FUN_0_DEVICE_ID);
	if (status != XST_SUCCESS)
	{
		print("ERROR: Initialize could not initialize.\n\r");
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}
void top_fun(XTop_fun *InstancePtr,u32 pic_in,u32 w_in,u32 out,u32 bias,u32 layer)
{
	XTop_fun_Set_In_DRAM(InstancePtr,pic_in);//设置输入偏移
	XTop_fun_Set_W_DRAM(InstancePtr, w_in);//设置权重偏移
	XTop_fun_Set_Out_DRAM(InstancePtr, out);//设置输出偏移
	XTop_fun_Set_Bias_DRAM(InstancePtr,bias);//设置偏差偏移
	XTop_fun_Set_layer(InstancePtr,layer);//设置当前层数

	XTop_fun_Start(InstancePtr); //启动moudle
	 while( !XTop_fun_IsIdle(InstancePtr) );//等待 Port ap_idle is asserted High to indicate that the design is waiting start again

}
void read_parameters()
{
	///////////读取weight

	SD_Transfer_read("param//conv1_w", (u32)(&W_CONV1[0][0][0][0]),4*6*5*5);
	SD_Transfer_read("param//conv3_w", (u32)(&W_CONV3[0][0][0][0]), 4*16*6*5*5);
	SD_Transfer_read("param//conv5_w", (u32)(&W_CONV5[0][0][0][0]), 4*120*16*5*5);



	SD_Transfer_read("param//b_conv1", (u32)(&b_conv1[0]), sizeof(b_conv1));
	SD_Transfer_read("param//b_conv3", (u32)(&b_conv3[0]), sizeof(b_conv3));
	SD_Transfer_read("param//b_conv5", (u32)(&b_conv5[0]), sizeof(b_conv5));
	SD_Transfer_read("param//b_fc6", (u32)(&b_fc6[0]), sizeof(b_fc6));
	SD_Transfer_read("param//b_fc7", (u32)(&b_fc7[0]), sizeof(b_fc7));

	SD_Transfer_read("param//test_label", (u32)label, sizeof(label));
	SD_Transfer_read("param//weight6", (u32)(&WFC6[0]), 10080*4);
	SD_Transfer_read("param//weight7", (u32)(&WFC7[0]),840*4);
	print("weight bias load ok\n\r");


	print("load Validation dataset..\n\r");
	SD_Transfer_read("param//vali_data", (u32)(&pic_in[0][0]),8952*32*32*4);
	Xil_DCacheFlushRange((u32)(&pic_in[0]), sizeof(pic_in));//刷新读取的数据到DDR
	print("load Validation dataset ok\n\r");




	Xil_DCacheFlushRange((u32)W_CONV1, sizeof(W_CONV1));//刷新读取的数据到DDR
	Xil_DCacheFlushRange((u32)W_CONV3, sizeof(W_CONV3));//刷新读取的数据到DDR
	Xil_DCacheFlushRange((u32)W_CONV5, sizeof(W_CONV5));//刷新读取的数据到DDR

	Xil_DCacheFlushRange((u32)WFC6, sizeof(WFC6));//刷新读取的数据到DDR
	Xil_DCacheFlushRange((u32)WFC7, sizeof(WFC7));//刷新读取的数据到DDR

	Xil_DCacheFlushRange((u32)b_conv1, sizeof(b_conv1));//刷新读取的数据到DDR
	Xil_DCacheFlushRange((u32)b_conv3, sizeof(b_conv3));//刷新读取的数据到DDR
	Xil_DCacheFlushRange((u32)b_conv5, sizeof(b_conv5));//刷新读取的数据到DDR
	Xil_DCacheFlushRange((u32)b_fc6, sizeof(b_fc6));//刷新读取的数据到DDR
	Xil_DCacheFlushRange((u32)b_fc7, sizeof(b_fc7));//刷新读取的数据到DDR


	print("DCacheFlush data ok\n\r");
}


int main()
{
	int status=0;
	XTime tEnd, tCur;
	float uesd_time;
    init_platform();
    SD_Init_m();//初始化SD
    print("-------------lenet-------------\n\r");
    read_parameters();
    print("initial top fun module\n\r");
    status = Init_Xtop_fun();//查找设备以及初始化CONV_MOUDLE
    if (status != XST_SUCCESS)
        {
        	print("ERROR: top fun module could not initialize.\n\r");
        	return XST_FAILURE;
        }


    int err_cnt = 0;
    XTime_GetTime(&tCur);
    for (int i = 0; i < 8952; i++)
    	{

    		top_fun(&XTop_fun_inst,(u32)&pic_in[i][0],(u32) &W_CONV1[0][0][0][0],(u32) &POOL24_DRAM[0][0][0], (u32)b_conv1,1);
    		Xil_DCacheInvalidateRange((u32)POOL24_DRAM, sizeof(POOL24_DRAM));

    		top_fun(&XTop_fun_inst,(u32)&POOL24_DRAM[0][0][0],(u32) &W_CONV3[0][0][0][0],(u32) &POOL24_DRAM[0][0][0], (u32)b_conv3,2);
    		Xil_DCacheInvalidateRange((u32)POOL24_DRAM, sizeof(POOL24_DRAM));

    		top_fun(&XTop_fun_inst,(u32)&POOL24_DRAM[0][0][0], (u32)&W_CONV5[0][0][0][0], (u32)&C5_DRAM[0],(u32) b_conv5,3);
    		Xil_DCacheInvalidateRange((u32)C5_DRAM, sizeof(C5_DRAM));

    		top_fun(&XTop_fun_inst,(u32)C5_DRAM, (u32)WFC6, (u32)C6_DRAM, (u32)b_fc6,4);
    		Xil_DCacheInvalidateRange((u32)C6_DRAM, sizeof(C6_DRAM));

    		top_fun(&XTop_fun_inst,(u32)C6_DRAM,(u32) WFC7, (u32)C7_DRAM, (u32)b_fc7,5);
    		Xil_DCacheInvalidateRange((u32)C7_DRAM, sizeof(C7_DRAM));

    		int max_arg = 0;
    		float max = -100000;
    		for (int i = 0; i < 10; i++)
    		{
    			if (C7_DRAM[i] > max)
    			{
    				max_arg = i;
    				max = C7_DRAM[i];
    			}
    		}
    		if (max_arg != label[i])
    			err_cnt++;
    		if(i%1000==0)
    			printf("%d /8952 \r\n",i);
    	}
    XTime_GetTime(&tEnd);
    uesd_time = ((float)(tEnd-tCur)*1000)/((float)COUNTS_PER_SECOND);//ms

    printf("err  =  %d  \r\n",err_cnt);
    float rate =  (1-  err_cnt/ 8952.0) * 100;
    printf("Recognition rate is %f %% \r\n",rate);
    printf("FPS is  %f fps/s \r\n",8952/(uesd_time/1000));

    cleanup_platform();
    return 0;
}








