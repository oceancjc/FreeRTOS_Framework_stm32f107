#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>
#include <stdio.h>

extern uint8_t gDht11Data[5];





/********** MQTT Related ************/
#define DEVICENAME                "State_MainBedroom_stm32"             //ONENET Device  ID
#define KEEPALIVE_s               (120)
#define USERNAME                  "h56qpk7/State_MainBedroom"           //ONENET Product ID
#define PASSWD                    "xky4WEI4dbwip4Dn"                    //ONENET JianQuan Info
#define MQTT_PUBLISH_CYCLE_s      (300)
#define MQTT_DELAY_INTERVAL_ms    (1000)
enum{
    MQTT_IDLE = 0,
    MQTT_CONNECT,
    MQTT_SUBSCRIBE,
    MQTT_PUBLISH,
    MQTT_PINGREQ,
    ERR_MQTT_CONNECT_FAIL = -1,
};

extern uint8_t gMQTTFrame[200];
extern uint8_t gMQTTServerIP[4];
extern uint16_t gMQTTServerPort;
extern uint32_t gMQTTPubInterval_s;



#endif

