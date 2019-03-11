#include "esp_interface.h"


void espDelay_ms(uint32_t delay_ms){
    #ifdef FreeRTOS
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    #else
        return;
    #endif    
}


uint16_t SentWaitRcv = 0;
void espSend(const char *format,...){
    va_list ap = { 0 };
    char s[128] = { 0 };
    va_start(ap, format);
    int len = vsprintf ((char*)s, format, ap);   
    va_end(ap); 
    if(s[0] == 'A' && s[1] == 'T'){
        Uart2SendStr(strcat(s,"\r\n"));
        SentWaitRcv = 0;    
    }

}

/********************************************************************
Receive uart msg from uart idle interrupt, the IRQ will clear flag 
SentWaitRcv when idle interrupt triggerred
Return :
    0  OK
    -1 ERROR from esp msg
    -2 busy from esp msg
    -3 timeout from user control
    -4 unknown response, maybe unfinished.
********************************************************************/
int espRcv(char* msg, int16_t timeout_ms){
    char* keyword_position = NULL;
    while(!SentWaitRcv && timeout_ms > 0) {
        espDelay_ms(5);    timeout_ms -= 5;
        if(timeout_ms <= 0)    return -3;
    }   
    if((keyword_position = strstr(msg,"OK"))!=NULL){
        *keyword_position = '\0';
        return 0;
    }
    else if((keyword_position = strstr(msg,"ERROR"))!=NULL){
        *keyword_position = '\0';
        return -1;
    }
    else if((keyword_position = strstr(msg,"busy"))!=NULL){
        return -2;
    }
    else    return -4;
}



int espQuery(uint8_t* response, int16_t timeout_ms, const char *format,...){
    va_list ap = { 0 };
    char s[128] = { 0 };
    va_start(ap, format);
    int len = vsprintf ((char*)s, format, ap);   
    va_end(ap); 
    Uart2SendStr(strcat(s,"\r\n"));
    *response = '\0';
    SentWaitRcv = 0;
    return  espRcv((char*)response, timeout_ms);
}



int espParse(uint8_t* msg, char* target, int16_t timeout_ms){
    char* keyword_position = NULL;
    while(!SentWaitRcv && timeout_ms > 0) {
        espDelay_ms(5);    timeout_ms -= 5;
        if(timeout_ms <= 0)    return -3;
    }   
    if((keyword_position = strstr((char*)msg,target))!=NULL){
        *keyword_position = '\0';
        return 0;
    }
    return -1;
}



int init_Esp8266AsStation(void){
    int ret = 0;
    ret = espQuery(Msgget2,500,"AT");    //AT function test: Should get OK
    ret |= espQuery(Msgget2,500,"ATE0"); //Close display
    ret |= espQuery(Msgget2,500,"AT+CWMODE=1"); //Station only
    ret |= espQuery(Msgget2,5000,"AT+CWDHCP=2,1"); //Enable DHCP
    if(ret)    return ret;
    espSend("AT+RST");   
    return 0;
}



int espConnectAP(uint8_t*ssid, uint8_t* passwd){
    int ret = espQuery(Msgget2,5000,"AT+CWJAP_CUR =%s,%s",ssid,passwd);
    ret = espQuery(Msgget2,1000,"AT+CIFSR");    //Check IP Address
    ret = espQuery(Msgget2,500,"AT+CIPMUX=1");  //Enable multi-connect
    return ret;
}



int passThroughStart(uint8_t* remoteIP, uint16_t remotePORT, uint8_t TCP_nUDP){
    int ret = espQuery(Msgget2,500,"AT+CIPMUX=0");  //Enable single-connect
    ret = espQuery(Msgget2,20,"AT+CIPMODE=1");
    if(TCP_nUDP)    espSend("AT+CIPSTART=\"TCP\",\"%s\",%d",remoteIP,remotePORT);
    else            espSend("AT+CIPSTART=\"UDP\",\"%s\",%d",remoteIP,remotePORT);
    espSend("AT+CIPSEND");
    ret = espParse(Msgget2,">",5);
    return ret;
}



inline void passThroughStop(void){
    espSend("+++");   
    espDelay_ms(5);
}
