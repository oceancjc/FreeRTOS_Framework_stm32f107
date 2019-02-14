/**  UART1 driver machenism   Create by oceancjc  yhmcjc@sina.com
* This driver can be used with/without OS and with/whout DMA Tx mode simutaniously, always DMA Rx mode.
* In most cases, DMA is recommended to use because the chip supports this function as well as relase cpu load
* Withoout DMA mode, uart send string byte by byte using while checking mode
* !!! DMA mode must be disabled before re-enable again

* With DMA mode and without OS, uart send string in DMA Uart Tx interrupte mode:
      Initial UART with dma_uartx_txready = 1
            Wait till dma_uartx_txready signal = 1 before start dma transmission for fear last transmission not finished
            Disable DMA and re-enable DMA transmission and assign dma_uartx_txready = 0
            In DMA transmission complete IRQ, clear status and assign dma_uartx_txready = 1
* With DMA mode and without OS, uart send string in Uart Rx Idle interrupte mode:
      Clear Flag and copy data from DMA Rx buffer to global variable Msgget
            Diable and re-enable DMA Rx channel

* With DMA mode and with OS, uart send string in DMA Uart Tx interrupte mode:
      Initial UART with binary semaphore(bsem) semb_dma_uartx initialized and give bsem
            Take bsem semb_dma_uartx and start uart-dma transmission
            In DMA transmission complete IRQ, clear status and give back bsem semb_dma_uartx
* With DMA mode and with OS, uart send string in Uart Rx Idle interrupte mode:
      Clear Flag and copy data from DMA Rx buffer to global variable Msgget
            Diable and re-enable DMA Rx channel
            Task notify uart1print_tskhandler. It is a task which printback what uart received and pass to post-process task as well
            The post process task handler is in the input parameter of uart1print_tskhandler's task, won't describe here.

TODO:
    All global varibals will be removed and managed in globals.c
*/

#include "usart.h"



#ifdef FreeRTOS
    static SemaphoreHandle_t semb_dma_uart1 = NULL;
    static SemaphoreHandle_t semb_dma_uart2 = NULL;
    TaskHandle_t uart1print_tskhandler = NULL;
    #if ( configUSE_TRACE_FACILITY == 1 )
        static traceHandle UART1IDLEHandle = 0;
        static traceHandle DMATx1Handle = 0;
        static traceHandle UART2IDLEHandle = 0;
        static traceHandle DMATx2Handle = 0;
    #endif
#else
    static char              dma_uart1_txready = 0;
    static char              dma_uart2_txready = 0;
#endif

static uint8_t* UART1_RxBuffer = NULL;   //Uart1����buf
static uint8_t* UART1_TxBuffer = NULL;  //Uart1����buf
static uint8_t* UART2_RxBuffer = NULL;   //Uart1����buf
static uint8_t* UART2_TxBuffer = NULL;  //Uart1����buf
uint8_t Msgget[ MAXBUF ] = {0};
uint8_t Msgget2[ MAXBUF ] = {0};
uint16_t GsmRcvCnt = 0;
uint16_t Debug1RcvCnt = 0;
uint8_t DebugBuf_U1[ MAXBUF ] = {0};
uint8_t DebugBuf_U2[ MAXBUF ] = {0};
uint16_t Debug2RcvCnt = 0;
unsigned char key_val = 0;


static void USART1_DMA_Config(void){
    DMA_InitTypeDef DMA_InitStructure = { 0 };
    NVIC_InitTypeDef NVIC_InitStructure = { 0 };
    
        /*DMA Irq config for Uart1 Tx*/
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 13;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
    NVIC_Init(&NVIC_InitStructure);
    
    /*DMA Irq config for Uart1 Rx*/
//     NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;  //DMA for UART1����
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
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE); //����UART_DMA���书��
}

