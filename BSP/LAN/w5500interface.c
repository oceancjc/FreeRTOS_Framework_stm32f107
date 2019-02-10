#include "w5500interface.h"


/* Private macro -------------------------------------------------------------*/
uint8_t gDATABUF[DATA_BUF_SIZE];
// Default Network Configuration
wiz_NetInfo gWIZNETINFO = { .mac = {0x00, 0x08, 0xdc,0x00, 0xab, 0xcd},
                            .ip = {192, 168, 1, 123},
                            .sn = {255,255,255,0},
                            .gw = {192, 168, 1, 1},
                            .dns = {180,76,76,76},
                            .dhcp = NETINFO_STATIC };    //NETINFO_DHCP

                            
void w5500delay_ms(uint16_t ms){
    /*Insert delay code Here*/
    #ifdef FreeRTOS
        vTaskDelay(pdMS_TO_TICKS(ms));
    #else

    #endif    
}
            


void w5500InitIO(void){
    /* Init SPI Configuration */
    Initial_SPI(W5500SPICHANNEL,0);
    GPIO_InitTypeDef GPIO_InitStruct = { 0 };
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    /* Init RST PIN and set init val = 1 */    
    GPIO_InitStruct.GPIO_Pin = W5500RST_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(W5500RST_PORT, &GPIO_InitStruct);
    GPIO_SetBits(W5500RST_PORT,W5500RST_PIN);
    /*Init CS PIN and set init val = 1*/
    GPIO_InitStruct.GPIO_Pin = W5500CS_PIN;
    GPIO_Init(W5500CS_PORT, &GPIO_InitStruct);
    GPIO_SetBits(W5500CS_PORT,W5500CS_PIN); 
}


int w5500ParametersConfiguration(void){
    uint8_t tmp = 0;
    uint8_t memsize[2][8] = {{2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};
    if(ctlwizchip(CW_INIT_WIZCHIP,(void*)memsize) == -1){
            uart1_printf("WIZCHIP Initialized fail.\r\n");
            return -1;
    }
    do{
        if(ctlwizchip(CW_GET_PHYLINK, (void*)&tmp) == -1){
            uart1_printf("Unknown PHY Link stauts.\r\n");
            return -2;
        }
        if(tmp == PHY_LINK_OFF)    w5500delay_ms(10);
        else                       break;
    }while( 1 );
    return 0;
}
            


int w5500CheckLinkState(void){
    uint8_t tmp = 0;
    if(ctlwizchip(CW_GET_PHYLINK, (void*)&tmp) == -1){
        uart1_printf("Unknown PHY Link stauts.\r\n");
        return -2;
    }
    return tmp;
}


int w5500NetworkConfig(void){
    uint8_t tmpstr[6] = { 0 };
    wiz_NetInfo NETINFO_retrieve = { 0 };
    ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);
    ctlnetwork(CN_GET_NETINFO, (void*)&NETINFO_retrieve);
    if(NETINFO_retrieve.ip[3]==gWIZNETINFO.ip[3] && NETINFO_retrieve.ip[1]==gWIZNETINFO.ip[1]){
        // Display Network Information
        ctlwizchip(CW_GET_ID,(void*)tmpstr);
        uart1_printf("\r\n=== %s NET CONF ===\r\n",(char*)tmpstr);
        uart1_printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",NETINFO_retrieve.mac[0],NETINFO_retrieve.mac[1],NETINFO_retrieve.mac[2],
              NETINFO_retrieve.mac[3],NETINFO_retrieve.mac[4],NETINFO_retrieve.mac[5]);
        uart1_printf("SIP: %d.%d.%d.%d\r\n", NETINFO_retrieve.ip[0], NETINFO_retrieve.ip[1], NETINFO_retrieve.ip[2], NETINFO_retrieve.ip[3]);
        uart1_printf("GAR: %d.%d.%d.%d\r\n", NETINFO_retrieve.gw[0], NETINFO_retrieve.gw[1], NETINFO_retrieve.gw[2], NETINFO_retrieve.gw[3]);
        uart1_printf("SUB: %d.%d.%d.%d\r\n", NETINFO_retrieve.sn[0], NETINFO_retrieve.sn[1], NETINFO_retrieve.sn[2], NETINFO_retrieve.sn[3]);
        uart1_printf("DNS: %d.%d.%d.%d\r\n", NETINFO_retrieve.dns[0],NETINFO_retrieve.dns[1],NETINFO_retrieve.dns[2],NETINFO_retrieve.dns[3]);
        uart1_printf("======================\r\n");    
        return 0;
    }
    uart1_printf("Static ip configuration fail\r\n");  
    return -1;
   
}





