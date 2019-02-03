#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>
#include <stdio.h>

extern uint8_t gDht11Data[5];





/**********Internet Related ************/
extern uint8_t gMQTTFrame[200];
extern uint8_t gMQTTServerIP[4];
extern uint16_t gMQTTServerPort;
extern uint32_t gMQTTPubInterval_s;



#endif

