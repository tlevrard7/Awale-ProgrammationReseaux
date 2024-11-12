#ifndef SERVER_H
#define SERVER_H

#include "../network.h"
#include "client.h"

typedef struct Server {
    SOCKET sock;
    int max;
    Client clients[MAX_CLIENTS];
    int clientCount;
    fd_set rdfs;
    char buffer[BUF_SIZE];
} Server;

static void app(void);
static int init_connection(void);
static int read_client(SOCKET sock, char *buffer);
static void write_client(SOCKET sock, const char *buffer);
static void send_to_all(Client *clients, Client client, int actual, const char *buffer, char from_server);
static void remove_client(Client *clients, int to_remove, int *actual);
static void clear_clients(Client *clients, int actual);

#endif /* guard */
