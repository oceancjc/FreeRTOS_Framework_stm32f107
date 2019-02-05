/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *    Sergio R. Caprile - "commonalization" from prior samples and/or documentation extension
 *******************************************************************************/

#include "transport.h"

/**
This simple low-level implementation assumes a single connection for a single thread. Thus, a static
variable is used for that connection.
On other scenarios, the user must solve this by taking into account that the current implementation of
MQTTPacket_read() has a function pointer for a function call to get the data to a buffer, but no provisions
to know the caller or other indicator (the socket id): int (*getfn)(unsigned char*, int)
*/
static int mysock = SOCK_MQTT;




int transport_sendPacketBuffer(int sock, unsigned char* buf, int buflen)
{
    return send(SOCK_MQTT,buf,buflen);
}


int transport_getdata(unsigned char* buf, int count)
{
    return recv(mysock,buf,count);
}

int transport_getdatanb(void *sck, unsigned char* buf, int count)
{
	int sock = *((int *)sck); 	/* sck: pointer to whatever the system may use to identify the transport */
	/* this call will return after the timeout set on initialization if no bytes;
	   in your system you will use whatever you use to get whichever outstanding
	   bytes your socket equivalent has ready to be extracted right now, if any,
	   or return immediately */
	int rc = recv(sock, buf, count);	
	if (rc == -1) {
		/* check error conditions from your system here, and return -1 */
		return 0;
	}
	return rc;
}

/**
return >=0 for a socket descriptor, <0 for an error code
@todo Basically moved from the sample without changes, should accomodate same usage for 'sock' for clarity,
removing indirections
*/
int transport_open(char* addr, int port)
{
    int ret = socket(SOCK_MQTT,Sn_MR_TCP,4500,Sn_MR_ND);
    if(ret != SOCK_OK)    uart1_printf("%d:Socket Error\r\n",SOCK_MQTT);
    else                  uart1_printf("%d:Opened\r\n",SOCK_MQTT);
    return ret;
}

int transport_close(int sock)
{
    close(SOCK_MQTT);
    return 0;
}



#define BUFLEN     200
int mqtt_remoteConnect(char* clientID, uint16_t keepalive, uint8_t cleansession, char*username, char* password){
    int len = 0;
    uint8_t buf[BUFLEN] = { 0 };
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.clientID.cstring  = clientID;
    data.username.cstring  = username;
    data.password.cstring  = password;
    data.keepAliveInterval = keepalive;
    data.cleansession      = cleansession;
    len = MQTTSerialize_connect(buf, BUFLEN, &data);
    /* 1. Connect Remote Server */
    transport_sendPacketBuffer(SOCK_MQTT, buf, len);
    /* wait for connack */
    if (MQTTPacket_read(buf, sizeof(buf), transport_getdata) == CONNACK){
        unsigned char sessionPresent, connack_rc;
        if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, buf, BUFLEN) != 1 || connack_rc != 0){
            uart1_printf("Unable to connect, return code %d\n", connack_rc);
            return connack_rc;
        }
        return 0;
    }
    else    return -1; 
}



int mqtt_publish(char *pTopic,char *pMessage){
    int len = 0,rc = 0;
    unsigned char buf[256] = { 0 };
    MQTTString topicString = MQTTString_initializer;
    int msglen = strlen(pMessage);
    int buflen = sizeof(buf);
    
    topicString.cstring = pTopic;
    /* 2 */
    len = MQTTSerialize_publish(buf, buflen, 0, 0, 0, 0, topicString, (unsigned char*)pMessage, msglen); 
    /* 3 */
//    memset(buf,0,buflen);
//    len += MQTTSerialize_disconnect(buf, buflen); 
    rc = transport_sendPacketBuffer(SOCK_MQTT,buf,len);
    memset(buf,0,buflen); 
    if (rc == len)        return len;             
    else                  return -1;
}


int mqtt_ping(uint8_t* buf){   //return 0 ok   -1 receive error   -2 receive wrong msg
    int len = MQTTSerialize_zero(buf, 20, PINGREQ);
    transport_sendPacketBuffer(SOCK_MQTT,buf,len);
    int retry = 10;
    while((len=getSn_RX_RSR(SOCK_MQTT)) <= 0 && retry--);
    if(retry <=0)    return -1;
    uint8_t buf_r[100] = { 0 };
    recv(SOCK_MQTT,buf_r,len);
    if(buf_r[0] == 0xD0)    return 0;
    else                    return -2;
    
}






