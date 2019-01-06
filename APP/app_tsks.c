#include "app_tsks.h"

#define MAX_KEY_NUM    12
#define RC1 0x802D000
#define RC2 0x802D800
#define RC3 0x802E000

#define LED_D3_ON() GPIO_SetBits(GPIOB, GPIO_Pin_8)
#define LED_D3_OFF() GPIO_ResetBits(GPIOB, GPIO_Pin_8)

#define LED_D2_ON() GPIO_ResetBits(GPIOB, GPIO_Pin_5)
#define LED_D2_OFF() GPIO_SetBits(GPIOB, GPIO_Pin_5)


TaskHandle_t tsk_handler[3];
TaskHandle_t Uart1SendBackTsk = NULL, CmdServerTsk = NULL;
TaskHandle_t IrdaSendTsk = NULL, IrdaLearnTsk = NULL;
TaskHandle_t SteeringEngCtlTsk = NULL;
TSK_PARAMETER_t SteeringCtlPara = { 0 };
TaskHandle_t lantcps_loopbackTsk = NULL;
TSK_PARAMETER_t lanloopbackPara = { 0 };
TSK_PARAMETER_t tsk_parameter[3];


/*******************************************************************
**************** Command Server Task ****************************
*******************************************************************/
static unsigned char send_finish = 0;
/****************************************************************
Commands
learn %d        learn the irda key, valid only irda learning task exists
                >=MAX_KEY_NUM: Save all key buf to Flash 
use   %d        send the irda key, valid only irda sending task exists
remote_set %d   1: start irda learning task and delete irda sending task if exists
                0: start irda sending task and delete irda learning task if exists 
moter_ctl %d    1: start steering engine control task and reset to 90 degree position
                0: delete steering engine control task 
moter_set %d    use when moter_ctl is set to 1  %d belong to 0-180, stand for degree, int only
****************************************************************/
void cmd_analysis_Task(void *pvParameters){
    char* cmd = (char*)pvPortMalloc(20*sizeof(char));  int opdata = 0;
    while(1){
        ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
        sscanf((char*)Msgget,"%s %d",cmd,&opdata);
        if(strstr((char*)cmd,"learn")){  
            if(IrdaLearnTsk == NULL){
                uart1_printf("Please Enter RC learning mode first\r\n");
                continue;
            }
            tsk_parameter[1].opdata[0] = opdata;
            strcpy(tsk_parameter[1].cmd,cmd);
            tsk_parameter[1].tsk2notify = IrdaLearnTsk;
            xTaskNotifyGive(tsk_parameter[1].tsk2notify);
        }
        else if(strstr((char*)cmd,"use")){
            if(IrdaSendTsk == NULL){
                uart1_printf("Please Enter RC Sending mode first\r\n");
                continue;
            }
            send_finish = 0;
            tsk_parameter[0].opdata[0] = opdata;
            strcpy(tsk_parameter[0].cmd,cmd);
            tsk_parameter[0].tsk2notify = IrdaSendTsk;
            xTaskNotifyGive(tsk_parameter[0].tsk2notify);
        }
        else if(strstr((char*)cmd,"remote_set")){
            if(opdata == 1 && IrdaLearnTsk == NULL){
                if(IrdaSendTsk){    
                    vTaskDelete(IrdaSendTsk);    IrdaSendTsk = NULL;    
                }
                xTaskCreate(irda_learning_Task, "code_learning", 800, &tsk_parameter[1], tskIDLE_PRIORITY + 2, &IrdaLearnTsk);
            }    
            else if(opdata != 1 && IrdaSendTsk == NULL){
                if(IrdaLearnTsk){
                    vTaskDelete(IrdaLearnTsk);    IrdaLearnTsk = NULL;    
                }
                xTaskCreate(irda_sending_Task, "code_sending", configMINIMAL_STACK_SIZE, &tsk_parameter[0], tskIDLE_PRIORITY + 2, &IrdaSendTsk);
            }                   
        }
        else if(strstr((char*)cmd,"moter_ctl")){
            if(opdata == 1 && SteeringEngCtlTsk == NULL )    
                xTaskCreate(steeringCtl_Task, "Steering Engine", configMINIMAL_STACK_SIZE, 
                            (void*)&SteeringCtlPara, tskIDLE_PRIORITY + 2, &SteeringEngCtlTsk);
            else if(opdata != 1 && SteeringEngCtlTsk != NULL){
                Timer3_pwm_Deinit();
                Uart1SendStr("Exit Steering Enginee Ctl\r\n");
                vTaskDelete(SteeringEngCtlTsk);    SteeringEngCtlTsk = NULL;
            }
        }
        else if(strstr((char*)cmd,"moter_set")){
            if(SteeringEngCtlTsk == NULL){
                Uart1SendStr("Enter moter control mode first\r\n");
                continue;
            }    
            SteeringCtlPara.opdata[0] = opdata;
            strcpy(SteeringCtlPara.cmd,cmd);
            SteeringCtlPara.tsk2notify = SteeringEngCtlTsk;
            xTaskNotifyGive(SteeringCtlPara.tsk2notify);
        }
        else if(strstr((char*)cmd,"lan_enable")){
            if(lantcps_loopbackTsk == NULL){
                lanloopbackPara.opdata[0] = opdata;
                xTaskCreate(lantcpserver_loopback_Task, "Lan TCP Server Loopback", configMINIMAL_STACK_SIZE, 
                             (void*)&lanloopbackPara, tskIDLE_PRIORITY + 2, &lantcps_loopbackTsk);            
            }
            else    Uart1SendStr("Already enabled Lan\r\n");

        }
        else if(strstr((char*)cmd,"lan_disable")){
            if(lantcps_loopbackTsk != NULL){
                vTaskDelete(lantcps_loopbackTsk);    lantcps_loopbackTsk = NULL;
            }
        }
        else    Uart1SendStr("Invalid Task\r\n");
        memset(Msgget,0,MAXBUF*sizeof(char));
    }
    vPortFree(cmd);
    vTaskDelete(NULL);
    
}


