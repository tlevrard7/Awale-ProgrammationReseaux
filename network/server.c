#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "server.h"
#include "client.h"

Server create_server(int port)
{
    SOCKET sock = open_socket();

    SOCKADDR_IN sin = {0};
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(port);
    sin.sin_family = AF_INET;

    if (bind(sock, (SOCKADDR *)&sin, sizeof sin) == SOCKET_ERROR)
    {
        perror("bind()");
        exit(errno);
    }

    if (listen(sock, MAX_CLIENTS) == SOCKET_ERROR)
    {
        perror("listen()");
        exit(errno);
    }

    Server server;
    server.sock = sock;
    server.maxFd = sock;
    server.clientCount = 0;
    return server;
}

void remove_client(Server *server, int i) {
    close_socket(server->clients[i]);
}

void close_server(Server *server) {
    int i = 0;
    for (i = 0; i < server->clientCount; i++) remove_client(server, i);
    close_socket(server->sock);
}

void disconnect_client(Server *server, int i) {
    printf("[net] %d disconnected\n\r", i);
    remove_client(server, i);
    memmove(server->clients + i, server->clients + i + 1, (server->clientCount- - i - 1) * sizeof(SOCKET));
    server->clientCount--;
}

int accept_connection(Server *server, SOCKET *client) {
    fd_set rdfs;
    FD_ZERO(&rdfs);
    FD_SET(server->sock, &rdfs);
    if (!FD_ISSET(server->sock, &rdfs)) return 0;

    SOCKADDR_IN csin = {0};
    socklen_t sinsize = sizeof csin;
    int csock = *client = accept(server->sock, (SOCKADDR *)&csin, &sinsize);
    if (csock == SOCKET_ERROR) {
       perror("accept()");
       return 0;
    }

    server->maxFd = csock > server->maxFd ? csock : server->maxFd;
    server->clients[server->clientCount] = csock;
    server->clientCount++;

    return 1;
}

void send_all(Server * server, const char * buffer, size_t n) {
    for (int i = 0; i < server->clientCount; i++) send_to(server->clients[i], buffer, n);
}

ssize_t receive_any(Server* server, int* recvFrom, char* buffer) {
    fd_set rdfs;
    FD_ZERO(&rdfs);

    for(int i = 0; i < server->clientCount; i++) FD_SET(server->clients[i], &rdfs);

    if (select(server->maxFd+1, &rdfs, NULL, NULL, NULL) == -1) {
        perror("select()");
        exit(errno);
    }

    for (int i = 0; i < server->clientCount; i++) {
        if (!FD_ISSET(server->clients[i], &rdfs)) continue;
        *recvFrom = i;
        ssize_t n = recv_from(server->clients[i], buffer);
        if (n <= 0) disconnect_client(server, i);
        else printf("[net] recv %ldb from %d\n\r", n, i);
        return n;
   }

    return 0;
}
