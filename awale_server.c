#include "awale/awale.h"
#include "network/server.h"
#include "packets/packets.h"
#include "stdio.h"
#include "string.h"
#include <stdlib.h>
#include <errno.h>

Server server;
Player players[MAX_CLIENTS];

size_t lobbyCount;
Lobby lobbies[MAX_LOBBIES];

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

int nextLobbyId;
uint32_t create_lobby() {
    if (lobbyCount == MAX_LOBBIES) return -1;
    int index = lobbyCount++;
    lobbies[index].id = nextLobbyId++;
    lobbies[index].awale = init_game();
    return index;
}
int get_game_index(Lobby* lobby) {
    for (size_t i = 0; i < lobbyCount; i++) {
        if (&lobbies[i] == lobby) return i;
    }
    return -1;
}

int get_game_player_index(Lobby* game, uint32_t playerId) {
    for (size_t i = 0; i < PLAYER_COUNT; i++) {
        if (game->players[i]->id == playerId) return i;
    }
    return -1;
}

void remove_game(int gameIndex) {
    memmove(lobbies + gameIndex, lobbies + gameIndex + 1, (lobbyCount - gameIndex - 1) * sizeof(Lobby));
    lobbyCount--;
}

// if leave, lobbyId = -1, create -2
void join_lobby(int client, uint32_t lobbyId) {
    Player* player = &players[client];

    Lobby *lobby;
    if (lobbyId == -1) lobby = new_lobby();
    else lobby = get_lobby(lobbyId);

    if (lobby == NULL || lobby->playerCount == MAX_CLIENTS) { // TODO reconnect
        // TODO respond fail
        return;
    }

    if (player->lobby != NULL) {
        if (player->lobby->id == lobbyId) {
            // TODO respond fail
            return;
        } else {
            // TODO if a player and not a spec
            int lobbyPlayerIndex = get_lobby_player_index(player);

            memmove(lobby->players + lobbyPlayerIndex, lobby->players + lobbyPlayerIndex + 1, (lobby->playerCount - lobbyPlayerIndex - 1) * sizeof(Player*));
            lobby->playerCount--;

            if (lobby->playerCount == 0) {
                del_lobby(lobby);
                // int lobbyIndex = get_lobby_index(lobby);
                // memmove(lobbies + lobbyIndex, lobbies + lobbyIndex + 1, (lobbyCount - lobbyIndex - 1) * sizeof(Lobby));
                // lobbyCount--;
            } else {
                LobbyPlayerLeavePacket onLeavePacket;
                onLeavePacket.playerIndex = lobbyPlayerIndex;
                Buffer buffer = serialize_LobbyPlayerJoinPacket(&onLeavePacket);
                for (size_t i = 0; i < lobby->playerCount; i++) send_to(server.clients[get_player_index(lobby->players[i])], &buffer);
            }
        }
    }

    if (lobbyId == -1 || player->lobby == 0) {
        disconnect_client(&server, client);
        return;
    }

    player->lobby = lobby;
    lobby->players[lobby->playerCount++] = player;

    {
        LobbyJoinAckPacket lobbyPacket;
        lobbyPacket.lobby = to_lobby_display(lobby);
        Buffer buffer = serialize_LobbyJoinAckPacket(&lobbyPacket);
        send_to(server.clients[client], &buffer);
    }
    {
        LobbyPlayerJoinPacket onJoinPacket;
        onJoinPacket.player = to_player_display(player);
        Buffer buffer = serialize_LobbyPlayerJoinPacket(&onJoinPacket);

        for (size_t i = 0; i < lobby->playerCount; i++) {
            if (lobby->players[i] != player) send_to(server.clients[get_player_index(lobby->players[i])], &buffer);
        }
    }
}