void w5500Reset(void){
    GPIO_ResetBits(W5500RST_PORT,W5500RST_PIN);
    w5500delay_ms(2);
    GPIO_SetBits(W5500RST_PORT,W5500RST_PIN);  
    w5500delay_ms(3);    
//    wizchip_sw_reset();
//    w5500delay_ms(50);
}




void my_ip_assign(void){
    getIPfromDHCP(gWIZNETINFO.ip);
    getGWfromDHCP(gWIZNETINFO.gw);
    getSNfromDHCP(gWIZNETINFO.sn);
    getDNSfromDHCP(gWIZNETINFO.dns);
    gWIZNETINFO.dhcp = NETINFO_DHCP;
//    ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);   
    int ret = w5500NetworkConfig();    
    if(!ret)    uart1_printf("DHCP LEASED TIME : %d Sec.\r\n", getDHCPLeasetime());
}

void my_ip_conflict(void){
    uart1_printf("CONFLICT IP from DHCP\r\n");
    while(1); 
}



int DHCPConfig(void){
    setSHAR(gWIZNETINFO.mac);    // must be set the default mac before DHCP started, ??MAC
    DHCP_init(SOCKET_DHCP,gDATABUF);
    reg_dhcp_cbfunc(my_ip_assign, my_ip_assign, my_ip_conflict);
    uint8_t dhcp_ret = DHCP_run();  //Try to run DHCP process
    uint8_t retry_times = MAX_DHCP_RETRY;
    /*Try run DHCP process until DHCP_IP_LEASED or time out*/
    while(retry_times--){
        if(dhcp_ret == DHCP_IP_LEASED)   return 0;
        uart1_printf("DHCP retry time %d: %d\r\n",dhcp_ret,retry_times);
        w5500delay_ms(500u);
        dhcp_ret = DHCP_run();
    }
    if( dhcp_ret != DHCP_IP_LEASED ){
        uart1_printf("DHCP Fail\r\n");  
        return -1;
    }
    return 0;
}



int DNSRun(uint8_t *name, uint8_t* ip){
    int ret = 0,i=0;
    uint8_t dnsserver[2][4] = { {223,6,6,6}, {114,114,114,114} };
    /* DNS client initialization */
    uart1_printf("> DNS 1st : %d.%d.%d.%d\r\n", gWIZNETINFO.dns[0], gWIZNETINFO.dns[1], gWIZNETINFO.dns[2], gWIZNETINFO.dns[3]);
    DNS_init(SOCKET_DNS, gDATABUF);
    /* DNS procssing */
    if ((ret = DNS_run(gWIZNETINFO.dns, name, ip)) > 0)    uart1_printf("> DNS 1 Reponsed\r\n");
    else if(ret == -1){    
        uart1_printf("> MAX_DOMAIN_NAME is too small. Should be redefined it.\r\n");
        return ret;
    }
    else                  uart1_printf("> DNS 1 Failed\r\n");

    if(ret > 0)     uart1_printf("> Translated %s to %d.%d.%d.%d\r\n",name,ip[0],ip[1],ip[2],ip[3]);
    else{
        for(i=0;i<2;i++){
            if ((ret = DNS_run(dnsserver[i], name, ip)) > 0){
                if(ip[0] == 0 && ip[1] == 0)      continue;
                uart1_printf("> DNS %d Reponsed\r\n",i+2);
                uart1_printf("> Translated %s to %d.%d.%d.%d\r\n",name,ip[0],ip[1],ip[2],ip[3]);
                return ret;
            }
            else{
                uart1_printf("> DNS %d Failed\r\n",i+2);
            }
        } 
        uart1_printf("> All DNS Failed\r\n");        
    }
    return ret;
    
}

void w5500Init(void){
    w5500InitIO();
    w5500Reset();
    // First of all, Should register SPI callback functions implemented by user for accessing WIZCHIP 
    /* Critical section callback */
    reg_wizchip_cris_cbfunc(NULL, NULL);    //Use default ones
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
    reg_wizchip_spi_cbfunc(NULL, NULL);       //Use default ones
}



