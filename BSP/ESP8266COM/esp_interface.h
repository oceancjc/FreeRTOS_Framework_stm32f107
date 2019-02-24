#ifndef __ESP_INTERFACE_H_
#define __ESP_INTERFACE_H_

#include "usart.h"
#include "stm32f10x.h"



extern void espSend(const char *format,...);
extern int espQuery(uint8_t* response, int16_t timeout_ms, const char *format,...);
extern int init_Esp8266AsStation(void);
extern int espConnectAP(uint8_t*ssid, uint8_t* passwd);
extern int passThroughStart(uint8_t* remoteIP, uint16_t remotePORT, uint8_t TCP_nUDP);
extern void passThroughStop(void);



#endif



