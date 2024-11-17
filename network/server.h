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

int accept_connection(Server *server, void on_connection(Server *server, int client));
void disconnect_client(Server *server, int i);

void send_all(Server *server, const Buffer* buffer);
void receive_any(Server *server, void on_disconnect(Server *server, int client), void on_receive(Server *server, int recvFrom, Buffer* buffer));