void Uart1Init(uint32_t bound){
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure = { 0 };
    USART_InitTypeDef USART_InitStructure = { 0 };
    NVIC_InitTypeDef NVIC_InitStructure = { 0 };
     
    #if USART1_PIN_REMAP
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);
        GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE); //uart1 �ܽ�����ӳ��
        //USART1_TX   PB.6
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
        //USART1_RX      PB.7
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOB, &GPIO_InitStructure);  
    #else
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
        //USART1_TX   PA.9
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        //USART1_RX      PA.10
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOA, &GPIO_InitStructure);  
    #endif

   //Usart1 NVIC ����

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;    //

    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;        //IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);    //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���USART1
   
    USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    #ifdef FreeRTOS
        UART1_RxBuffer = (uint8_t*)pvPortMalloc(MAXBUF*sizeof(uint8_t));
        UART1_TxBuffer = (uint8_t*)pvPortMalloc(MAXBUF*sizeof(uint8_t));
    #else
        UART1_RxBuffer = (uint8_t*)malloc(MAXBUF*sizeof(uint8_t));
        UART1_TxBuffer = (uint8_t*)malloc(MAXBUF*sizeof(uint8_t));
    #endif
    if(UART1_TxBuffer!=NULL)    memset(UART1_TxBuffer,0,MAXBUF*sizeof(char));
    if(UART1_RxBuffer!=NULL)    memset(UART1_RxBuffer,0,MAXBUF*sizeof(char));
    USART1_DMA_Config();
    
    USART_Init(USART1, &USART_InitStructure);
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//�����ж�
    USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 

#ifdef FreeRTOS
    semb_dma_uart1 = xSemaphoreCreateBinary();
    #if ( configUSE_TRACE_FACILITY == 1 )
        vTraceSetSemaphoreName(semb_dma_uart1, "semb_dma_uart1");
        DMATx1Handle = xTraceSetISRProperties("UART1_DMATx_IRQ", 1);
        UART1IDLEHandle = xTraceSetISRProperties("UART1_IDLE_IRQ", 2);
    #endif
    xSemaphoreGive( semb_dma_uart1 );
#else
    dma_uart1_txready = 1;
#endif

}

void Uart1SendHex(uint8_t ch){
    while((USART1->SR & 0x40)==0);//ѭ������,ֱ���������   
    USART1->DR = ch; 
}




void Uart1SendStr(char* str){
    #ifdef FreeRTOS
        xSemaphoreTake( semb_dma_uart1, portMAX_DELAY );
    #endif
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    #if DMA == 0
        uint16_t cnt=0;
        while( *(str+cnt)){    //�ж�һ�������Ƿ����
            USART_SendData(USART1, *(str+cnt));
            while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);//�������               
            cnt++;    //׼������һ������
        }
        #ifdef FreeRTOS
            xSemaphoreGive( mutex_dma_uart1 );
        #endif
    #else    
        #ifndef FreeRTOS
            while(dma_uart1_txready == 0);
        #endif
        strcpy((char*)UART1_TxBuffer,str);  
        DMA_Cmd (DMA1_Channel4,DISABLE);     //20181215:  Can this line removed ? 
        DMA_SetCurrDataCounter( DMA1_Channel4,strlen(str) ); 
        DMA_Cmd (DMA1_Channel4,ENABLE);
        USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE); //����UART_DMA���书�� 
        #ifndef FreeRTOS
            dma_uart1_txready = 0;
        #endif
        /*Release the binary sem in DMA send finish IRQ*/      
    #endif
}


void uart1_printf(const char* format,...){
    #ifdef FreeRTOS
        xSemaphoreTake( semb_dma_uart1, portMAX_DELAY );
    #endif
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    va_list ap = { 0 };
    va_start(ap, format);
    int len = vsprintf ((char*)UART1_TxBuffer, format, ap);   
    va_end(ap);    
    #if DMA == 0
        uint16_t cnt=0;
        while( *(UART1_TxBuffer+cnt)){    //�ж�һ�������Ƿ����
            USART_SendData(USART1, *(UART1_TxBuffer+cnt));
            while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);//�������               
            cnt++;    //׼������һ������
        }
        #ifdef FreeRTOS
            xSemaphoreGive( mutex_dma_uart1 );
        #endif
    #else    
        //strcpy((char*)UART1_TxBuffer,str); 
        #ifndef FreeRTOS
            while(dma_uart1_txready == 0);
        #endif
        DMA_Cmd (DMA1_Channel4,DISABLE);  //20181215:  Can this line removed ? A: No. DMA should disable before re-enable
        DMA_SetCurrDataCounter( DMA1_Channel4,len+1 ); 
        DMA_Cmd (DMA1_Channel4,ENABLE);
        USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE); //����UART_DMA���书��
        #ifndef FreeRTOS
            dma_uart1_txready = 0;
        #endif
        /*Release the binary sem in DMA send finish IRQ*/        
     #endif
}





