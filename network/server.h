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

void fd_set_server(Server* server, fd_set* rdfs);
int fd_is_set_accept(Server* server, fd_set *rdfs);

void accept_connection(Server *server);
void disconnect_client(Server *server, int i);
Buffer receive_server(Server *server, int* client, fd_set* rdfs);

void send_all(Server *server, const Buffer* buffer);
