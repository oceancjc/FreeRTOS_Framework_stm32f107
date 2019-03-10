#ifndef __CMD_SERVER_H__
#define __CMD_SERVER_H__



typedef struct {
    char cmd[20];
    int (*func)(void*para);
}CMDLIST_t;

extern int cmd_analysis(char* msg, char*cmd, int* opdata);

#endif