/***************************************       �жϺ���        ************************************/
void USART1_IRQHandler(void){//IDLE�ж�
    uint32_t DATA_LEN = 0;
    #if ( configUSE_TRACE_FACILITY == 1 )
        vTracePrintF(0x001,"UART1_IDLE_IRQ In");
        vTraceStoreISRBegin(UART1IDLEHandle);
    #endif
    if(USART_GetITStatus(USART1, USART_IT_IDLE) == SET){
        DMA_Cmd(DMA1_Channel5, DISABLE);
/*---------------- �ȶ�SR,�ٶ�DR�������IDLE -----------------------------------------*/            
        DATA_LEN = USART1->SR;
        DATA_LEN = USART1->DR;  
        DATA_LEN = MAXBUF - DMA_GetCurrDataCounter(DMA1_Channel5);            

        if(DATA_LEN > 0){
            snprintf((char*)Msgget,DATA_LEN+1, (char*)UART1_RxBuffer);
            #ifdef FreeRTOS
                BaseType_t pxHigherPriorityTaskWoken = pdTRUE;
                vTaskNotifyGiveFromISR( uart1print_tskhandler, &pxHigherPriorityTaskWoken );
                portYIELD_FROM_ISR( pxHigherPriorityTaskWoken );
                #if ( configUSE_TRACE_FACILITY == 1 )
                    vTracePrintF(0x003,"%d:%s\n",DATA_LEN,UART1_RxBuffer);
                #endif
            #endif
        }
        DMA_ClearFlag(DMA1_FLAG_GL5);
        USART_ClearITPendingBit(USART1, USART_IT_IDLE); 
        DMA_SetCurrDataCounter(DMA1_Channel5,MAXBUF);
        DMA_Cmd(DMA1_Channel5, ENABLE);  
        
                
    }
    #if ( configUSE_TRACE_FACILITY == 1 )
        vTracePrintF(0x002,"UART1_IDLE_IRQ Out");
        vTraceStoreISREnd(1);
    #endif
}


void DMA1_Channel4_IRQHandler(void){  //UART1�������DMA�ж� 
    #if ( configUSE_TRACE_FACILITY == 1 )
        vTraceStoreISRBegin(DMATx1Handle);
    #endif    
    if(DMA_GetFlagStatus(DMA1_FLAG_TC4)==SET){   
        //    DMA_Cmd (DMA1_Channel4,DISABLE);          
        DMA_ClearFlag(DMA1_FLAG_TC4); 
        #ifdef FreeRTOS
            xSemaphoreGiveFromISR( semb_dma_uart1, NULL );
        #else
            dma_uart1_txready = 1;
        #endif
     } 
    #if ( configUSE_TRACE_FACILITY == 1 )
        vTraceStoreISREnd(0);
    #endif    
}






void USART2_DMA_Config(void){
    DMA_InitTypeDef DMA_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;  //DMA for UART1����
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
    
    DMA_DeInit(DMA1_Channel6);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DR);       
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)UART2_RxBuffer;    
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;    
    DMA_InitStructure.DMA_BufferSize = MAXBUF;   
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;    
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;     
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;     
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;  
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; 
    DMA_Init(DMA1_Channel6, &DMA_InitStructure);        
    DMA_Cmd (DMA1_Channel6,ENABLE);
    USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE); //����UART_DMA���书��
              
}


void Uart2Init(uint32_t bound)
{
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
       
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    #if USART2_PIN_REMAP
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_AFIO, ENABLE);
        GPIO_PinRemapConfig(GPIO_Remap_USART2,ENABLE); //uart2 �ܽ�����ӳ��
        //USART2_TX   PD.5
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOD, &GPIO_InitStructure);
        //USART2_RX      PD.6
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOD, &GPIO_InitStructure);  
      #else
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        //USART2_TX   PA.2  
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;    //PA2
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        //USART2_RX   PA.3
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
        GPIO_Init(GPIOA, &GPIO_InitStructure);  
      #endif

   //Usart2 NVIC ����

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;    //

    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;        //IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);    //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���USART1
   
    USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    #ifdef FreeRTOS
        UART2_RxBuffer = (uint8_t*)pvPortMalloc(MAXBUF*sizeof(uint8_t));
        UART2_TxBuffer = (uint8_t*)pvPortMalloc(MAXBUF*sizeof(uint8_t));
    #else
        UART2_RxBuffer = (uint8_t*)malloc(MAXBUF*sizeof(uint8_t));
        UART2_TxBuffer = (uint8_t*)malloc(MAXBUF*sizeof(uint8_t));
    #endif
    if(UART2_TxBuffer!=NULL)    memset(UART2_TxBuffer,0,MAXBUF*sizeof(char));
    if(UART2_RxBuffer!=NULL)    memset(UART2_RxBuffer,0,MAXBUF*sizeof(char));
    USART2_DMA_Config();
    
    USART_Init(USART2, &USART_InitStructure);
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);//�����ж�
    USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 