/****************************************************************************
********************* LED Related Tasks *************************************
****************************************************************************/
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // ?? GPIOB ???    

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_5 ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    LED_D2_ON();
    LED_D3_ON();
}


/**************************************************************
Use delay to toggle LED state
**************************************************************/
void LED_D1_Task(void *pvParameters){
    Uart1SendStr("You are in task --- LED \r\n");
    while (1){
        LED_D3_ON();
        vTaskDelay(pdMS_TO_TICKS(100));
//        TIM2_delay_us(560);
        LED_D3_OFF();
        vTaskDelay(pdMS_TO_TICKS(100)); 
//        TIM2_delay_us(5600);        
    }
    vTaskDelete(NULL);
}


/******************************************************
* Use software timer to toggle state as heart signal
* Should not use any function that may blocking 
* The prototype of the callback function of sw timer:
    void xxxxx(TimerHandle_t xTimer)
******************************************************/
void LED_D2_toggle(TimerHandle_t xTimer){
    static unsigned char state = 0;
    state = ~state;
    state? LED_D2_ON(): LED_D2_OFF(); 
//    Uart1SendStr("You are in task 2\r\n");  
}


/***********************************************************************
******************** Irda Remote Control Tasks *************************
***********************************************************************/
void irda_learning_Task(void *pvParameters){
    Uart1SendStr("You are in task --- irda_learning \r\n");
    code_state = 0;
    int idle_for_code_state = 0;
    ulTaskNotifyTake( pdTRUE, 0 );
    TSK_PARAMETER_t* tskparam = (TSK_PARAMETER_t*)pvParameters;
    short* code_set[MAX_KEY_NUM] = {NULL};
      for(idle_for_code_state = 0; idle_for_code_state < MAX_KEY_NUM;idle_for_code_state++){
          code_set[idle_for_code_state] = (short*)pvPortMalloc(80*sizeof(short));
          memcpy(code_set[idle_for_code_state],Flash_ReadHalfWord(RC1+80*idle_for_code_state*2),80*sizeof(short));
    }
      idle_for_code_state = 0;
//    Flash_ErasePage(RC1);
    while(1){
        ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
        if(tskparam->opdata[0] >= MAX_KEY_NUM){
            Flash_ErasePage(RC1);
            unsigned char i=0;
            for(i =0;i<MAX_KEY_NUM;i++)    Flash_WriteHalfWord(RC1+160*i, (unsigned short*)code_set[i], 80);
            Uart1SendStr("Saving key value to flash\r\n");
            continue;
        }
        Uart1SendStr("Start Recording...\r\n");
        key_val_buffer_pt[1] = 0;
        TIM_ITConfig(TIM4,TIM_IT_CC4| TIM_IT_Update,ENABLE);
        while(1){
            if(code_state == 2 && idle_for_code_state == 0)        idle_for_code_state = 1;
            else if(code_state == 2 && idle_for_code_state == 1)   break;
            else                                                   idle_for_code_state = 0;
            vTaskDelay(pdMS_TO_TICKS(50));
        }   
        TIM_ITConfig(TIM4,TIM_IT_CC4| TIM_IT_Update,DISABLE);        
        code_state = 0;    
        if( key_val_buffer_pt[1] <= 3 ){
            Uart1SendStr("noise is coming...abort\r\n");
            continue;
        }    
        uart1_printf("record success in code_set[%d]\r\n",tskparam->opdata[0]);
        memcpy(code_set[tskparam->opdata[0]],key_val_buffer_pt,(key_val_buffer_pt[1]+2)*sizeof(short));
//        Flash_WriteHalfWord(RC1+160*tskparam->opdata[0], (unsigned short*)code_set[tskparam->opdata[0]], 80);

               
    }
    vPortFree(key_val_buffer_pt);
    vTaskDelete(NULL);
}




/*unsigned short rr[80] = {0};
unsigned short tt[80] = {9000,4500,560,560,560,1690,560,560,560,560,560,1690,560,1690,560,560,560,1690,//01001101 
                         560,1690,560,560,560,1690,560,1690,560,560,560,560,560,1690,560,560,  // 10110010
                         560,560,560,560,560,1690,560,1690,560,1690,560,560,560,1690,560,1690,      //00111011
                         560,1690,560,1690,560,560,560,560,560,560,560,1690,560,560,560,560,560//11000100 2
                        };*/