int w5500SetIp(uint8_t isDHCPenabled){
    int ret = 0,retry = 3;
    while(retry--){
        ret = w5500ParametersConfiguration();
        if(ret)    return ret;
        /* Dynamic Network initialization */
        if(isDHCPenabled){  
            gWIZNETINFO.dhcp = NETINFO_DHCP;
            if( 0 == DHCPConfig() )    return 0;
        }    
        /* Static Network initialization */
        else  return w5500NetworkConfig();            
    }
    return w5500NetworkConfig(); 

}



/**
  * @brief  Loopback Test Example Code using ioLibrary_BSD    
  * @retval None
  */
int loopback_tcps(uint8_t sn, uint8_t* buf, uint16_t port){
    int32_t ret = 0;
    uint16_t size = 0, sentsize=0;
    switch(getSn_SR(sn)) {
        case SOCK_ESTABLISHED :
            if(getSn_IR(sn) & Sn_IR_CON){
                uart1_printf("%d:Connected\r\n",sn);
                setSn_IR(sn,Sn_IR_CON);
            }
            if((size = getSn_RX_RSR(sn)) > 0){
                if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE;
                ret = recv(sn,buf,size);
                if(ret <= 0)     return ret;
                sentsize = 0;
                while(size != sentsize){
                    ret = send(sn,buf+sentsize,size-sentsize);
                    if(ret < 0){
                        close(sn);
                        return ret;
                    }
                    sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
                }
            }
            break;
        case SOCK_CLOSE_WAIT :
            uart1_printf("%d:CloseWait\r\n",sn);
            if((ret=disconnect(sn)) != SOCK_OK) return ret;
            uart1_printf("%d:Closed\r\n",sn);
            break;
        case SOCK_INIT :
             uart1_printf("%d:Listen, port [%d]\r\n",sn, port);
            if( (ret = listen(sn)) != SOCK_OK) return ret;
            break;
        case SOCK_CLOSED:
            uart1_printf("%d:LBTStart\r\n",sn);
            if((ret=socket(sn,Sn_MR_TCP,port,0x00)) != sn)    return ret;
            uart1_printf("%d:Opened\r\n",sn);
            /* Set Keep Alive Frame Function, interval = 6s */
            setSn_KPALVTR(sn,6); 
            break;
        default:
            break;
    }
    return 1;
}



int loopback_tcpc(uint8_t sn, uint8_t* ip, uint16_t port){
    uint32_t len = 0;    int ret = 0;
    switch(getSn_SR(sn)){
        case SOCK_INIT:                                                     
            if((ret = connect(sn,ip,port)) != SOCK_OK){
                disconnect(sn);
                uart1_printf("Connect TCP server fail,ret = %d\r\n",ret);
            }
            break;
        case SOCK_ESTABLISHED:                                         
            if(getSn_IR(sn) & Sn_IR_CON)     setSn_IR(sn, Sn_IR_CON);                  
            len=getSn_RX_RSR(sn);                                
            if(len > DATA_BUF_SIZE)    len = DATA_BUF_SIZE;
            if(len){
                ret = recv(sn,gDATABUF,len);   
                if(ret<0)    return ret;            
                uart1_printf("%s\r\n",gDATABUF);
                send(sn,gDATABUF,len); 
            }                                                    
            break;
        case SOCK_CLOSE_WAIT:                                     
            disconnect(sn);    
            break;
        case SOCK_CLOSED:                 
            if((ret=socket(sn,Sn_MR_TCP,5000,0)) != SOCK_TCPS)    return ret;  //???Why is 5000 rather than port? 
            uart1_printf("%d:Opened\r\n",sn);        
            break;
        default:
            break;
    }
    return 0;
}


/* Baidu Cloud Iot Hub Part */
enum{
    MQTT_IDLE = 0,
    MQTT_CONNECT,
    MQTT_SUBSCRIBE,
    MQTT_PUBLISH,
    MQTT_PINGREQ,
    ERR_MQTT_CONNECT_FAIL = -1,
};

static volatile uint8_t mqtt_outside = MQTT_IDLE;
void setMqttState(uint8_t state){
    mqtt_outside = state;
}