#ifdef FreeRTOS
    semb_dma_uart2 = xSemaphoreCreateBinary();
    #if ( configUSE_TRACE_FACILITY == 1 )
        vTraceSetSemaphoreName(semb_dma_uart2, "semb_dma_uart1");
        DMATx2Handle = xTraceSetISRProperties("UART2_DMATx_IRQ", 1);
        UART2IDLEHandle = xTraceSetISRProperties("UART2_IDLE_IRQ", 2);
    #endif
    xSemaphoreGive( semb_dma_uart2 );
#else
    dma_uart2_txready = 1;
#endif                   //ʹ�ܴ��� 
    
}



void Uart2SendHex(uint8_t ch){
    while((USART2->SR & 0x40)==0);//ѭ������,ֱ���������   
    USART2->DR = ch; 
}



void Uart2SendStr(char* str){
    #ifdef FreeRTOS
        xSemaphoreTake( semb_dma_uart2, portMAX_DELAY );
    #endif
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
    #if DMA == 0
        uint16_t cnt=0;
        while( *(str+cnt)){    //�ж�һ�������Ƿ����
            USART_SendData(USART2, *(str+cnt));
            while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);//�������               
            cnt++;    //׼������һ������
        }
        #ifdef FreeRTOS
            xSemaphoreGive( mutex_dma_uart2 );
        #endif
    #else    
        #ifndef FreeRTOS
            while(dma_uart2_txready == 0);
        #endif
        strcpy((char*)UART2_TxBuffer,str);  
        DMA_Cmd (DMA1_Channel7,DISABLE);     //20181215:  Can this line removed ? 
        DMA_SetCurrDataCounter( DMA1_Channel7,strlen(str) ); 
        DMA_Cmd (DMA1_Channel7,ENABLE);
        USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE); //����UART_DMA���书�� 
        #ifndef FreeRTOS
            dma_uart2_txready = 0;
        #endif
        /*Release the binary sem in DMA send finish IRQ*/      
    #endif
}


void uart2_printf(const char* format,...){
    #ifdef FreeRTOS
        xSemaphoreTake( semb_dma_uart2, portMAX_DELAY );
    #endif
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
    va_list ap = { 0 };
    va_start(ap, format);
    int len = vsprintf ((char*)UART2_TxBuffer, format, ap);   
    va_end(ap);    
    #if DMA == 0
        uint16_t cnt=0;
        while( *(UART2_TxBuffer+cnt)){    //�ж�һ�������Ƿ����
            USART_SendData(USART2, *(UART2_TxBuffer+cnt));
            while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);//�������               
            cnt++;    //׼������һ������
        }
        #ifdef FreeRTOS
            xSemaphoreGive( mutex_dma_uart2 );
        #endif
    #else    
        //strcpy((char*)UART1_TxBuffer,str); 
        #ifndef FreeRTOS
            while(dma_uart2_txready == 0);
        #endif
        DMA_Cmd (DMA1_Channel7,DISABLE);  //20181215:  Can this line removed ? A: No. DMA should disable before re-enable
        DMA_SetCurrDataCounter( DMA1_Channel7,len+1 ); 
        DMA_Cmd (DMA1_Channel7,ENABLE);
        USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE); //����UART_DMA���书��
        #ifndef FreeRTOS
            dma_uart2_txready = 0;
        #endif
        /*Release the binary sem in DMA send finish IRQ*/        
     #endif
}





