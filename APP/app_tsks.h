#ifndef __APP_TSKS_H
#define __APP_TSKS_H

#include "stm32f10x.h"
/* FreeRTOs Related Head Files */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

/*System Driver Head Files*/
#include "usart.h"
#include "timer.h"
#include "spi.h"
#include "flash.h"

extern TaskHandle_t IrdaLearnTsk, IrdaSendTsk;
extern TaskHandle_t SteeringEngCtlTsk;
extern TaskHandle_t Uart1SendBackTsk, CmdServerTsk;

typedef struct{
    char cmd[20];
    unsigned int opdata[3]; 
    TaskHandle_t tsk2notify;
}TSK_PARAMETER_t;

extern TSK_PARAMETER_t tsk_parameter[3];
/***********  Tasks Declarision  *************/
extern void LED_Init(void);
extern void LED_D1_Task(void *pvParameters);
extern void irda_sending_Task(void *pvParameters);
extern void LED_D2_toggle( TimerHandle_t xTimer);
extern void irda_learning_Task(void *pvParameters);
extern void cmd_analysis_Task(void *pvParameters); 
extern void code_sending(unsigned int address);
extern void steeringCtl_Task(void *pvParameters);
extern void Uart1_print_back_Task(void *pvParameters);




#endif /* __APP_TSKS_H */

