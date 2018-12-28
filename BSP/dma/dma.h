#ifndef __DMA_H
#define __DMA_H

#include "stm32f10x.h"
#include "usart.h"
//#include "spi.h"

 



void USART1_DMA_Config(void);
void USART2_DMA_Config(void);
void SPI_DMA_Config(void);
void DMA1_Channel4_IRQHandler(void);
void DMA1_Channel5_IRQHandler(void);



#endif /* __DMA_H */

