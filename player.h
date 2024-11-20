#pragma once

#include "awale/awale.h"
#include "network/network.h"

typedef enum PlayerStatus {
    DISCONNECTED,
    CONNECTING,
    IDLE,
    PLAYING,
} PlayerStatus;

typedef struct Lobby Lobby;

typedef struct Player {
    uint32_t id;
    char name[256];
    PlayerStatus status;
    Lobby* lobby;
} Player;

Player new_player();
