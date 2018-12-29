/***********************************************************
酷学之家 http://simcom.taobao.com/
联系方式：15296811438 林工
*************************************************************/

#include "stm32f10x.h"
#include "timer.h"
#include "usart.h"

//通用定时器中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!

u8 Flag_108ms[4]={0};


/*

void Timerx_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能

    TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值     计数到5000为500ms
    TIM_TimeBaseStructure.TIM_Prescaler =(psc); //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
    TIM_ITConfig(  //使能或者失能指定的TIM中断
    TIM3, //TIM2
    TIM_IT_Update  |  //TIM 中断源
    TIM_IT_Trigger,   //TIM 触发中断源 
    ENABLE  //使能
    );
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //先占优先级0级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //从优先级3级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

    TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设
                 
}

//TIM3中断,将串口2收到的gsm数据反馈到串口1，将串口1收到的数据发送给gsm模块
void TIM3_IRQHandler(void)   
{
    u16 i = 0;

    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
    {
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清除TIMx的中断待处理位:TIM 中断源
//#ifdef UART1_DEBUG//发送数据用于调试。
    Uart1SendStr((char*)DebugBuf_U2);
    for(i=0; i<MAXBUF; i++)
    {
        DebugBuf_U2[i] = 0;    
    }
    Debug2RcvCnt = 0;

    Uart2SendStr((char*)DebugBuf_U1);
    for(i=0; i<MAXBUF; i++)
    {
        DebugBuf_U1[i] = 0;    
    }
    Debug1RcvCnt = 0;


    }
}

void Timer2_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能

    TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值     计数到5000为500ms
    TIM_TimeBaseStructure.TIM_Prescaler =(psc-1); //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    TIM_ITConfig(  //使能或者失能指定的TIM中断
    TIM2,
    TIM_IT_Update  |  //TIM 中断源
    TIM_IT_Trigger, 
    ENABLE  //使能
    );
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

    TIM_Cmd(TIM2, ENABLE);  //使能TIMx外设
                 
}

void TIM2_IRQHandler(void){
    
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET){ //检查指定的TIM中断发生与否:TIM 中断源 
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //清除TIMx的中断待处理位:TIM 中断源
        Flag_108ms[0]++;    Flag_108ms[1]++;
        Flag_108ms[2]++;    Flag_108ms[3]++;

    }
    
} 

void re_count_108ms(void){
    TIM_Cmd(TIM2, DISABLE);
    TIM_SetCounter(TIM2, 0);
    TIM_Cmd(TIM2, ENABLE);
}*/



