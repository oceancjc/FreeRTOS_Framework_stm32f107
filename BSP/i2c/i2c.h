#ifndef __I2C_H__
#define __I2C_H__


#include "stm32f10x.h"
#include "globals.h"


extern void I2C_Configuration(void);
extern void I2C_RegisterByteWrite(uint8_t pBuffer, uint8_t devAddr, uint8_t regAddr);
extern int I2C_RegisterByteRead(uint8_t devAddr, uint8_t ReadAddr, uint8_t *pBuffer);
extern void I2C_WaitResponse(uint8_t devAddr);

#endif