int jasonFramer(char* frame, uint8_t lightState, uint8_t temp, uint8_t humidity){
    cJSON *root = cJSON_CreateObject();
    cJSON *report = cJSON_CreateObject();
    char* s = NULL;
    if( root == NULL || frame == NULL )   return -1;
    if(report==NULL)  return -2;
    cJSON_AddItemToObject(root, "reported", report );
    cJSON_AddBoolToObject(report, "light_state", lightState==0);
    cJSON_AddNumberToObject(report, "tempature", temp);
    cJSON_AddNumberToObject(report, "humidity", humidity);
    s = cJSON_Print(root);
    strcpy(frame,s);
    cJSON_Delete(root);
    vPortFree(s);
    return 0;
}


int mqttStateMachine(void){
    static uint8_t state = MQTT_CONNECT,retry = 3;
    static uint32_t update_counter = 0;
    int ret = 0;
    switch(state){
        case MQTT_IDLE:
            if(mqtt_outside == MQTT_IDLE){
                /* If Idle for a long time, send heart beat */
                if(update_counter > 100){
                    state = MQTT_PUBLISH;    update_counter = 0;
                }    
                else{
                    update_counter++;
                    w5500delay_ms(1000);
                }      
            }    
            else{
                state = mqtt_outside;
                mqtt_outside = MQTT_IDLE;
            }
            break;
        case MQTT_CONNECT:
            while(retry--){
                ret = mqtt_remoteConnect(DEVICENAME,120,1,USERNAME,PASSWD);
                if(ret){
                    uart1_printf("Connect Server Fail with ret = %d\r\n",ret);
                    w5500delay_ms(100);
                }
                else    break;
            }
            retry = 3;
            if(ret)     return ERR_MQTT_CONNECT_FAIL;
            uart1_printf("Connect Server Success\r\n");
            state = MQTT_PUBLISH;
            break;
        case MQTT_PINGREQ:
            mqtt_ping((uint8_t*)"hello");

        case MQTT_PUBLISH:
            ret = jasonFramer((char*)gMQTTFrame,1,gDht11Data[2], gDht11Data[0]);
            if(ret)    uart1_printf("Jason Malloc Fail, ret=%d\r\n",ret);
            if(0 > mqtt_publish("$baidu/iot/shadow/State_MainBedroom/update",(char*)gMQTTFrame)){
                uart1_printf("Sth wrong with MQTT_Publish\r\n"); 
                return -1;
            }     
            uart1_printf("%s\n",gMQTTFrame);
            gMQTTFrame[0] = 0;
            state = MQTT_IDLE;
            mqtt_outside = MQTT_IDLE;
//            w5500delay_ms(2000);
            break;
        case MQTT_SUBSCRIBE:
            state = MQTT_IDLE;
            mqtt_outside = MQTT_IDLE;
            break;
        default:
            break;             
    }
    return 0;
}
    


int baiduMqttPublishtest(uint8_t* buf){
    int ret = 0,retry = 10;
    switch(getSn_SR(SOCK_MQTT)){
        case SOCK_INIT:
            if((ret = connect(SOCK_MQTT,gMQTTServerIP,gMQTTServerPort)) != SOCK_OK){
                disconnect(SOCK_MQTT);
                uart1_printf("Connect TCP server fail,ret = %d\r\n",ret);
            }
            break;
        case SOCK_ESTABLISHED: 
//            uart1_printf("%d:Established\r\n",SOCK_MQTT); 
            if(getSn_IR(SOCK_MQTT) & Sn_IR_CON)     setSn_IR(SOCK_MQTT, Sn_IR_CON);
            ret = mqttStateMachine();
            if(ret)    disconnect(SOCK_MQTT);      
            break;
        case SOCK_CLOSE_WAIT:                                     
            disconnect(SOCK_MQTT);    
            break;
        case SOCK_CLOSED:                 
            if((ret=socket(SOCK_MQTT,Sn_MR_TCP,5001,0)) != SOCK_MQTT)    return ret;  //???Why is 5000 rather than port? 
            uart1_printf("%d:Opened\r\n",SOCK_MQTT);        
            setMqttState(MQTT_CONNECT);
            break;
        default:        break;
    }
    return 0;
}





