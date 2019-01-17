#include "dht11_driver.h"


void dhtdelay_us(uint16_t us){
    do{
		__NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
		__NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
		__NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
		__NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
		__NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
		__NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
		__NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
		__NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); 
	}while(--us);	
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
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = DHT11_GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(DHT11_GPIO_TYPE, &GPIO_InitStructure);
}

void dht11_gpio_output(void){
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(DHT11_RCC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = DHT11_GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_Init(DHT11_GPIO_TYPE, &GPIO_InitStructure);
}

void dht11_reset(void){
    dht11_gpio_output();
    DHT11_OUT_L;
    dhtdelay_ms(20);
    DHT11_OUT_H;
    dhtdelay_us(30);
}



uint8_t dht11_check(void){
    uint8_t retry = 128;
    dht11_gpio_input();
    while(DHT11_IN && --retry)     dhtdelay_us(1);
    if(!retry)    return 1;
    retry = 128;
    while(!DHT11_IN && --retry)    dhtdelay_us(1);  
    if(!retry)    return 1;
    else          return 0;
}



uint8_t dht11_read_bit(void){
    uint8_t retry = 128;
    while( DHT11_IN && retry-- )    dhtdelay_us(1);
    retry = 88;
    while( !DHT11_IN && retry--)    dhtdelay_us(1);
    dhtdelay_us(38);
    if (DHT11_IN == 1)        return 1;
    else                      return 0;
}



uint8_t dht11_read_byte(void){
    uint8_t i = 0, data = 0;
    for (i = 0; i < 8; i++){
        data <<= 1;
        data |= dht11_read_bit();
    }
    return data;
}



uint8_t dht11_init(void){
    dht11_reset();
    return dht11_check();
}



uint16_t dht11_read_data(uint8_t buffer[5]){
    uint8_t i = 0;
    dht11_reset();
    if(dht11_check() == 0){
        for (i = 0; i < 5; i++)    buffer[i] = dht11_read_byte();
        if ( (buffer[0] + buffer[1] + buffer[2] + buffer[3]) != buffer[4])    return ERR_CHECKSUM;
    }
    else    return ERR_SCAN;
    return 0;
}