/***************************************       �жϺ���        ************************************/
void USART2_IRQHandler(void){//IDLE�ж�
    uint32_t DATA_LEN = 0;
    #if ( configUSE_TRACE_FACILITY == 1 )
        vTracePrintF(0x001,"UART2_IDLE_IRQ In");
        vTraceStoreISRBegin(UART2IDLEHandle);
    #endif
    if(USART_GetITStatus(USART2, USART_IT_IDLE) == SET){
        DMA_Cmd(DMA1_Channel6, DISABLE);
/*---------------- �ȶ�SR,�ٶ�DR�������IDLE -----------------------------------------*/            
        DATA_LEN = USART2->SR;
        DATA_LEN = USART2->DR;  
        DATA_LEN = MAXBUF - DMA_GetCurrDataCounter(DMA1_Channel6);            

        if(DATA_LEN > 0){
            snprintf((char*)Msgget2,DATA_LEN+1, (char*)UART2_RxBuffer);
            SentWaitRcv = DATA_LEN;
            #ifdef FreeRTOS
                #if ( configUSE_TRACE_FACILITY == 1 )
                    vTracePrintF(0x003,"%d:%s\n",DATA_LEN,UART1_RxBuffer);
                #endif
            #endif
        }
        DMA_ClearFlag(DMA1_FLAG_GL6);
        USART_ClearITPendingBit(USART2, USART_IT_IDLE); 
        DMA_SetCurrDataCounter(DMA1_Channel6,MAXBUF);
        DMA_Cmd(DMA1_Channel6, ENABLE);  
        
                
    }
    #if ( configUSE_TRACE_FACILITY == 1 )
        vTracePrintF(0x002,"UART2_IDLE_IRQ Out");
        vTraceStoreISREnd(1);
    #endif
}


void DMA1_Channel7_IRQHandler(void){  //UART1�������DMA�ж� 
    #if ( configUSE_TRACE_FACILITY == 1 )
        vTraceStoreISRBegin(DMATx2Handle);
    #endif    
    if(DMA_GetFlagStatus(DMA1_FLAG_TC7)==SET){   
        //    DMA_Cmd (DMA1_Channel7,DISABLE);          
        DMA_ClearFlag(DMA1_FLAG_TC7); 
        #ifdef FreeRTOS
            xSemaphoreGiveFromISR( semb_dma_uart2, NULL );
        #else
            dma_uart2_txready = 1;
        #endif
     } 
    #if ( configUSE_TRACE_FACILITY == 1 )
        vTraceStoreISREnd(0);
    #endif    
}



void Uart3Init(uint32_t bound)
{
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);  
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3 , ENABLE);
     //USART3_TX   PB.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
   
    //USART3_RX      PB.11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);  

   //Usart3 NVIC ����

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;    //

    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;        //IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);    //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���USART3
  
   //USART ��ʼ������

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);  //RCC_APB1_USART3ʹ��
    USART_InitStructure.USART_BaudRate = 9600;  //������
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //�����ֳ�8
    USART_InitStructure.USART_StopBits = USART_StopBits_1;  //ֹͣλΪ1
    USART_InitStructure.USART_Parity = USART_Parity_No;  //��żʧ��
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;  //����ʹ��  ����ʹ��
    USART_Init(USART3, &USART_InitStructure);

    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�����ж�
   
    USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ���
}

void USART3_IRQHandler(void)                    //����3�жϷ������
{

}

void Uart3SendHex(uint8_t ch)
{
    while((USART3->SR&0X40)==0);//ѭ������,ֱ���������   
    USART3->DR = (u8) ch; 
}

void Uart3SendStr(u8* str)
{
    u16 cnt=0;
    while(*(str+cnt))    //�ж�һ�������Ƿ����
    {
    while((USART3->SR&0X40)==0);//ѭ������,ֱ���������   
        USART3->DR = *(str+cnt);
    cnt++;    //׼������һ������
    }
}



//void CleanGsmRcv(void)
//{
//    u16 i = 0;
//    for(i=0; i<MAXRCV-1; i++)
//    {
//    UART1_Rcv[i] = 0;    
//    }

//    GsmRcvCnt = 0;    
//}

//uint8_t Hand(unsigned char *a)
//{ 
//    if(strstr(UART1_Rcv,a)!=NULL)
//        return 1;
//    else
//    return 0;
//}

//u8* Msg_get(void){    //����Ҫ�������Ϣȫ������Msgget�����У���ֹͻȻ����Ϣ����UART1_Rcv�ı�
//    strcpy(Msgget,UART1_Rcv);
//    CleanGsmRcv();
//    return strstr(Msgget,"0891");  //0D0A

//} 
