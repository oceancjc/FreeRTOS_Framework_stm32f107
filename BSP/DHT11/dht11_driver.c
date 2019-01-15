#include "dht11_driver.h"


void dhtdelay_us(uint16_t us){
    uint32_t delay = (((uint32_t)us)<<3) + (((uint32_t)us)<<6) - 5;   //us*72 - 5
    do{
		__NOP();
	}while(delay--);	
}



void dhtdelay_ms(uint16_t ms){
    /*Insert delay code Here*/
    #ifdef FreeRTOS
        vTaskDelay(pdMS_TO_TICKS(ms));
    #else
        do{
			dhtdelay_us(1000);
		}while(--ms);
    #endif   
	
}


void dht11_gpio_input(void){
    GPIO_InitTypeDef g;
    g.GPIO_Pin = DHT11_GPIO_PIN;
    g.GPIO_Speed = GPIO_Speed_50MHz;
    g.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(DHT11_GPIO_TYPE, &g);
}

void dht11_gpio_output(void){
    GPIO_InitTypeDef g;
    RCC_APB2PeriphClockCmd(DHT11_RCC, ENABLE);
    g.GPIO_Pin = DHT11_GPIO_PIN;
    g.GPIO_Speed = GPIO_Speed_50MHz;
    g.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_Init(DHT11_GPIO_TYPE, &g);
}

void dht11_reset(void){
    dht11_gpio_output();
    DHT11_OUT_L;
    dhtdelay_ms(18);
    DHT11_OUT_H;
    dhtdelay_us(30);
    dht11_gpio_input();
}

inline uint8_t dht11_scan(void){
    return (DHT11_IN);
}

uint8_t dht11_read_bit(void)
{
    while (DHT11_IN == 0);
    dhtdelay_us(38);
    if (DHT11_IN == 1){
        while (DHT11_IN == 1);
        return 1;
    }
    else    return 0;
}

uint8_t dht11_read_byte(void){
    uint16_t i = 0, data = 0;
    for (i = 0; i < 8; i++){
        data <<= 1;
        data |= dht11_read_bit();
    }
    return data;
}

uint16_t dht11_read_data(uint8_t buffer[5]){
    uint8_t i = 0;
    dht11_reset();
    if (dht11_scan() == 0){
        /* check for dht11 response */
        while (dht11_scan() == 0);
        while (dht11_scan() == 1);
        for (i = 0; i < 5; i++)    buffer[i] = dht11_read_byte();
        
        while (dht11_scan() == RESET);
        dht11_gpio_output();
        DHT11_OUT_H;
        
        uint8_t checksum = buffer[0] + buffer[1] + buffer[2] + buffer[3];
        if (checksum != buffer[4])    return ERR_CHECKSUM;
    }
    else    return ERR_SCAN;
    return 0;
}
