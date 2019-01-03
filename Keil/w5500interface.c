#include "socket.h"
#include "dhcp.h"
#include "stdio.h"
#include "usart.h"

#define DATA_BUF_SIZE   2048
/* Private macro -------------------------------------------------------------*/
uint8_t gDATABUF[DATA_BUF_SIZE];
// Default Network Configuration
wiz_NetInfo gWIZNETINFO = { .mac = {0x00, 0x08, 0xdc,0x00, 0xab, 0xcd},
                            .ip = {192, 168, 0, 123},
                            .sn = {255,255,255,0},
                            .gw = {192, 168, 0, 1},
                            .dns = {0,0,0,0},
                            .dhcp = NETINFO_STATIC };

                            
                            
int w5500ParametersConfiguration(void){
    uint8_t tmp = 0,times_of_retry = 255;
    uint8_t memsize[2][8] = {{2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};
    if(ctlwizchip(CW_INIT_WIZCHIP,(void*)memsize) == -1){
//            uart1_printf("WIZCHIP Initialized fail.\r\n");
            return -1;
    }

    do{
        if(ctlwizchip(CW_GET_PHYLINK, (void*)&tmp) == -1){
//            uart1_printf("Unknown PHY Link stauts.\r\n");
            return -2;
        }
    }while(tmp == PHY_LINK_OFF && times_of_retry--);
    return 0;
}
                          


void w5500NetworkConfig(void){
    uint8_t tmpstr[6] = { 0 };
	ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);
	ctlnetwork(CN_GET_NETINFO, (void*)&gWIZNETINFO);

	// Display Network Information
	ctlwizchip(CW_GET_ID,(void*)tmpstr);
	uart1_printf("\r\n=== %s NET CONF ===\r\n",(char*)tmpstr);
	uart1_printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",gWIZNETINFO.mac[0],gWIZNETINFO.mac[1],gWIZNETINFO.mac[2],
		  gWIZNETINFO.mac[3],gWIZNETINFO.mac[4],gWIZNETINFO.mac[5]);
	uart1_printf("SIP: %d.%d.%d.%d\r\n", gWIZNETINFO.ip[0],gWIZNETINFO.ip[1],gWIZNETINFO.ip[2],gWIZNETINFO.ip[3]);
	uart1_printf("GAR: %d.%d.%d.%d\r\n", gWIZNETINFO.gw[0],gWIZNETINFO.gw[1],gWIZNETINFO.gw[2],gWIZNETINFO.gw[3]);
	uart1_printf("SUB: %d.%d.%d.%d\r\n", gWIZNETINFO.sn[0],gWIZNETINFO.sn[1],gWIZNETINFO.sn[2],gWIZNETINFO.sn[3]);
	uart1_printf("DNS: %d.%d.%d.%d\r\n", gWIZNETINFO.dns[0],gWIZNETINFO.dns[1],gWIZNETINFO.dns[2],gWIZNETINFO.dns[3]);
	uart1_printf("======================\r\n");
}





void w5500Reset(void){
    wizchip_sw_reset();
}




void my_ip_assign(void){
    getIPfromDHCP(gWIZNETINFO.ip);
    getGWfromDHCP(gWIZNETINFO.gw);
    getSNfromDHCP(gWIZNETINFO.sn);
    getDNSfromDHCP(gWIZNETINFO.dns);
    gWIZNETINFO.dhcp = NETINFO_DHCP;
    ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);     
    uart1_printf("DHCP LEASED TIME : %d Sec.\r\n", getDHCPLeasetime());
}

void my_ip_conflict(void){
    uart1_printf("CONFLICT IP from DHCP\r\n");
    while(1); 
}



void DHCPConfig(void){
    setSHAR(gWIZNETINFO.mac);    // must be set the default mac before DHCP started, ??MAC
    DHCP_init(1,gDATABUF);
    reg_dhcp_cbfunc(my_ip_assign, my_ip_assign, my_ip_conflict);
    uint8_t dhcp_ret = DHCP_run();//???DHCP_run(),?????????IP???
    while(dhcp_ret != DHCP_IP_LEASED){//??DHCP_IP_LEASED??????IP????,?????,????
//        HAL_Delay(1000);
        dhcp_ret = DHCP_run();
    }
    w5500NetworkConfig();

}



int w5500Init(uint8_t isDHCPenabled){
	int ret = 0;
	// First of all, Should register SPI callback functions implemented by user for accessing WIZCHIP 
	/* Critical section callback */
	reg_wizchip_cris_cbfunc(NULL, NULL);	//Use default ones
	/* Chip selection call back */
#if   _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_VDM_
	reg_wizchip_cs_cbfunc(NULL, NULL);      //Use default ones
#elif _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_FDM_
	reg_wizchip_cs_cbfunc(SPI_CS_Select, SPI_CS_Deselect);  // CS must be tried with LOW.
#else
   #if (_WIZCHIP_IO_MODE_ & _WIZCHIP_IO_MODE_SIP_) != _WIZCHIP_IO_MODE_SIP_
      #error "Unknown _WIZCHIP_IO_MODE_"
   #else
      reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
   #endif
#endif
	/* SPI Read & Write callback function */
	reg_wizchip_spi_cbfunc(NULL, NULL);	   //Use default ones

	/* WIZCHIP SOCKET Buffer initialize */
    ret = w5500ParametersConfiguration();
    if(ret)    return ret;
	/* Static Network initialization */
	if(!isDHCPenabled)    w5500NetworkConfig();
    /* Dynamic Network initialization */
    else                  DHCPConfig();
    
    return 0;
	/* Main loop */
//	while(1)
//	{
//		/* Loopback Test */
//		// TCP server loopback test
//		if( (ret = loopback_tcps(SOCK_TCPS, gDATABUF, 5000)) < 0) {
//			//uart1_printf("SOCKET ERROR : %ld\r\n", ret);
//		}
//    // UDP server loopback test
//		if( (ret = loopback_udps(SOCK_UDPS, gDATABUF, 3000)) < 0) {
//			//uart1_printf("SOCKET ERROR : %ld\r\n", ret);
//		}
//	} // end of Main loop
}
