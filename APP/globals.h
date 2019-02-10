#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>
#include <stdio.h>

extern uint8_t gDht11Data[5];





/********** MQTT Related ************/
#define DEVICENAME        "State_MainBedroom_stm32"                   //ONENET Device  ID
#define USERNAME          "h56qpk7/State_MainBedroom"           //ONENET Product ID
#define PASSWD            "xky4WEI4dbwip4Dn"                    //ONENET JianQuan Info
extern uint8_t gMQTTFrame[200];
extern uint8_t gMQTTServerIP[4];
extern uint16_t gMQTTServerPort;
extern uint32_t gMQTTPubInterval_s;



#endif

