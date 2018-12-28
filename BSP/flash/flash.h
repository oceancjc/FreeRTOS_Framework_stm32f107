#ifndef __FLASH_H
#define __FLASH_H

#include "stm32f10x.h"
#include "usart.h"
//#include "spi.h"

 



unsigned short* Flash_ReadHalfWord(u32 addr);
void Flash_WriteHalfWord(u32 addr, unsigned short *data, unsigned short len);
void Flash_ErasePage(u32 addr);



#endif /* __FLASH_H */

