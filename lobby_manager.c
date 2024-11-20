#include "player.h"
#include "lobby_manager.h"

size_t lobbyCount;
Lobby lobbies[MAX_LOBBIES];
u_int32_t nextId;

Lobby* open_lobby() {
    if (lobbyCount == MAX_LOBBIES) return NULL;
    Lobby* lobby = &lobbies[lobbyCount++];
    lobby->id = nextId++;
    lobby->playerCount = 0;
    lobby->awale = init_game();
    lobby->awale.state = STATE_DRAW;
    return lobby;
}

Lobby *get_lobby(uint32_t id) {
    for (size_t i = 0; i < lobbyCount; i++) {
        if (lobbies[i].id == id) return &lobbies[i];
    }
    return NULL;
}

void close_lobby(uint32_t id) {
    Lobby* lobby = get_lobby(id);
    for (size_t i = 0; i < lobby->playerCount; i++) lobby->players[i]->lobby = NULL;
}

LobbyJoinResult join_lobby(Player *player, uint32_t id) {
    if (player->lobby != NULL) return LJR_ALREADY_IN_LOBBY;

    Lobby* lobby = get_lobby(id);
    if (lobby == NULL) return LJR_INVALID_LOBBY;
    if (lobby->playerCount == MAX_PLAYERS) return LJR_LOBBY_FULL;
    lobby->players[lobby->playerCount++] = player;
    player->lobby = lobby;
    return LJR_JOINED;
}

void leave_lobby(Player *player) {
    if (player->lobby == NULL) return;

    Lobby* lobby = player->lobby;
    int index = get_lobby_player_index(player);
    memmove(lobby->players + index, lobby->players + index + 1, (lobby->playerCount - index - 1) * sizeof(Player*));
    lobby->playerCount--;
}

int get_lobby_player_index(Player *player) {
    Lobby* lobby = player->lobby;
    for (size_t i = 0; i < lobby->playerCount; i++){
        if (lobby->players[i] == player) return i;
    }
    return -1;
}
