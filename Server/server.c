#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "server.h"
#include "client.h"


void addFDs(Server* server) {
   FD_ZERO(&server->rdfs);

   /* add STDIN_FILENO */
   FD_SET(STDIN_FILENO, &server->rdfs);

   /* add the connection socket */
   FD_SET(server->sock, &server->rdfs);

   /* add socket of each client */
   for(int i = 0; i < server->clientCount; i++)
   {
      FD_SET(server->clients[i].sock, &server->rdfs);
   }

}

void handle_connection(Server* server) {
   SOCKADDR_IN csin = {0};
   size_t sinsize = sizeof csin;
   int csock = accept(server->sock, (SOCKADDR *)&csin, &sinsize);
   if (csock == SOCKET_ERROR)
   {
      perror("accept()");
      return;
   }

   /* what is the new maximum fd ? */
   server->max = csock > server->max ? csock : server->max;

   FD_SET(csock, &server->rdfs);

   Client c = {CL_CONNECTING, csock};
   // strncpy(c.name, server->buffer, BUF_SIZE - 1);
   server->clients[server->clientCount] = c;
   server->clientCount++;
}

void handle_packet(Server* server, int c) {
   switch (server->buffer[0])
   {
   case PACKET_CONNECTION:
      buffer_to_ConnectionPacket(server->buffer);

      strncpy(server->clients[i].name, server->buffer, BUF_SIZE - 1);


      break;
   default:
      break;
   }
}

void handle_clients(Server* server) {
   int i = 0;
   for (i = 0; i < server->clientCount; i++)
   {
      /* a client is talking */
      if (FD_ISSET(server->clients[i].sock, &server->rdfs))
      {
         Client client = server->clients[i];
         int c = read_client(server->clients[i].sock, server->buffer);
         /* client disconnected */
         if (c == 0)
         {
            closesocket(server->clients[i].sock);
            remove_client(server->clients, i, &server->clientCount);
            strncpy(server->buffer, client.name, BUF_SIZE - 1);
            strncat(server->buffer, " disconnected !", BUF_SIZE - strlen(server->buffer) - 1);
            send_to_all(server->clients, client, server->clientCount, server->buffer, 1);
         }
         else
         {
            handle_packet(server, i);
            send_to_all(server->clients, client, server->clientCount, server->buffer, 0);
         }
         break;
      }
   }
}

static void clear_clients(Client *clients, int actual)
{
   int i = 0;
   for(i = 0; i < actual; i++)
   {
      closesocket(clients[i].sock);
   }
}

static void remove_client(Client *clients, int to_remove, int *actual)
{
   /* we remove the client in the array */
   memmove(clients + to_remove, clients + to_remove + 1, (*actual - to_remove - 1) * sizeof(Client));
   /* number client - 1 */
   (*actual)--;
}

static void send_to_all(Server* server, const char *buffer)
{
   for(int i = 0; i < server->clientCount; i++) send_to(server->clients[i], buffer);
}

static void send_to(Client client, const char *buffer)
{
   SOCKET sock = client.sock;
   if (send(sock, buffer, strlen(buffer), 0) < 0)
   {
      perror("send()");
      exit(errno);
   }
}

static int init_connection(void)
{
   SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
   SOCKADDR_IN sin = { 0 };

   if(sock == INVALID_SOCKET)
   {
      perror("socket()");
      exit(errno);
   }

   sin.sin_addr.s_addr = htonl(INADDR_ANY);
   sin.sin_port = htons(PORT);
   sin.sin_family = AF_INET;

   if(bind(sock,(SOCKADDR *) &sin, sizeof sin) == SOCKET_ERROR)
   {
      perror("bind()");
      exit(errno);
   }

   if(listen(sock, MAX_CLIENTS) == SOCKET_ERROR)
   {
      perror("listen()");
      exit(errno);
   }

   return sock;
}

static int read_client(SOCKET sock, char *buffer)
{
   int n = 0;

   if((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0)
   {
      perror("recv()");
      /* if recv error we disonnect the client */
      n = 0;
   }

   buffer[n] = 0;

   return n;
}

static void app(void)
{
   Server server;
   server.sock = init_connection();
   server.clientCount = 0;
   server.max = server.sock;

   while(1)
   {
      addFDs(&server);
      
      if (select(server.max + 1, &server.rdfs, NULL, NULL, NULL) == -1)
      {
         perror("select()");
         exit(errno);
      }


      /* something from standard input : i.e keyboard */
      if(FD_ISSET(STDIN_FILENO, &server.rdfs))
      {
         //handle_stdin(&server);
         /* stop process when type on keyboard */
         break;
      }
      else if(FD_ISSET(server.sock, &server.rdfs))
      {
         handle_connection(&server);
      }
      else
      {
         handle_clients(&server);
      }
   }

   clear_clients(server.clients, server.clientCount);
   end_connection(server.sock);
}

int main(int argc, char **argv)
{
   init();

   app();

   end();

   return EXIT_SUCCESS;
}
