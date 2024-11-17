#pragma once

#include "../network/network.h" // Pour avoir MAX_CLIENTS

#define MAX_NAME_SIZE 8 // Pour ne pas avoir à gérer le cas où le bufffer est trop petit pour contenir tous les pseudos


enum PACKET_IDS {
    PACKET_CONNECTION = 1,
    PACKET_CHAT = 2,
    PACKET_REQUEST_USER_NAMES_LIST = 3,
    PACKET_ANSWER_USER_NAMES_LIST = 4,
    PACKET_CHALLENGE_IN_DUEL = 5,
    PACKET_GAME_PACKET = 6,
};

typedef struct ConnectionPacket {
    char name[MAX_NAME_SIZE];
} ConnectionPacket;

Buffer serialize_ConnectionPacket(ConnectionPacket* packet);
ConnectionPacket deserialize_ConnectionPacket(Buffer* buffer);

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
    char playersNames[MAX_CLIENTS][MAX_NAME_SIZE+1];
    int nbPlayers;
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




