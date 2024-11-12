#ifndef CLIENT_H
#define CLIENT_H

#include "server.h"

enum ClientState {
   CL_CONNECTING,
   CL_WAITING,
   CL_PLAYING,
};

typedef struct
{
   int state;
   SOCKET sock;
   char name[BUF_SIZE];
}Client;

#endif /* guard */
