#include "app_tsks.h"
#include "cmd_server.h"

/************** Command register Part **********************/
int cmd_learn(void* para){
    if(IrdaLearnTsk == NULL){
        uart1_printf("Please Enter RC learning mode first\r\n");
        return -1;
    }
    IrdaLearnTskPara.opdata[0] = ((int*)para)[0];
    IrdaLearnTskPara.tsk2notify = IrdaLearnTsk;
    xTaskNotifyGive(IrdaLearnTskPara.tsk2notify);
    return 0;
}


int cmd_use(void* para){
    if(IrdaSendTsk == NULL){
        uart1_printf("Please Enter RC Sending mode first\r\n");
        return -1;
    }
    IrdaSendTskPara.opdata[0] = ((int*)para)[0];
    IrdaSendTskPara.tsk2notify = IrdaSendTsk;
    xTaskNotifyGive(IrdaSendTskPara.tsk2notify);
    return 0;
}


int cmd_remote_set(void* para){
    int* opdata = (int*)para;
    if(opdata[0] == 1 && IrdaLearnTsk == NULL){
        if(IrdaSendTsk){    
            vTaskDelete(IrdaSendTsk);    IrdaSendTsk = NULL;    
        }
        xTaskCreate(irda_learning_Task, "code_learning", 800, &IrdaLearnTskPara, tskIDLE_PRIORITY + 2, &IrdaLearnTsk);
    }    
    else if(opdata[0] != 1 && IrdaSendTsk == NULL){
        if(IrdaLearnTsk){
            vTaskDelete(IrdaLearnTsk);    IrdaLearnTsk = NULL;    
        }
        xTaskCreate(irda_sending_Task, "code_sending", configMINIMAL_STACK_SIZE, &IrdaSendTskPara, tskIDLE_PRIORITY + 2, &IrdaSendTsk);
    }  
    return 0;
}


int cmd_moter_ctl(void* para){
    int* opdata = (int*)para;
    if(opdata[0] == 1 && SteeringEngCtlTsk == NULL )    
        xTaskCreate(steeringCtl_Task, "Steering Engine", configMINIMAL_STACK_SIZE, 
                    (void*)&SteeringCtlPara, tskIDLE_PRIORITY + 2, &SteeringEngCtlTsk);
    else if(opdata[0] != 1 && SteeringEngCtlTsk != NULL){
        Timer3_pwm_Deinit();
        Uart1SendStr("Exit Steering Enginee Ctl\r\n");
        vTaskDelete(SteeringEngCtlTsk);    SteeringEngCtlTsk = NULL;
    }
    return 0;
}



int cmd_moter_set(void* para){
    if(SteeringEngCtlTsk == NULL){
        Uart1SendStr("Enter moter control mode first\r\n");
        return -1;
    }    
    SteeringCtlPara.opdata[0] = ((int*)para)[0];
    SteeringCtlPara.tsk2notify = SteeringEngCtlTsk;
    xTaskNotifyGive(SteeringCtlPara.tsk2notify);
    return 0;
}


int cmd_lan_enable(void* para){
    if(lantcps_loopbackTsk == NULL){
        lanloopbackPara.opdata[0] = ((int*)para)[0];
        xTaskCreate(lantcpserver_loopback_Task, "Lan TCP Server Loopback", configMINIMAL_STACK_SIZE<<1, 
                    (void*)&lanloopbackPara, tskIDLE_PRIORITY + 2, &lantcps_loopbackTsk);            
    }
    else    Uart1SendStr("Already enabled Lan\r\n");
    return 0;
}

int cmd_lan_disable(void* para){
    if(lantcps_loopbackTsk != NULL){
        Uart1SendStr("Lan disabled now\r\n");
        vTaskDelete(lantcps_loopbackTsk);    lantcps_loopbackTsk = NULL;
        return 0;
    }
    Uart1SendStr("Lan already disabled\r\n");
    return 0;
}



static CMDLIST_t cmdlist[] = {
    {"learn",           cmd_learn},
    {"use",             cmd_use},
    {"moter_ctl",       cmd_moter_ctl},
    {"moter_set",       cmd_moter_set},
    {"lan_enable",      cmd_lan_enable},
    {"lan_disable",     cmd_lan_disable},
    {{0},                NULL}
};

int cmd_analysis(char* msg, char*cmd, int* opdata){
    CMDLIST_t *cmd_p = cmdlist;
    sscanf((char*)msg,"%s %d %d %d",cmd,&opdata[0],&opdata[1],&opdata[2]);
    while(cmd_p->cmd[0]!=0){
        if(strstr((char*)cmd,cmdlist->cmd))    return cmdlist->func((void*)opdata);    
        cmd_p++;
    }
    return -2;
}


