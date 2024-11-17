#include "network/server.h"
#include "packets/packets.h"
#include "stdio.h"
#include "string.h"
#include <stdlib.h>

typedef struct Client {
    char name[MAX_NAME_SIZE+1];
} Client;

Client players[MAX_CLIENTS];

void on_connection(Server *server, int client) { }

void on_connected(Server *server, int numClient, ConnectionPacket packet) {
    printf("%s joined\n",packet.name); 
    // On rajoute le nouveau joueur à la liste players
    strncpy(players[numClient].name, packet.name, strlen(packet.name)); 
    on_connection(server, numClient);
}

void send_user_names_list(Server *server, int numClientToSend){
    char buffer[BUF_SIZE];
    AnswerUsernamesListPacket  answerUsernamesListPacket;
    // On récupère la liste des pseudos
    for(int i=0; i<server->clientCount; i++){
        strcpy(answerUsernamesListPacket.playersNames[i], players[i].name);
    }
    answerUsernamesListPacket.nbPlayers = server->clientCount;
    int n = serialize_AnswerUsernamesListPacket(&answerUsernamesListPacket, buffer);
    send_to(server->clients[numClientToSend], buffer, n);
}

void on_receive(Server *server, int recvFrom, char *buffer, size_t n) {
    switch (buffer[0]) {
    case PACKET_CONNECTION:
        on_connected(server, recvFrom, deserialize_ConnectionPacket(buffer, n));
        break;
    case PACKET_REQUEST_USER_NAMES_LIST:
        send_user_names_list(server,recvFrom);
        break;
    }
}



void on_disconnect(Server* server, int numClient) {
    // On enlève le joueur de la liste des players
    memmove(players + numClient, players + numClient + 1, (server->clientCount - numClient - 1) * MAX_NAME_SIZE *  sizeof(char));
}

int main(int argc, char **argv){
    
    if(argc !=2)
    {
        printf("Usage : %s [port] \n", argv[0]);
        return 1;
    }

    init_network();

    Server server = create_server(atoi(argv[1]));
    
    while (1) {
        accept_connection(&server, on_connection);
        receive_any(&server, on_disconnect, on_receive);
    }

    close_server(&server);

    end_network();
}