void on_connection(int client, ConnectionPacket packet) {
    Player* player = &players[client];
    if (player->status != CONNECTING) return;

    // On rajoute le nouveau joueur à la liste players
    uint32_t lobbyId = 0;
    if (packet.id == 0) {
        printf("%s joined\r\n",packet.player.name); 
        player->id = new_player_id(&packet.player);
        strcpy(player->name, packet.player.name);
        player->status = IDLE;
        player->lobby = NULL;
    } else {
        // player->id = packet.player.id;
        // printf("%s reconnected\r\n", packet.player.name);
        // for (size_t i = 0; i < lobbyCount; i++) {
        //     if (get_game_player_index(&lobbies[i], player->id) == -1) continue;
        //     player->lobby = &lobbies[i];
        //     break;
        // }
        // player->status = player->lobby == NULL ? IDLE : PLAYING;
    }

    ConnectionAckPacket response;
    response.id = player->id;
    Buffer buffer = serialize_ConnectionAckPacket(&response);
    send_to(server.clients[client], &buffer);

    join_lobby(client, lobbyId);

    // if (player->status != PLAYING) {
    //     LobbyJoinPacket lobbyPacket;
    //     lobbyPacket.lobby = to_lobby_display(player->lobby);
    //     Buffer buffer = serialize_LobbyJoinPacket(&lobbyPacket);
    //     send_to(server.clients[client], &buffer);
    // } else {
    //     int gIndex = get_game_index(player->lobby);
    //     int playerIndex = get_game_player_index(&lobbies[gIndex], player->id);
    //     AwaleReconnectPacket packet = {
    //         lobbies[gIndex].awale,
    //         *lobbies[gIndex].players[1-playerIndex],
    //         playerIndex};
    //     Buffer buffer = serialize_AwaleReconnectPacket(&packet);
    //     send_to(server.clients[client], &buffer);
    // }
}

void send_user_names_list(int numClientToSend){
    AnswerUsernamesListPacket packet;
    packet.count = server.clientCount;
    for(int i=0; i<server.clientCount; i++) packet.players[i] = players[i];
    Buffer buffer = serialize_AnswerUsernamesListPacket(&packet);
    send_to(server.clients[numClientToSend], &buffer);
}

void sync_game(Lobby* game) {
    AwaleSyncPacket packet = {game->awale};
    Buffer buffer = serialize_AwaleSyncPacket(&packet);
    for (size_t i = 0; i < PLAYER_COUNT; i++) send_to(server.clients[get_num_client_by_idClient(game->players[i]->id)], &buffer);
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

            int index = create_lobby();
            if (index != -1) {
                lobbies[index].players[0] = &players[requesterClient];
                players[requesterClient].lobby = &lobbies[index];
                players[requesterClient].status = PLAYING;
                lobbies[index].players[1] = &players[client];
                players[client].lobby = &lobbies[index];
                players[client].status = PLAYING;
                sync_game(&lobbies[index]);
                printf("Started a game (%d) between %s (%d) and %s (%d)\r\n", lobbies[index].id, players[requesterClient].name, players[requesterClient].id, players[client].name, players[client].id);
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
    int gIndex = get_game_index(players[client].lobby);
    PlayResult res = play(&lobbies[gIndex].awale, get_game_player_index(&lobbies[gIndex], players[client].id), packet.cell);

    AwalePlayAckPacket response = {res};
    Buffer buffer = serialize_AwalePlayAckPacket(&response);
    send_to(server.clients[client], &buffer);
    if (res != PLAYED) return;

    sync_game(&lobbies[gIndex]);

    if (lobbies[gIndex].awale.state < 0) return;
    printf("Game %d finish with state %d\r\n", players[client].lobby->id, lobbies[gIndex].awale.state);
    for (size_t i = 0; i < PLAYER_COUNT; i++) {
        int pIndex = get_num_client_by_idClient(lobbies[gIndex].players[i]->id);
        players[pIndex].status = IDLE;
        players[pIndex].lobby = NULL;
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
    nextLobbyId = 1;
    lobbyCount = 0;
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
            players[server.clientCount - 1] = new_player();
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
