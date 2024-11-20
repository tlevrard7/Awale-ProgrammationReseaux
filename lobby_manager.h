#pragma once

#include "awale/awale.h"
#include "network/network.h"


#define MAX_LOBBIES 8
#define MAX_PLAYERS MAX_CLIENTS

typedef struct Player Player;

typedef struct Lobby {
    uint32_t id;
    int playerCount;
    Player *players[MAX_PLAYERS];
    Awale awale;
} Lobby;

typedef enum LobbyJoinResult {
    LJR_JOINED,
    LJR_INVALID_LOBBY,
    LJR_LOBBY_FULL,
    LJR_ALREADY_IN_LOBBY,
    
} LobbyJoinResult;

Lobby* open_lobby();
Lobby* get_lobby(uint32_t id);
void close_lobby(uint32_t id);

LobbyJoinResult join_lobby(Player* player, uint32_t lobbyId);
void leave_lobby(Player *player);

int get_lobby_player_index(Player *player);