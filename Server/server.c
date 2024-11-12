#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "server.h"
#include "client.h"

static void init(void)
{
#ifdef WIN32
   WSADATA wsa;
   int err = WSAStartup(MAKEWORD(2, 2), &wsa);
   if(err < 0)
   {
      puts("WSAStartup failed !");
      exit(EXIT_FAILURE);
   }
#endif
}

static void end(void)
{
#ifdef WIN32
   WSACleanup();
#endif
}

static void addFDs(Server* server) {
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

static void handle_connection(Server* server) {
   SOCKADDR_IN csin = {0};
   size_t sinsize = sizeof csin;
   int csock = accept(server->sock, (SOCKADDR *)&csin, &sinsize);
   if (csock == SOCKET_ERROR)
   {
      perror("accept()");
      return;
   }

   /* after connecting the client sends its name */
   if (read_client(csock, server->buffer) == -1)
   {
      /* disconnected */
      return;
   }

   /* what is the new maximum fd ? */
   server->max = csock > server->max ? csock : server->max;

   FD_SET(csock, &server->rdfs);

   Client c = {csock};
   strncpy(c.name, server->buffer, BUF_SIZE - 1);
   server->clients[server->clientCount] = c;
   server->clientCount++;
}

static void handle_clients(Server* server) {
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
            send_message_to_all_clients(server->clients, client, server->clientCount, server->buffer, 1);
         }
         else
         {
            send_message_to_all_clients(server->clients, client, server->clientCount, server->buffer, 0);
         }
         break;
      }
   }
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

static void send_message_to_all_clients(Client *clients, Client sender, int actual, const char *buffer, char from_server)
{
   int i = 0;
   char message[BUF_SIZE];
   message[0] = 0;
   for(i = 0; i < actual; i++)
   {
      /* we don't send message to the sender */
      if(sender.sock != clients[i].sock)
      {
         if(from_server == 0)
         {
            strncpy(message, sender.name, BUF_SIZE - 1);
            strncat(message, " : ", sizeof message - strlen(message) - 1);
         }
         strncat(message, buffer, sizeof message - strlen(message) - 1);
         write_client(clients[i].sock, message);
      }
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

static void end_connection(int sock)
{
   closesocket(sock);
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

static void write_client(SOCKET sock, const char *buffer)
{
   if(send(sock, buffer, strlen(buffer), 0) < 0)
   {
      perror("send()");
      exit(errno);
   }
}

int main(int argc, char **argv)
{
   init();

   app();

   end();

   return EXIT_SUCCESS;
}
