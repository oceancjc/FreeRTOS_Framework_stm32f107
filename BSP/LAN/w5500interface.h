#ifndef __W5500_INTERFACE__
#define __W5500_INTERFACE__

#include "socket.h"
#include "dhcp.h"
#include "dns.h"
#include "stdio.h"
#include "usart.h"
#include "spi.h"
#include "transport.h"

#ifdef FreeRTOS
    #include "FreeRTOS.h"
    #include "task.h"
    #include "queue.h"
    #include "timers.h"
    #include "semphr.h"
#endif	

#define W5500SPICHANNEL                 (1)
#define W5500RST_PORT                  GPIOA
#define W5500CS_PORT                   GPIOA
#define W5500RST_PIN                 GPIO_Pin_3
#define W5500CS_PIN                  GPIO_Pin_4

#define DATA_BUF_SIZE   2048

#define SOCKET_DHCP      0
#define SOCKET_DNS       0

#define SOCK_TCPS        0
#define SOCK_UDPS        2
#define SOCK_MQTT        3

extern uint8_t gDATABUF[DATA_BUF_SIZE];


extern void w5500InitIO(void);
extern void w5500Reset(void);
extern void w5500Init(void);
extern int w5500SetIp(uint8_t isDHCPenabled);
extern int w5500CheckLinkState(void);
extern int DNSRun(uint8_t *name, uint8_t* ip);
extern int loopback_tcps(uint8_t sn, uint8_t* buf, uint16_t port);
extern int loopback_tcpc(uint8_t sn, uint8_t* ip, uint16_t port);
extern int baiduMqttPublishtest(uint8_t* buf);



#endif
