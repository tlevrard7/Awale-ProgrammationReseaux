#include "network.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

int check_read(int ndfs, fd_set *rdfs) {
    // 100ms timeout
    struct timeval tv = {0, 100000};
    int n;
    if ((n = select(ndfs, rdfs, NULL, NULL, &tv)) == -1) {
        perror("select()");
        exit(errno);
    }
    return n;
}

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

Buffer recv_from(SOCKET sock) {
    Buffer buffer = new_buffer();
    ssize_t n = recv(sock, &buffer.size, sizeof(short), 0);
    if (n < 0) {
        perror("recv()");
        buffer.size = 0;
    }
    else {
        if (buffer.size == 0) netlog("%d disconnected\r\n", sock);
        else netlog("recv %db from %d\r\n", buffer.size, sock);
        recv(sock, buffer.data, buffer.size, 0);
    }
    return buffer;
}

void send_to(SOCKET sock, const Buffer* buffer) {
    ssize_t n = send(sock, &buffer->size, sizeof(short), 0);
    if (n < 0) {
        perror("send()");
        exit(errno);
    }
    send(sock, buffer->data, buffer->size, 0);
    netlog("send %db to %d \r\n", buffer->size, sock);
}