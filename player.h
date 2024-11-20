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


typedef struct Lobby Lobby;

typedef struct Player {
    uint32_t id;
    PlayerStatus status;
    char name[256];
    Lobby* lobby;
} Player;

struct Lobby {
    uint32_t id;
    Player* players[PLAYER_COUNT];
    Awale awale;
};
