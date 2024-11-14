#include "network.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

void init_network() {
#ifdef WIN32
    WSADATA wsa;
    int err = WSAStartup(MAKEWORD(2, 2), &wsa);
    if (err < 0) {
        puts("WSAStartup failed !");
        exit(EXIT_FAILURE);
    }
#endif
}

void end_network() {
#ifdef WIN32
    WSACleanup();
#endif
}

SOCKET open_socket() {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sock == INVALID_SOCKET) {
        perror("socket()");
        exit(errno);
    }
    return sock;
}

void close_socket(SOCKET sock) {
    closesocket(sock);
}

ssize_t recv_from(SOCKET sock, char *buffer) {
    ssize_t n = recv(sock, buffer, BUF_SIZE - 1, 0);
    if (n < 0) perror("recv()");
    return n;
}

ssize_t send_to(SOCKET sock, const char *buffer, size_t n) {
    ssize_t sent = send(sock, buffer, n, 0);
    if (sent < 0) {
        perror("send()");
        exit(errno);
    }
    return sent;
}