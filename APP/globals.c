#include "globals.h"

/********* Sensor Related ***************/
uint8_t gDht11Data[5]         __attribute__((section(".ARM.__at_0x2000001c"))) = { 0 };    // wetI, wetF, tempI, tempF, checksum





/**********Internet Related ************/
uint8_t  gMQTTFrame[200]      __attribute__((section(".ARM.__at_0x200002b0"))) = { 0 };
uint8_t  gMQTTServerIP[4]     __attribute__((section(".ARM.__at_0x20000021"))) = { 183,240,93,17 };
uint16_t gMQTTServerPort      __attribute__((section(".ARM.__at_0x20000026"))) = 1883;
uint32_t gMQTTPubInterval_s   __attribute__((section(".ARM.__at_0x20000000"))) = 300;