/**********************************************************************
**********************  TIM3 Related Functions  ***********************
**********************************************************************/
static uint32_t Timer3TicksPeriod = 0;
void Timer3_pwm_Init(u16 arr,u16 psc){
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    Timer3TicksPeriod = arr;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能
    
    TIM_TimeBaseStructure.TIM_Period = arr-1; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值     计数到5000为500ms
    TIM_TimeBaseStructure.TIM_Prescaler =(psc-1); //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
    
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB , ENABLE); //时钟使能
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /* Configures the TIM3 Channel 1-4 in PWM Mode */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_Pulse = arr/3-1;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    
    
    TIM_OC1Init(TIM3,&TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
    
    TIM_OC2Init(TIM3,&TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
    
    TIM_OC3Init(TIM3,&TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
    
    TIM_OC4Init(TIM3,&TIM_OCInitStructure);
    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
    
    
    
    TIM_CtrlPWMOutputs(TIM3,ENABLE);   //??TIM2?PWM?????
    TIM_ARRPreloadConfig(TIM3, ENABLE); //??TIMx?ARR???????? 
    TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设
                
}

void Timer3_pwm_off(char channel){
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    if(channel == 1){
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        GPIO_ResetBits(GPIOA, GPIO_Pin_6);
    }    
    else if(channel ==2){
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        GPIO_ResetBits(GPIOA, GPIO_Pin_7);    
    }
    else if(channel ==3){
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
        GPIO_ResetBits(GPIOB, GPIO_Pin_0);    
    }
    else if(channel ==4){
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
        GPIO_ResetBits(GPIOB, GPIO_Pin_1);    
    }
    else if(channel ==0){
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
        GPIO_ResetBits(GPIOA, GPIO_Pin_6|GPIO_Pin_7);
        GPIO_ResetBits(GPIOB, GPIO_Pin_0|GPIO_Pin_1);
    }    


}


void Timer3_pwm_on(char channel){
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    if(channel == 1){
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    }    
    else if(channel ==2){
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    }    
    else if(channel ==3){
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
    }    
    else if(channel ==4){
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
    }  
    else if(channel ==0){
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    }    

}

/****************************************************************
Return 
     0:  OK
    -1:  Parameter invalid
****************************************************************/
int Timer3_setPulseWidth(uint8_t channel, float pulsePercent){
    if(pulsePercent > 0.999 || pulsePercent < 0)    return -1;
    register uint16_t cmpval = (uint16_t)(pulsePercent * Timer3TicksPeriod);
    switch(channel){
        case 1:
            TIM_SetCompare1(TIM3,cmpval);    break;
        case 2:
            TIM_SetCompare2(TIM3,cmpval);    break;
        case 3:
            TIM_SetCompare3(TIM3,cmpval);    break;
        case 4:
            TIM_SetCompare4(TIM3,cmpval);    break;
        case 0:
            TIM_SetCompare1(TIM3,cmpval);    TIM_SetCompare2(TIM3,cmpval);
            TIM_SetCompare3(TIM3,cmpval);    TIM_SetCompare4(TIM3,cmpval);
            break;
        default:
            return -1;
    }
    return 0;
}



/**********************************************************************
**********************  TIM4 Related Functions  ***********************
**********************************************************************/
unsigned short *key_val_buffer_pt = NULL;

void Timer4_capture_Init(u16 arr,u16 psc){
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    GPIO_InitTypeDef         GPIO_InitStructure;
    TIM_ICInitTypeDef        TIM_ICInitStructure;
    NVIC_InitTypeDef         NVIC_InitStructure;     
    
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //时钟使能
    TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值     计数到5000为500ms
    TIM_TimeBaseStructure.TIM_Prescaler =(psc-1); //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
    

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //时钟使能
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // Configures the TIM4 Channel 1 in Both edge capture Mode 
    TIM_ICInitStructure.TIM_Channel  = TIM_Channel_4;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1; 
    TIM_ICInitStructure.TIM_ICFilter = 0x03;    
    TIM_ICInit(TIM4, &TIM_ICInitStructure);
    
  /*********************???????************************/
    // http://www.openedv.com/thread-12644-1-1.html
//    TIM4->CCER &= (uint16_t)~((uint16_t)TIM_CCER_CC4E);
//    TIM4->CCMR1 |= 0x03;
//    TIM4->SMCR |= (1 << 6);
//    TIM4->SMCR &= ~((1 << 5) | (1 << 4));
//    TIM4->CCER |= ((uint16_t)TIM_CCER_CC4E);
   /*********************???????************************/  
   
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;   
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init(&NVIC_InitStructure);  

    #ifdef FreeRTOS
        key_val_buffer_pt = (unsigned short*)pvPortMalloc(MAXBUFLEN*sizeof(unsigned short));
    #else
        key_val_buffer_pt = (unsigned short*)malloc(MAXBUFLEN*sizeof(unsigned short));
    #endif
    memset(key_val_buffer_pt,0,MAXBUFLEN*sizeof(unsigned short));
    TIM_ClearFlag(TIM4, TIM_IT_CC4 | TIM_FLAG_Update); 
    TIM_ITConfig(TIM4,TIM_IT_CC4 |TIM_IT_Update,ENABLE);
    TIM_ARRPreloadConfig(TIM4, ENABLE); 
    TIM_Cmd(TIM4, ENABLE);  //使能TIMx外设
                 
}

static unsigned short cur_counter = 0;
unsigned char code_state = 0;     // 0 -- reset val   1 -- code_start   2-- code_finish
//unsigned short status[100] = {0}; int i= 0;
void TIM4_IRQHandler(void){
    if(TIM_GetITStatus(TIM4,TIM_IT_CC4) != RESET){
        cur_counter = TIM_GetCapture4(TIM4);
        TIM_ClearFlag(TIM4,TIM_IT_CC4);
        if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9))     TIM4->CCER|= TIM_CCER_CC4P;
        else                                            TIM4->CCER&= ~(TIM_CCER_CC4P);
        code_state = 1;
        *(key_val_buffer_pt+2+key_val_buffer_pt[1]) = cur_counter;
        key_val_buffer_pt[1]++;         
    }
    else{
        TIM_ClearFlag(TIM4,TIM_IT_Update);
        if(code_state == 1)        code_state = 2;
    }
}

/**********************************************************************
**********************  TIM2 Related Functions  ***********************
**********************************************************************/
void Timer2_counter_Init(u16 arr,u16 psc){
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef         NVIC_InitStructure;         
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能
    TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值     计数到5000为500ms
    TIM_TimeBaseStructure.TIM_Prescaler =(psc-1); //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
          
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=5;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;   
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init(&NVIC_InitStructure);  

    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM2, ENABLE);  //使能TIMx外设
                 
}

volatile unsigned char time_out = 0;
void TIM2_IRQHandler(void){
    if(TIM_GetITStatus(TIM2,TIM_IT_Update) != RESET){
        TIM_ClearFlag(TIM2,TIM_IT_Update);
        if(time_out < 2)    time_out++;
        else    TIM_Cmd(TIM2, DISABLE);
    }
}


void TIM2_delay_us(unsigned short delay_us){
    time_out = 0;
//    #ifdef FreeRTOS
//        UBaseType_t uxPriority;
//        uxPriority = uxTaskPriorityGet( NULL );
//        vTaskPrioritySet(NULL,configMAX_PRIORITIES-1);
//    #endif
    Timer2_counter_Init(delay_us-5,72);
    while(time_out != 2 );
//    #ifdef FreeRTOS
//        vTaskPrioritySet(NULL,uxPriority);
//    #endif
}





