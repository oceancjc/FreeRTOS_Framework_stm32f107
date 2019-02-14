#ifndef __USART_H
#define __USART_H
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include "stm32f10x.h" 


#define DMA                   ( 1 )

#define USART1_PIN_REMAP      ( 1 )
#define USART2_PIN_REMAP      ( 0 )

#define UART1_DEBUG 
#define MAXBUF               ( 500 )


extern uint8_t Msgget[MAXBUF];
extern uint8_t Msgget2[MAXBUF];
extern uint16_t SentWaitRcv;
extern uint16_t GsmRcvCnt;
extern uint16_t Debug1RcvCnt;
extern uint8_t DebugBuf_U1[MAXBUF];
extern uint8_t DebugBuf_U2[MAXBUF];
extern uint16_t Debug2RcvCnt;
extern unsigned char key_val;



#ifdef FreeRTOS
    #include "FreeRTOS.h"
    #include "task.h"
    #include "queue.h"
    #include "timers.h"
    #include "semphr.h"
#endif	


#ifdef FreeRTOS
    extern TaskHandle_t uart1print_tskhandler;
#endif


void Uart1Init(uint32_t bound);
void Uart1SendHex(uint8_t ch);
void Uart1SendStr(char* str);
void uart1_printf(const char* format,...);
void Uart2Init(uint32_t bound);
void Uart2SendHex(uint8_t ch);
void Uart2SendStr(char* str);
void Uart3Init(uint32_t bound);
void Uart3SendHex(uint8_t ch);
void Uart3SendStr(uint8_t* str);
void CleanGsmRcv(void);



uint8_t Hand(unsigned char *a);
uint8_t* Msg_get(void);
#endif
