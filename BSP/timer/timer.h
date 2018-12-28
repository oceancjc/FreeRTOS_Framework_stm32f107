#ifndef __TIMER_H
#define __TIMER_H
#include "stm32f10x.h"

//////////////////////////////////////////////////////////////////////////////////     
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK Mini STM32������
//ͨ�ö�ʱ�� ��������               
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/12/03
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
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
