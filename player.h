#pragma once

#include "awale/awale.h"
#include "network/network.h"

typedef enum PlayerStatus {
    DISCONNECTED,
    CONNECTING,
    IDLE,
    CHALLENGED,
    PLAYING,
} PlayerStatus;

typedef struct Player {
    uint32_t id;
    PlayerStatus status;
    char name[256];
    uint32_t gameId;
} Player;

typedef struct Game {
    uint32_t id;
    uint32_t playerIds[PLAYER_COUNT];
    Awale awale;
} Game;
