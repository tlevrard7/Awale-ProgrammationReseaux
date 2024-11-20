#pragma once

#include "../network/network.h"
#include "../player.h" // TODO remove
#include "../player_display.h"
#include "../awale/awale.h"

#define MAX_NAME_SIZE 8 // Pour ne pas avoir à gérer le cas où le bufffer est trop petit pour contenir tous les pseudos

typedef enum PacketId {
    PACKET_CONNECTION,
    PACKET_CONNECTION_ACK,
    PACKET_CHAT,
    PACKET_REQUEST_USER_NAMES_LIST,
    PACKET_ANSWER_USER_NAMES_LIST,
    PACKET_CHALLENGE_IN_DUEL,
    PACKET_AWALE_SYNC,
    PACKET_AWALE_PLAY,
    PACKET_AWALE_PLAY_ACK,
    PACKET_AWALE_RECONNECT,
} PacketId;

typedef struct ConnectionPacket {
    uint32_t id;
    PlayerDisplay player;
} ConnectionPacket;

Buffer serialize_ConnectionPacket(ConnectionPacket* packet);
ConnectionPacket deserialize_ConnectionPacket(Buffer* buffer);

typedef struct ConnectionAckPacket {
    uint32_t id;
} ConnectionAckPacket;

Buffer serialize_ConnectionAckPacket(ConnectionAckPacket* packet);
ConnectionAckPacket deserialize_ConnectionAckPacket(Buffer* buffer);

typedef struct ChatPacket {
    char sender;
    char message[256];
} ChatPacket;

Buffer serialize_ChatPacket(ChatPacket *packet);
ChatPacket deserialize_ChatPacket(Buffer* buffer);

typedef struct RequestUsernamesListPacket{
} RequestUsernamesListPacket;

Buffer serialize_RequestUsernamesListPacket(RequestUsernamesListPacket *packet);
RequestUsernamesListPacket deserialize_RequestUsernamesListPacket(Buffer* buffer);

typedef struct AnswerUsernamesListPacket{
    uint32_t count;
    Player players[MAX_CLIENTS];
} AnswerUsernamesListPacket;

Buffer serialize_AnswerUsernamesListPacket(AnswerUsernamesListPacket *packet);
AnswerUsernamesListPacket deserialize_AnswerUsernamesListPacket(Buffer* buffer);

typedef enum STATE_CHALLENGE{
    SENT, // On vient juste de l'envoyer pas encore de réponse
    OPPONENT_DOESNT_EXIST,
    ACCEPTED,
    REFUSED,
} STATE_CHALLENGE;

typedef struct ChallengeInDuelPacket{
    Player requester;
    Player opponent;
    STATE_CHALLENGE etat;
} ChallengeInDuelPacket;

Buffer serialize_ChallengeInDuelPacket(ChallengeInDuelPacket *packet);
ChallengeInDuelPacket deserialize_ChallengeInDuelPacket(Buffer* buffer);

typedef struct AwaleSyncPacket{
    Awale awale;
} AwaleSyncPacket;

Buffer serialize_AwaleSyncPacket(AwaleSyncPacket *packet);
AwaleSyncPacket deserialize_AwaleSyncPacket(Buffer* buffer);

typedef struct AwalePlayPacket{
    uint8_t cell;
} AwalePlayPacket;

Buffer serialize_AwalePlayPacket(AwalePlayPacket *packet);
AwalePlayPacket deserialize_AwalePlayPacket(Buffer* buffer);

typedef struct AwalePlayAckPacket{
    PlayResult result;
} AwalePlayAckPacket;

Buffer serialize_AwalePlayAckPacket(AwalePlayAckPacket *packet);
AwalePlayAckPacket deserialize_AwalePlayAckPacket(Buffer* buffer);

typedef struct AwaleReconnectPacket{
    Awale awale;
    Player opponent;
    int playerIndex;
} AwaleReconnectPacket;

Buffer serialize_AwaleReconnectPacket(AwaleReconnectPacket *packet);
AwaleReconnectPacket deserialize_AwaleReconnectPacket(Buffer* buffer);