void code_sending(unsigned int address){
    unsigned short *code = Flash_ReadHalfWord(address);
    unsigned char i = 0;
    LED_D3_OFF();
    Timer3_pwm_off(0);
    for(i=0;i<code[1]-1;i++){
        if(i%2 == 0){
            LED_D3_ON();
            Timer3_pwm_on(0);
        }
        else{
            LED_D3_OFF();
            Timer3_pwm_off(0);            
        }
        if(code[3+i]>code[2+i]){   TIM2_delay_us( code[3+i]-code[2+i] );    }
        else                   {   TIM2_delay_us( MAXCOUNTER -code[2+i] + code[3+i] );  }
    }
    LED_D3_OFF();
    Timer3_pwm_off(0);
//    for(i=0;i<11;i++){
//        sprintf(cmdrr,"code = %d,%d,%d,%d,%d,%d,%d\r\n",rr[7*i+0],rr[7*i+1],rr[7*i+2],rr[7*i+3],rr[7*i+4],rr[7*i+5],rr[7*i+6]);
//        Uart1SendStr(cmdrr);
//    }
}

void code_sending_fix(){
//    int i =0;
//    while(tt[i]!=0){
//        if(i%2 == 0){
//            LED_D3_ON();
//            Timer3_pwm_on(0);
//        }
//        else{
//            LED_D3_OFF();
//            Timer3_pwm_off(0);            
//        }
//        TIM2_delay_us(tt[i++]);        
//    }
    Timer3_pwm_off(0);
}



void irda_sending_Task(void *pvParameters){
    Uart1SendStr("You are in task --- irda_sending\r\n");
    ulTaskNotifyTake( pdTRUE, 0 );    
    TSK_PARAMETER_t* tskparam = (TSK_PARAMETER_t*)pvParameters;
    while(1){
        ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
        if(send_finish == 0){
            code_sending(RC1+80*tskparam->opdata[0]*2);
        } 
        Uart1SendStr("Send signal out\r\n");
        send_finish = 1;        
    }
    vTaskDelete(NULL);
}



/************************************************************************
***************** Uart1 Print back Tasks ********************************
************************************************************************/
void Uart1_print_back_Task(void *pvParameters){
    #if ( configUSE_TRACE_FACILITY == 1 )
        vTracePrintF(0x0023,"uart1printback_begin In");
    #endif
    uint32_t ulEventsToProcess = 0;
    uart1print_tskhandler = xTaskGetCurrentTaskHandle();
    #if ( configUSE_TRACE_FACILITY == 1 )
        vTracePrintF(0x0024,"uart1printback_begin Out");
    #endif
    while(1){
        ulEventsToProcess = ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
        if(ulEventsToProcess == 1)        Uart1SendStr((char*)Msgget); 
        else if(ulEventsToProcess > 1)    uart1_printf("Send too fast, overflow: %d\n",ulEventsToProcess);        
        xTaskNotifyGive((TaskHandle_t)pvParameters);        
    }
}



void steeringCtl_Task(void *pvParameters){
    Uart1SendStr("You are in task --- Steering Engine Control\r\n");
    Timer3_pwm_Init(200,7200);  //ÖÜÆÚ = 72e6 / 200 /7200 = 50Hz
    Timer3_pwm_off(0);
    Timer3_setPulseWidth(0, 1.5/20);    //Reset Steering Engine
    Timer3_pwm_on(0);
    TSK_PARAMETER_t* para = (TSK_PARAMETER_t*)pvParameters;
    vTaskDelay(pdMS_TO_TICKS(200));
    while(1){
        Timer3_pwm_off(0);
        ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
        if(para->opdata[0]> 180){
            uart1_printf("Invalid angle:%d\r\n",para->opdata[0]);
            continue;
        }   
        Timer3_setPulseWidth(0, (para->opdata[0]/90.+0.5)/20);      //0--0.5   180--2.5
        Timer3_pwm_on(0);
        uart1_printf("Angle set:%d\r\n",para->opdata[0]);   
        vTaskDelay(pdMS_TO_TICKS(200));        
        
    }     
}



void lantcpserver_loopback_Task(void *pvParameters){
    Uart1SendStr("You are in task --- Lan TCP Server Loopback\r\n");
    w5500Reset();
    int ret = w5500Init( ((TSK_PARAMETER_t*)pvParameters)->opdata[0]);
    if(ret){
        uart1_printf("W5520 Init fail, Err = %d\r\n",ret);
        
    }    
    while(1){
        /* Loopback Test */
        // TCP server loopback test
        if( (ret = loopback_tcps(SOCK_TCPS, gDATABUF, 5000)) < 0) {
            uart1_printf("SOCKET ERROR : %ld\r\n", ret);
        }
        // UDP server loopback test
//        if( (ret = loopback_udps(SOCK_UDPS, gDATABUF, 3000)) < 0) {
//            uart1_printf("SOCKET ERROR : %ld\r\n", ret);
//        }

    }
    
}

