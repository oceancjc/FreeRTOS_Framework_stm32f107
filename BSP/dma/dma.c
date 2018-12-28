#include "dma.h"


#ifdef FreeRTOS
    #include "FreeRTOS.h"
    #include "task.h"
    #include "queue.h"
    #include "timers.h"
    #include "semphr.h"
#endif


void USART1_DMA_Config(void){
    DMA_InitTypeDef DMA_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;  //DMA for UART1发送
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 13;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
    NVIC_Init(&NVIC_InitStructure);
    

//     NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;  //DMA for UART1接收
//     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
//     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
//     NVIC_Init(&NVIC_InitStructure);

    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    DMA_DeInit(DMA1_Channel4); 
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);       
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)UART1_TxBuffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = MAXBUF;    
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;    
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;     
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;     
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;  
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;   
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);        
    DMA_Cmd (DMA1_Channel4,ENABLE);
    DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);

    
    

    
    DMA_DeInit(DMA1_Channel5);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);       
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)UART1_RxBuffer;    
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;    
    DMA_InitStructure.DMA_BufferSize = MAXBUF;   
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;    
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;     
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;     
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;  
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; 
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);        
    DMA_Cmd (DMA1_Channel5,ENABLE);
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE); //开启UART_DMA传输功能
}

void USART2_DMA_Config(void){
    DMA_InitTypeDef DMA_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;  //DMA for UART1发送
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
    NVIC_Init(&NVIC_InitStructure);
    
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    DMA_DeInit(DMA1_Channel7); 
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DR);       
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)UART1_TxBuffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = MAXBUF;    
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;    
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;     
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;     
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;  
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;   
    DMA_Init(DMA1_Channel7, &DMA_InitStructure);        
    DMA_Cmd (DMA1_Channel7,ENABLE);
    DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);
              
}


void DMA1_Channel4_IRQHandler(void){  //UART1发送完成DMA中断    
    if(DMA_GetFlagStatus(DMA1_FLAG_TC4)==SET){   
        //    DMA_Cmd (DMA1_Channel4,DISABLE);          
        DMA_ClearFlag(DMA1_FLAG_TC4); 
        #ifdef FreeRTOS
            xSemaphoreGiveFromISR( semb_dma_uart1, NULL );
        #endif
     }    
}


void DMA1_Channel7_IRQHandler(void)  //UART2发送完成DMA中断
{    
    if(DMA_GetFlagStatus(DMA1_FLAG_TC7)==SET){  
        //    DMA_Cmd (DMA1_Channel7,DISABLE);          
            DMA_ClearFlag(DMA1_FLAG_TC7); 
    }    
}

