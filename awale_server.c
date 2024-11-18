#include "network/server.h"
#include "packets/packets.h"
#include "stdio.h"
#include "string.h"
#include <stdlib.h>
#include <errno.h>

int nextPlayerId;

typedef struct Client {
    char name[MAX_NAME_SIZE+1];
} Client;

Server server;
Client players[MAX_CLIENTS];

void on_connection(int client, ConnectionPacket packet) {
    printf("%s joined\n",packet.player.name); 
    // On rajoute le nouveau joueur à la liste players
    strncpy(players[server.clientCount-1].name, packet.player.name, strlen(packet.player.name));
    ConnectionAckPacket response;
    response.id = nextPlayerId;
    Buffer buffer = serialize_ConnectionAckPacket(&response);
    send_to(server.clients[client], &buffer);
}

// void on_player_list_req(int client, PlayerListReqPacket packet) {
//     (void)client;
//     (void)packet;
// }

void send_user_names_list(int numClientToSend){
    AnswerUsernamesListPacket  answerUsernamesListPacket;
    // On récupère la liste des pseudos
    for(int i=0; i<server.clientCount; i++){
        strcpy(answerUsernamesListPacket.playersNames[i], players[i].name);
    }
    answerUsernamesListPacket.nbPlayers = server.clientCount;
    Buffer buffer = serialize_AnswerUsernamesListPacket(&answerUsernamesListPacket);
    send_to(server.clients[numClientToSend], &buffer);
}

// void on_receive(Server *server, int recvFrom, Buffer* buffer) {
//     switch (buffer->data[0]) {
//     case PACKET_CONNECTION:
//         on_connected(server, recvFrom, deserialize_ConnectionPacket(buffer));
//         break;
//     case PACKET_REQUEST_USER_NAMES_LIST:
//         send_user_names_list(server,recvFrom);
//         break;
//     }
// }

void on_disconnect(int numClient) {
    // On enlève le joueur de la liste des players
    memmove(players + numClient, players + numClient + 1, (server.clientCount - numClient - 1) * MAX_NAME_SIZE *  sizeof(char));
}

int main(int argc, char **argv){
    
    if(argc !=2) {
        printf("Usage : %s [port] \n", argv[0]);
        return 1;
    }

    init_network();

    nextPlayerId = 1;
    server = create_server(atoi(argv[1]));

    while (1) {
        fd_set rdfs;
        FD_ZERO(&rdfs);
        fd_set_server(&server, &rdfs);
        int n;
        if ((n = select(server.maxFd+1, &rdfs, NULL, NULL, NULL)) == -1) {
            perror("select()");
            exit(errno);
        }
        if (fd_is_set_accept(&server, &rdfs)) {
            accept_connection(&server);
        } else {
            int client;
            Buffer buffer = receive_server(&server, &client, &rdfs);
            if (buffer.size == 0) {
                on_disconnect(client);
                disconnect_client(&server, client);
                continue;
            }
            switch (buffer.data[0]) {
            case PACKET_CONNECTION:
                on_connection(client, deserialize_ConnectionPacket(&buffer));
                break;
            case PACKET_REQUEST_USER_NAMES_LIST:
                send_user_names_list(client);
                break;

            // case PACKET_PLAYER_LIST_REQ:
            //     on_player_list_req(client, deserialize_PlayerListReqPacket(&buffer));
            //     break;
            }
        }
    }

    close_server(&server);

    end_network();
}
