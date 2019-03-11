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
#include "app_tsks.h"
#include "sx1276.h"
#include "w5500interface.h"

/***************************************************************************
GPIOA.3  -- DIO0 for SX1278   RST for W5500
GPIOA.4  -- NSS3 PIN for SX1278
GPIOA.5  -- SPI1.CLK
GPIOA.6  -- TIMER3.CH1 SPI1.MISO
GPIOA.7  -- TIMER3.CH2 SPI1.MOSI
GPIOA.10 -- DHT11.DATA
GPIOB.0  -- TIMER3.CH3
GPIOB.1  -- TIMER3.CH4
GPIOB.5  -- LED2
GPIOB.6  -- USART1.Tx
GPIOB.7  -- USART1.Rx
GPIOB.8  -- LED8
GPIOB.9  -- TIMER4.CH4
GPIOB.13 -- SPI2.CLK
GPIOB.14 -- SPI2.MOSI
GPIOB.15 -- SPI2.MISO
GPIOC.0  -- DIO1 PIN for SX1278
GPIOC.1  -- DIO3 PIN for SX1278
GPIOC.2  -- DIO4 PIN for SX1278
//GPIOC.3  -- DIO0 PIN for SX1278
GPIOC.4  -- DIO2 PIN for SX1278
GPIOC.5  -- RESET PIN for SX1278
GPIOC.6  -- DIO5 PIN for SX1278
****************************************************************************/



uint16_t leveldio0 = 5;

int main(void){
    SystemInit();
    NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
/* Initialize and enable trace - Currently configured for snapshot mode */
    #if ( configUSE_TRACE_FACILITY == 1 )
	    vTraceEnable(TRC_START);
    #endif
/***************   System Initialization part  ******************/
    LED_Init(); 
    Uart1Init(115200); //Timer init must follow usart, can't change order
//    Uart2Init(9600);
////    Timer3_pwm_Init(237,8);  //周期 = 72e6 / 8 /237 = 37.97KHz
    //Timer4_capture_Init(MAXCOUNTER,72);//周期 = 6ms  分辨率1us
    //TIM_ITConfig(TIM4,TIM_IT_CC4| TIM_IT_Update,DISABLE);
    //Uart1SendStr("Hello oceancjc\r\n"); Uart发送驱动增加了FreeRTOS功能，不能在task外调用
//    Initial_SPI(1,0);
//    SX1276Init(&sx1278device);
//	//leveldio0 = DIO0;
//	//GPIO_ResetBits( NSS_IOPORT, NSS_PIN ); 
//    //leveldio0 = DIO0;
//  	//GPIO_SetBits( NSS_IOPORT, NSS_PIN ); 
//    //leveldio0 = DIO0;
//    SX276TxStart(&sx1278device);
//    uint8_t *data = (uint8_t*)"hello cjc";
//    SX1276Send( &sx1278device, data, 9 );
//    int i = 4096;
//    while(i--);
//    leveldio0 = DIO0;


/****************   OS Tasks Generation part *******************/    

    xTaskCreate(LED_D1_Task, "LED_D1", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
	
    xTaskCreate(cmd_analysis_Task, "Command Server",800,NULL,tskIDLE_PRIORITY+1,&CmdServerTsk);

    xTaskCreate(Uart1_print_back_Task, "UART1_Sendback", 800, (void*)CmdServerTsk, tskIDLE_PRIORITY + 2, &Uart1SendBackTsk);

    TimerHandle_t ptimer = xTimerCreate( "LED_D2_timer", pdMS_TO_TICKS(2000), pdTRUE, 0, LED_D2_toggle );
    if(ptimer!=NULL)    xTimerStart(ptimer,0);

    xTaskCreate(DHT11_Fetch_Task, "DHT11", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3, NULL);


    vTaskStartScheduler(); 
    
    /* Misra failure, Rule 14.1  Unreachable code cannot be avoided */ 
    #pragma diag_suppress=111
    /* If all is well, the scheduler will now be running, and the following
    line will never be reached.  If the following line does execute, then
    there was insufficient FreeRTOS heap memory available for the idle and/or
    timer tasks	to be created.  See the memory management section on the
    FreeRTOS web site for more details. */
    while(1);

    return 0;
    /* Restoring the Misra Rule checking */
    #pragma diag_default=111
    
}




