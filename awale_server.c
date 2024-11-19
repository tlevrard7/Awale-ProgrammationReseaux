#include "awale/awale.h"
#include "network/server.h"
#include "packets/packets.h"
#include "stdio.h"
#include "string.h"
#include <stdlib.h>
#include <errno.h>

#define MAX_GAMES 8

Server server;
Player players[MAX_CLIENTS];

size_t gameCount;
Game games[MAX_GAMES];

int nextPlayerId;
int new_player_id(Player* player) {
    (void)player;
    return nextPlayerId++; // idea: hash(name+id)
}

int get_num_client_by_idClient(uint32_t idClient){
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (players[i].id == idClient) {
            return i;
        }
    }
    return -1; 
}

int nextGameId;
uint32_t create_game() {
    if (gameCount == MAX_GAMES) return -1;
    int index = gameCount++;
    games[index].id = nextGameId++;
    games[index].awale = init_game();
    return index;
}
int get_game_index(uint32_t id) {
    for (size_t i = 0; i < gameCount; i++) {
        if (games[i].id == id) return i;
    }
    return -1;
}

int get_game_player_index(Game* game, uint32_t playerId) {
    for (size_t i = 0; i < PLAYER_COUNT; i++) {
        if (game->playerIds[i] == playerId) return i;
    }
    return -1;
}

void remove_game(int gameIndex) {
    memmove(games + gameIndex, games + gameIndex + 1, (gameCount - gameIndex - 1) * sizeof(Game));
    gameCount--;
}

void on_connection(int client, ConnectionPacket packet) {
    if (players[client].status != CONNECTING) return;
    printf("%s joined\r\n",packet.player.name); 
    // On rajoute le nouveau joueur à la liste players

    players[client].status = IDLE;
    players[client].id = new_player_id(&packet.player);
    strcpy(players[client].name, packet.player.name);

    ConnectionAckPacket response;
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

void sync_game(Game* game) {
    AwaleSyncPacket packet = {game->awale};
    Buffer buffer = serialize_AwaleSyncPacket(&packet);
    for (size_t i = 0; i < PLAYER_COUNT; i++) send_to(server.clients[get_num_client_by_idClient(game->playerIds[i])], &buffer);
}

void process_challenge_in_duel_packet(int client, ChallengeInDuelPacket packet){
    switch(packet.etat){
        Buffer buffer;
        int requesterClient;
        case SENT:

            // BUG can challenge self
            if (players[client].status != IDLE) return;
            // Un client vient de challenger un autre client, on doit transmettre au challengé
            int opponentClient = get_num_client_by_idClient(packet.opponent.id);
            if(opponentClient != -1 && players[opponentClient].status == IDLE){
                // le challengé existe
                players[client].status = CHALLENGED;
                players[opponentClient].status = CHALLENGED;
                packet.opponent = players[opponentClient];
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

            // BUG 1 challenges 2, 3 challenges 4 then 1 accepts a challenge with 3
            if (players[client].status != CHALLENGED || players[requesterClient].status != CHALLENGED) return;
            send_to(server.clients[requesterClient], &buffer);

            int index = create_game();
            if (index != -1) {
                games[index].playerIds[0] = players[requesterClient].id;
                players[requesterClient].gameId = games[index].id;
                players[requesterClient].status = PLAYING;
                games[index].playerIds[1] = players[client].id;
                players[client].gameId = games[index].id;
                players[client].status = PLAYING;
                sync_game(&games[index]);
                printf("Started a game (%d) between %s (%d) and %s (%d)\r\n", games[index].id, players[requesterClient].name, players[requesterClient].id, players[client].name, players[client].id);
            } else {
                // TODO cannot start game
            }
            break;
        case REFUSED:
            // On l'envoie au requester 
            buffer = serialize_ChallengeInDuelPacket(&packet);
            requesterClient = get_num_client_by_idClient(packet.requester.id);
            players[client].status = IDLE;
            players[requesterClient].status = IDLE;
            send_to(server.clients[requesterClient], &buffer);
            break;
        case OPPONENT_DOESNT_EXIST:
            // ne doit pas arriver normalement car c'est le serveur qui met cet été
            break;
    }
}

void on_awale_play(int client, AwalePlayPacket packet) {
    if (players[client].status != PLAYING) return;
    int gIndex = get_game_index(players[client].gameId);
    PlayResult res = play(&games[gIndex].awale, get_game_player_index(&games[gIndex], players[client].id), packet.cell);

    AwalePlayAckPacket response = {res};
    Buffer buffer = serialize_AwalePlayAckPacket(&response);
    send_to(server.clients[client], &buffer);
    if (res != PLAYED) return;

    sync_game(&games[gIndex]);

    if (games[gIndex].awale.state < 0) return;
    printf("Game %d finish with state %d\r\n", players[client].gameId, games[gIndex].awale.state);
    for (size_t i = 0; i < PLAYER_COUNT; i++) {
        int pIndex = get_num_client_by_idClient(games[gIndex].playerIds[i]);
        players[pIndex].status = IDLE;
        players[pIndex].gameId = 0;
        remove_game(gIndex);
    }
}

void on_disconnect(int numClient) {
    // On enlève le joueur de la liste des players
    memmove(players + numClient, players + numClient + 1, (server.clientCount - numClient - 1) * sizeof(Player));
}

int main(int argc, char **argv){
    
    if(argc !=2) {
        printf("Usage : %s <port> \r\n", argv[0]);
        return 1;
    }

    init_network();

    nextPlayerId = 1;
    nextGameId = 1;
    gameCount = 0;
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
            case PACKET_AWALE_PLAY:
                on_awale_play(client, deserialize_AwalePlayPacket(&buffer));
                break;
            }
        }
    }

    close_server(&server);

    end_network();
}
