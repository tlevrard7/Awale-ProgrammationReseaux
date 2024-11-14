#pragma once

#include "network.h"

typedef struct Server {
    SOCKET sock;
    int maxFd;
    SOCKET clients[MAX_CLIENTS];
    int clientCount;
} Server;

Server create_server(int port);
void close_server(Server *server);

int accept_connection(Server* server, SOCKET* client);
void disconnect_client(Server *server, int i);

void send_all(Server *server, const char *buffer, size_t);
ssize_t receive_any(Server* server, SOCKET* client, char *buffer);
