#ifndef __TIMER_H
#define __TIMER_H
#include "stm32f10x.h"

//////////////////////////////////////////////////////////////////////////////////     
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK Mini STM32开发板
//通用定时器 驱动代码               
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/12/03
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////       

#define MAXBUFLEN   500
#define MAXCOUNTER  60000


extern u8 Flag_108ms[4];
extern unsigned char code_state;
extern unsigned short* key_val_buffer_pt;




void Timerx_Init(u16 arr,u16 psc); 
void Timer2_Init(u16 arr,u16 psc); 
void re_count_108ms(void);
void Timer4_capture_Init(u16 arr,u16 psc);
void Timer4_pwm_off(char channel);
void Timer4_pwm_on(char channel);
void Timer3_pwm_Init(u16 arr,u16 psc);
void Timer3_pwm_off(char channel);
void Timer3_pwm_on(char channel);
void Timer2_counter_Init(u16 arr,u16 psc);
void TIM2_delay_us(unsigned short delay_us);

#endif
