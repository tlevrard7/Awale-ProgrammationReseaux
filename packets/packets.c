#include "packets.h"
#include "string.h"

void serialize_player(Buffer* buffer, Player* player){
    serialize_uint32(buffer, player->id);
    serialize_str(buffer, player->name);
}

Player deserialize_player(Buffer* buffer) {
    Player player;
    player.id = deserialize_uint32(buffer);
    deserialize_str(buffer, player.name);
    return player;
}

Buffer serialize_ConnectionPacket(ConnectionPacket *packet) {
    Buffer buffer = new_buffer();
    serialize_uint8(&buffer, PACKET_CONNECTION);
    serialize_player(&buffer, &packet->player);
    return buffer;
}

ConnectionPacket deserialize_ConnectionPacket(Buffer* buffer) {
    ConnectionPacket packet;
    deserialize_uint8(buffer);
    packet.player = deserialize_player(buffer);
    return packet;
}

Buffer serialize_ConnectionAckPacket(ConnectionAckPacket* packet) {
    Buffer buffer = new_buffer();
    serialize_uint8(&buffer, PACKET_CONNECTION_ACK);
    serialize_uint32(&buffer, packet->id);
    return buffer;
}

ConnectionAckPacket deserialize_ConnectionAckPacket(Buffer* buffer) {
    ConnectionAckPacket packet;
    deserialize_uint8(buffer);
    packet.id = deserialize_uint32(buffer);
    return packet;
}

Buffer serialize_ChatPacket(ChatPacket* packet) {
    Buffer buffer = new_buffer();
    serialize_uint8(&buffer, PACKET_CHAT);
    serialize_player(&buffer, &packet->sender);
    serialize_player(&buffer, &packet->receiver);
    serialize_str(&buffer, packet->message);
    return buffer;
}

ChatPacket deserialize_ChatPacket(Buffer* buffer) {
    ChatPacket packet;
    deserialize_uint8(buffer);
    packet.sender = deserialize_player(buffer);
    packet.receiver = deserialize_player(buffer);
    deserialize_str(buffer, packet.message);
    return packet;
}

Buffer serialize_RequestUsernamesListPacket(RequestUsernamesListPacket* packet){
    (void)packet;
    Buffer buffer = new_buffer();
    serialize_uint8(&buffer, PACKET_REQUEST_USER_NAMES_LIST);
    return buffer;
}

RequestUsernamesListPacket deserialize_RequestUsernamesListPacket(Buffer* buffer){
    RequestUsernamesListPacket packet;
    deserialize_uint8(buffer);
    return packet;
}

Buffer serialize_AnswerUsernamesListPacket(AnswerUsernamesListPacket* packet){
    //TO DO : gestion des dépassements
    Buffer buffer = new_buffer();
    serialize_uint8(&buffer, PACKET_ANSWER_USER_NAMES_LIST);
    serialize_uint32(&buffer, packet->count);
    for(size_t i = 0; i<packet->count; i++) serialize_player(&buffer, &packet->players[i]);
    return buffer;
}

AnswerUsernamesListPacket deserialize_AnswerUsernamesListPacket(Buffer* buffer){
    //TO DO : gestion des dépassements
    AnswerUsernamesListPacket packet;
    deserialize_uint8(buffer);
    packet.count = deserialize_uint32(buffer);
    for(size_t i = 0; i < packet.count; i++) packet.players[i] = deserialize_player(buffer);
    return packet;
}

Buffer serialize_RequestGamesListPacket(RequestGamesListPacket* packet){
    (void)packet;
    Buffer buffer = new_buffer();
    serialize_uint8(&buffer, PACKET_REQUEST_GAMES_LIST);
    return buffer;
}

RequestGamesListPacket deserialize_RequestGamesListPacket(Buffer* buffer){
    RequestGamesListPacket packet;
    deserialize_uint8(buffer);
    return packet;
}

void serialize_awale(Buffer* buffer, Awale* awale);
void deserialize_awale(Buffer* buffer, Awale* awale);

void serialize_game(Buffer* buffer, Game* game){
    serialize_uint32(buffer, game->id);
    for(int i=0; i<PLAYER_COUNT; i++)  serialize_uint32(buffer, game->playerIds[i]);
    serialize_awale(buffer, &game->awale);
}

Game deserialize_game(Buffer* buffer) {
    Game game;
    game.id = deserialize_uint32(buffer);
    for(int i=0; i<PLAYER_COUNT; i++)  game.playerIds[i] = deserialize_uint32(buffer);
    deserialize_awale(buffer, &game.awale);
    return game;
}

