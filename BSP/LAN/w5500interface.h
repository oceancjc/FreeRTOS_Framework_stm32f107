#ifndef __W5500_INTERFACE__
#define __W5500_INTERFACE__

#include "socket.h"
#include "dhcp.h"
#include "dns.h"
#include "stdio.h"
#include "usart.h"


#ifdef FreeRTOS
    #include "FreeRTOS.h"
    #include "task.h"
    #include "queue.h"
    #include "timers.h"
    #include "semphr.h"
#endif	



#define DATA_BUF_SIZE   2048

#define SOCKET_DHCP      0
#define SOCKET_DNS       1

#define SOCK_TCPS        0
#define SOCK_UDPS        1


extern uint8_t gDATABUF[DATA_BUF_SIZE];



extern void w5500Reset(void);
extern int w5500Init(uint8_t isDHCPenabled);
extern int loopback_tcps(uint8_t sn, uint8_t* buf, uint16_t port);
extern int loopback_tcpc(uint8_t* ip, uint16_t port);





#endif
