#pragma once

#include "awale/awale.h"
#include "network/network.h"

typedef enum PlayerStatus {
    DISCONNECTED,
    CONNECTING,
    IN_LOBBY,
    CHALLENGED,
    PLAYING,
    SPECTATING,
} PlayerStatus;


typedef struct Player {
    PlayerStatus status;
    uint32_t id;
    char name[256];
} Player;