Buffer serialize_AnswerGamesListPacket(AnswerGamesListPacket* packet){
    //TO DO : gestion des dépassements
    Buffer buffer = new_buffer();
    serialize_uint8(&buffer, PACKET_ANSWER_GAMES_LIST);
    serialize_uint32(&buffer, packet->nbGames);
    for(size_t i = 0; i<packet->nbGames; i++) serialize_game(&buffer, &packet->games[i]);
    for(size_t i = 0; i<packet->nbGames; i++){
        for(size_t j = 0; j<PLAYER_COUNT; j++){
            serialize_player(&buffer, &packet->players[i][j]);
        }
    }
    return buffer;
}

AnswerGamesListPacket deserialize_AnswerGamesListPacket(Buffer* buffer){
    //TO DO : gestion des dépassements
    AnswerGamesListPacket packet;
    deserialize_uint8(buffer);
    packet.nbGames = deserialize_uint32(buffer);
    for(size_t i = 0; i < packet.nbGames; i++) packet.games[i] = deserialize_game(buffer);
    for(size_t i = 0; i<packet.nbGames; i++){
        for(size_t j = 0; j<PLAYER_COUNT; j++){
            packet.players[i][j] = deserialize_player(buffer);
        }
    }
    return packet;
}


Buffer serialize_ChallengeInDuelPacket(ChallengeInDuelPacket *packet) {
    Buffer buffer = new_buffer();
    serialize_uint8(&buffer, PACKET_CHALLENGE_IN_DUEL);
    serialize_player(&buffer, &packet->requester);
    serialize_player(&buffer, &packet->opponent);
    serialize_uint8(&buffer, packet->etat);
    return buffer; 
}

ChallengeInDuelPacket deserialize_ChallengeInDuelPacket(Buffer* buffer) {
    ChallengeInDuelPacket packet;
    deserialize_uint8(buffer);
    packet.requester = deserialize_player(buffer);
    packet.opponent = deserialize_player(buffer);
    packet.etat = deserialize_uint8(buffer);
    return packet;
}

void serialize_awale(Buffer* buffer, Awale* awale){
    serialize_uint32(buffer, awale->state);
    for(int i=0;i<CELL_COUNT;i++) serialize_uint32(buffer, awale->cells[i]);
    serialize_uint32(buffer, awale->turn);
    for(int i=0;i<PLAYER_COUNT;i++) serialize_uint32(buffer, awale->score[i]);
}

Buffer serialize_AwaleSyncPacket(AwaleSyncPacket *packet) {
    Buffer buffer = new_buffer();
    serialize_uint8(&buffer, PACKET_AWALE_SYNC);
    serialize_awale(&buffer, &packet->awale);
    return buffer;
}

void deserialize_awale(Buffer* buffer, Awale* awale){
    awale->state = deserialize_uint32(buffer);
    for(int i=0;i<CELL_COUNT;i++) awale->cells[i] = deserialize_uint32(buffer);
    awale->turn = deserialize_uint32(buffer);
    for(int i=0;i<PLAYER_COUNT;i++) awale->score[i] = deserialize_uint32(buffer);
}

AwaleSyncPacket deserialize_AwaleSyncPacket(Buffer *buffer) {
    AwaleSyncPacket packet;
    deserialize_uint8(buffer);
    deserialize_awale(buffer, &packet.awale);
    return packet;
}

Buffer serialize_AwalePlayPacket(AwalePlayPacket *packet) {
    Buffer buffer = new_buffer();
    serialize_uint8(&buffer, PACKET_AWALE_PLAY);
    serialize_uint8(&buffer, packet->cell);
    return buffer;
}

AwalePlayPacket deserialize_AwalePlayPacket(Buffer *buffer) {
    AwalePlayPacket packet;
    deserialize_uint8(buffer);
    packet.cell = deserialize_uint8(buffer);
    return packet;
}

Buffer serialize_AwalePlayAckPacket(AwalePlayAckPacket *packet) {
    Buffer buffer = new_buffer();
    serialize_uint8(&buffer, PACKET_AWALE_PLAY_ACK);
    serialize_uint8(&buffer, packet->result);
    return buffer;
}

AwalePlayAckPacket deserialize_AwalePlayAckPacket(Buffer *buffer) {
    AwalePlayAckPacket packet;
    deserialize_uint8(buffer);
    packet.result = deserialize_uint8(buffer);
    return packet;
}

Buffer serialize_AwaleReconnectPacket(AwaleReconnectPacket *packet) {
    Buffer buffer = new_buffer();
    serialize_uint8(&buffer, PACKET_AWALE_RECONNECT);
    serialize_awale(&buffer, &packet->awale);
    serialize_player(&buffer, &packet->opponent);
    serialize_uint32(&buffer, packet->playerIndex);
    return buffer;
}

AwaleReconnectPacket deserialize_AwaleReconnectPacket(Buffer *buffer) {
    AwaleReconnectPacket packet;
    deserialize_uint8(buffer);
    deserialize_awale(buffer, &packet.awale);
    packet.opponent = deserialize_player(buffer);
    packet.playerIndex = deserialize_uint32(buffer);
    return packet;
}
