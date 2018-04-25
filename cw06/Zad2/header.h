#ifndef __HEADERH
#define __HEADERH

#define LOGIN   1
#define MIRROR  2
#define CALC    3
#define TIME    4
#define END     5
#define STOP    6

#define SEED 666
#define MAX_MSG 10
#define MAX_MSG_LEN 256
#define MAX_CLIENTS 7
#define SERVER_PATH "/server"

typedef struct Msg
{
    long msgType;
    char txt[MAX_MSG_LEN];
    pid_t senderPID;
} Msg;

const size_t MSG_SIZE = sizeof(Msg);

#endif
