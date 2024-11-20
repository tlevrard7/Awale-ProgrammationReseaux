#pragma once

#include "awale/awale.h"
#include "network/network.h"

typedef struct PlayerDisplay {
    char name[256];
} PlayerDisplay;

typedef struct LobbyDisplay {
    uint32_t id;
    int playerCount;
    PlayerDisplay players[PLAYER_COUNT];
    int spectatorCount;
    PlayerDisplay spectators[MAX_CLIENTS];
    Awale awale;
} LobbyDisplay;
