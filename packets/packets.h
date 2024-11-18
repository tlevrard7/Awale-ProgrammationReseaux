#pragma once

#include "../network/network.h"
#include "../player.h"

#define MAX_NAME_SIZE 8 // Pour ne pas avoir à gérer le cas où le bufffer est trop petit pour contenir tous les pseudos

typedef enum PacketId {
    PACKET_CONNECTION,
    PACKET_CONNECTION_ACK,
    PACKET_CHAT,
    PACKET_REQUEST_USER_NAMES_LIST,
    PACKET_ANSWER_USER_NAMES_LIST,
    PACKET_CHALLENGE_IN_DUEL,
    PACKET_GAME_PACKET,
} PacketId;

typedef struct ConnectionPacket {
    Player player;
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

typedef struct ChallengeInDuelPacket{
    char requesterName[MAX_NAME_SIZE+1];
    char opponentName[MAX_NAME_SIZE+1];
    char etat; //'0' pour pas encore accepté, '1' pour accepté, '2' pour refusé
} ChallengeInDuelPacket;

Buffer serialize_ChallengeInDuelPacket(ChallengeInDuelPacket *packet);
ChallengeInDuelPacket deserialize_ChallengeInDuelPacket(Buffer* buffer);


