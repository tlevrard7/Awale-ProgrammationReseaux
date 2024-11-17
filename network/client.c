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

void receive_from(SOCKET client, void on_disconnect(SOCKET client), void on_receive(SOCKET client, Buffer* buffer)) {
   fd_set rdfs;
   FD_ZERO(&rdfs);
   FD_SET(client, &rdfs);
   check_read(client + 1, &rdfs);

   if (!FD_ISSET(client, &rdfs)) return;

   Buffer buffer = recv_from(client);

   if (buffer.size <= 0) {
      netlog("disconnected\n\r");
      if (on_disconnect != NULL) on_disconnect(client);
   }
   else {
      netlog("recv %db \n\r", buffer.size);
      if (on_receive != NULL) on_receive(client, &buffer);
   }
}
