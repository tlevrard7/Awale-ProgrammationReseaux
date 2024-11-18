#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "client.h"

SOCKET create_client(const char *address, int port) {

    SOCKET sock = open_socket();
 
    struct hostent *hostinfo;
    hostinfo = gethostbyname(address);
    if (hostinfo == NULL) {
        fprintf(stderr, "Unknown host %s.\n", address);
        exit(EXIT_FAILURE);
    }
 
    SOCKADDR_IN sin = {0};
    sin.sin_addr = *(IN_ADDR *)hostinfo->h_addr_list[0];
    sin.sin_port = htons(port);
    sin.sin_family = AF_INET;
 
    if (connect(sock, (SOCKADDR *)&sin, sizeof(SOCKADDR)) == SOCKET_ERROR) {
        perror("connect()");
        exit(errno);
    }
 
    return sock;
}

void close_client(SOCKET client) {
    close_socket(client);
}

void fd_set_client(SOCKET client, fd_set* rdfs) { FD_SET(client, rdfs); }
int fd_is_set_client(SOCKET client, fd_set *rdfs) { return FD_ISSET(client, rdfs); }

Buffer receive_client(SOCKET client) {
    Buffer buffer = recv_from(client);
    if (buffer.size == 0) netlog("disconnected\n\r");
    else netlog("recv %db \n\r", buffer.size);
    return buffer;
}
