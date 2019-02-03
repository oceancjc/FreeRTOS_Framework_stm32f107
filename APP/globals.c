#include "globals.h"

uint8_t gDht11Data[5] = { 0 }; // wetI, wetF, tempI, tempF, checksum





/**********Internet Related ************/
uint8_t gMQTTFrame[200] = { 0 };
uint8_t gMQTTServerIP[4] = { 183,240,93,17 };
uint16_t gMQTTServerPort = 1883;
uint32_t gMQTTPubInterval_s = 300;
