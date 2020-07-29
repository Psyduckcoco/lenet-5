#ifndef SD_h
#define SD_h

#include "platform.h"
#include "xparameters.h"
#include "xil_printf.h"
#include "ff.h"
#include "xdevcfg.h"



int SD_Init();
void SD_Init_m();
int SD_Transfer_read(char *FileName,u32 DestinationAddress,u32 ByteLength);
int SD_Transfer_write(char *FileName,u32 SourceAddress,u32 ByteLength);

#endif
