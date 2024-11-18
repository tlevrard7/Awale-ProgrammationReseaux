#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "server.h"
#include "client.h"

Server create_server(int port) {
    SOCKET sock = open_socket();

    SOCKADDR_IN sin = {0};
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(port);
    sin.sin_family = AF_INET;

    if (bind(sock, (SOCKADDR *)&sin, sizeof sin) == SOCKET_ERROR) {
        perror("bind()");
        exit(errno);
    }

    if (listen(sock, MAX_CLIENTS) == SOCKET_ERROR) {
        perror("listen()");
        exit(errno);
    }

    Server server;
    server.sock = sock;
    server.maxFd = sock;
    server.clientCount = 0;
    return server;
}

void remove_client(Server *server, int i) { close_socket(server->clients[i]); }

void close_server(Server *server) {
    int i = 0;
    for (i = 0; i < server->clientCount; i++) remove_client(server, i);
    close_socket(server->sock);
}

void disconnect_client(Server *server, int i) {
    remove_client(server, i);
    memmove(server->clients + i, server->clients + i + 1, (server->clientCount - i - 1) * sizeof(SOCKET));
    server->clientCount--;
}

void fd_set_server(Server* server, fd_set* rdfs) {
    FD_SET(server->sock, rdfs);
    for (int i = 0; i < server->clientCount; i++) FD_SET(server->clients[i], rdfs);
}
int fd_is_set_accept(Server* server, fd_set *rdfs) {
    return FD_ISSET(server->sock, rdfs);
}

void accept_connection(Server *server) {
    SOCKADDR_IN csin = {0};
    socklen_t sinsize = sizeof csin;
    int csock = accept(server->sock, (SOCKADDR *)&csin, &sinsize);
    if (csock == SOCKET_ERROR) {
       perror("accept()");
       return;
    }
    netlog("%i connected\n\r", server->clientCount);

    server->maxFd = csock > server->maxFd ? csock : server->maxFd;
    int client = server->clientCount;
    server->clients[client] = csock;
    server->clientCount++;
}

void send_all(Server * server, const Buffer* buffer) {
    for (int i = 0; i < server->clientCount; i++) send_to(server->clients[i], buffer);
}

Buffer receive_server(Server *server, int* client, fd_set* rdfs) {
    for (int i = 0; i < server->clientCount; i++) {
        if (!FD_ISSET(server->clients[i], rdfs)) continue;
        *client = i;
        Buffer buffer = recv_from(server->clients[i]);
        if (buffer.size <= 0) netlog("%d disconnected\n\r", i);
        else netlog("recv %db from %d\n\r", buffer.size, i);
        return buffer;
    }
    *client = -1;
    return new_buffer();
}
