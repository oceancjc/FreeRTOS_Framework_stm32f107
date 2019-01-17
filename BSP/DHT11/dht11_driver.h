#ifndef __DHT11_DRIVER_H__
#define __DHT11_DRIVER_H__

#include "stm32f10x.h"
#include "usart.h"
#ifdef FreeRTOS
    #include "FreeRTOS.h"
    #include "task.h"
    #include "queue.h"
    #include "timers.h"
    #include "semphr.h"
#endif	

#define DHT11_GPIO_TYPE  GPIOB
#define DHT11_GPIO_PIN   GPIO_Pin_10
#define DHT11_RCC        RCC_APB2Periph_GPIOB


#define DHT11_OUT_H GPIO_SetBits(DHT11_GPIO_TYPE, DHT11_GPIO_PIN)
#define DHT11_OUT_L GPIO_ResetBits(DHT11_GPIO_TYPE, DHT11_GPIO_PIN)
#define DHT11_IN    GPIO_ReadInputDataBit(DHT11_GPIO_TYPE, DHT11_GPIO_PIN)


#define ERR_CHECKSUM    (1)
#define ERR_SCAN        (2)



extern void dht11_gpio_input(void);
extern void dht11_gpio_output(void);
extern uint8_t dht11_scan(void);
extern uint8_t dht11_init(void);
extern uint8_t dht11_read_bit(void);
extern uint8_t dht11_read_byte(void);
extern uint16_t dht11_read_data(uint8_t buffer[5]);

#endif
