#pragma once

#include <stdio.h>
#include "buffer.h"

#ifdef WIN32

#include <winsock2.h>

#elif defined(linux)

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h>  /* gethostbyname */
#include <sys/select.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

#else

#error not defined for this platform

#endif

#define CRLF "\r\n"
#define MAX_CLIENTS 100

#ifdef NETLOG
#define netlog(...) printf("[net] " __VA_ARGS__)
#else
#define netlog(...)
#endif

int check_read(int ndfs, fd_set *rdfs);

void init_network();
void end_network();

SOCKET open_socket();
void close_socket(SOCKET sock);

Buffer recv_from(SOCKET sock);
void send_to(SOCKET sock, const Buffer *buffer);