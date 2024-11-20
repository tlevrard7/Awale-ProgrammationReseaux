#include "player.h"

Player new_player() {
    Player player;
    player.id = 0;
    player.status = CONNECTING;
    player.lobby = NULL;
    player.name[0] = '\0';
    return player;
}