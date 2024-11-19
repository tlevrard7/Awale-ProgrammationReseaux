#include "network/server.h"
#include "packets/packets.h"
#include "stdio.h"
#include "string.h"
#include <stdlib.h>
#include <errno.h>


Server server;
Player players[MAX_CLIENTS];

int nextPlayerId;
int new_player_id(Player* player) {
    (void)player;
    return nextPlayerId++; // idea: hash(name+id)
}

int get_num_client_by_idClient(int idClient){
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if ((int) players[i].id == idClient) {
            return i;
        }
    }
    return -1; 
}

void on_connection(int client, ConnectionPacket packet) {
    if (players[client].status != CONNECTING) return;
    printf("%s joined\n",packet.player.name); 
    // On rajoute le nouveau joueur à la liste players
    ConnectionAckPacket response;

    // TODO reconnect
    // if (packet.player.id != -1) players[client].id = packet.player.id;
    // else
    players[client].status = IDLE;
    players[client].id = new_player_id(&packet.player);
    strcpy(players[client].name, packet.player.name);

    response.id = players[client].id;
    Buffer buffer = serialize_ConnectionAckPacket(&response);
    send_to(server.clients[client], &buffer);
}

void send_user_names_list(int numClientToSend){
    AnswerUsernamesListPacket packet;
    packet.count = server.clientCount;
    for(int i=0; i<server.clientCount; i++) packet.players[i] = players[i];
    Buffer buffer = serialize_AnswerUsernamesListPacket(&packet);
    send_to(server.clients[numClientToSend], &buffer);
}

void process_challenge_in_duel_packet(int client, ChallengeInDuelPacket packet){
    switch(packet.etat){
        Buffer buffer;
        int requesterClient;
        case SENT:
            // Un client vient de challenger un autre client, on doit transmettre au challengé
            int opponentClient = get_num_client_by_idClient(packet.opponent.id);
            if(opponentClient!=-1){
                // le challengé existe
                buffer = serialize_ChallengeInDuelPacket(&packet);
                send_to(server.clients[opponentClient], &buffer);
            }
            else{
                // le challengé n'existe pas, on prévient le requester
                packet.etat = OPPONENT_DOESNT_EXIST;
                buffer = serialize_ChallengeInDuelPacket(&packet);
                send_to(server.clients[client], &buffer);
            }
            break;
        case ACCEPTED:
            // On l'envoie au requester 
            buffer = serialize_ChallengeInDuelPacket(&packet);
            requesterClient = get_num_client_by_idClient(packet.requester.id);
            send_to(server.clients[requesterClient], &buffer);
            break;
        case REFUSED:
            // On l'envoie au requester 
            buffer = serialize_ChallengeInDuelPacket(&packet);
            requesterClient = get_num_client_by_idClient(packet.requester.id);
            send_to(server.clients[requesterClient], &buffer);
            break;
        case OPPONENT_DOESNT_EXIST:
            // ne doit pas arriver normalement car c'est le serveur qui met cet été
            break;
    }
}

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
            players[server.clientCount-1].status = CONNECTING;
            players[server.clientCount-1].id = 0;
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
            case PACKET_CHALLENGE_IN_DUEL:
                process_challenge_in_duel_packet(client, deserialize_ChallengeInDuelPacket(&buffer));
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
