#include "SD.h"
static FATFS fatfs;

int SD_Initt()
{
    FRESULT rc;
    rc = f_mount(&fatfs,"",0);
    if(rc)
    {
        xil_printf("ERROR: f_mount returned %d\r\n",rc);
        return XST_FAILURE;
    }
    return XST_SUCCESS;
}

void SD_Init_m()
{
	int rc;

	rc = SD_Initt();
	if(XST_SUCCESS != rc)
	{
		xil_printf("fail to open SD Card\n\r");
	}
	else
	{
		xil_printf("success to open SD Card\n\r");
	}

}

int SD_Transfer_read(char *FileName,u32 DestinationAddress,u32 ByteLength)
{
    FIL fil;
    FRESULT rc;
    UINT br;

    rc=f_open(&fil,FileName,FA_READ);
    if(rc)
    {
        xil_printf("ERROR1:f_open returned %d\r\n",rc);
        return XST_FAILURE;
    }
    rc = f_lseek(&fil,0);
    if(rc)
    {
        xil_printf("ERROR2:f_open returned %d\r\n",rc);
        return XST_FAILURE;
    }
    rc = f_read(&fil,(void*)DestinationAddress,ByteLength,&br);
    if(rc)
    {
        xil_printf("ERROR3:f_open returned %d\r\n",rc);
        return XST_FAILURE;
    }
    rc = f_close(&fil);
    if(rc)
    {
        xil_printf("ERROR4:f_open returned %d\r\n",rc);
        return XST_FAILURE;
    }
    return XST_SUCCESS;
}

int SD_Transfer_write(char *FileName,u32 SourceAddress,u32 ByteLength)
{
    FIL fil;
    FRESULT rc;
    UINT bw;

    rc = f_open(&fil,FileName,FA_CREATE_ALWAYS | FA_WRITE);
    if(rc)
    {
        xil_printf("ERROR : f_open returned %d\r\n",rc);
        return XST_FAILURE;
    }
    rc = f_lseek(&fil, 0);
    if(rc)
    {
        xil_printf("ERROR : f_lseek returned %d\r\n",rc);
        return XST_FAILURE;
    }
    rc = f_write(&fil,(void*) SourceAddress,ByteLength,&bw);
    if(rc)
    {
        xil_printf("ERROR : f_write returned %d\r\n", rc);
        return XST_FAILURE;
    }
    rc = f_close(&fil);
    if(rc){
        xil_printf("ERROR : f_close returned %d\r\n",rc);
        return XST_FAILURE;
    }
    return XST_SUCCESS;
}
