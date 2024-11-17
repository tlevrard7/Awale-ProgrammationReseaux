#ifdef linux

#include <sys/types.h>

#endif

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

size_t serialize_ConnectionPacket(ConnectionPacket *packet, char *buffer);
ConnectionPacket deserialize_ConnectionPacket(char *buffer, size_t n);

typedef struct ChatPacket {
    char sender;
    char message[256];
} ChatPacket;

size_t serialize_ChatPacket(ChatPacket *packet, char *buffer);
ChatPacket deserialize_ChatPacket(char *buffer, size_t n);

typedef struct RequestUsernamesListPacket{
} RequestUsernamesListPacket;

size_t serialize_RequestUsernamesListPacket(RequestUsernamesListPacket *packet, char *buffer);
RequestUsernamesListPacket deserialize_RequestUsernamesListPacket(char *buffer, size_t n);

typedef struct AnswerUsernamesListPacket{
    char playersNames[MAX_CLIENTS][MAX_NAME_SIZE+1];
    int nbPlayers;
} AnswerUsernamesListPacket;

size_t serialize_AnswerUsernamesListPacket(AnswerUsernamesListPacket *packet, char *buffer);
AnswerUsernamesListPacket deserialize_AnswerUsernamesListPacket(char *buffer, size_t n);

typedef struct ChallengeInDuelPacket{
    char requesterName[MAX_NAME_SIZE+1];
    char opponentName[MAX_NAME_SIZE+1];
    char etat; //'0' pour pas encore accepté, '1' pour accepté, '2' pour refusé
} ChallengeInDuelPacket;

size_t serialize_ChallengeInDuelPacket(ChallengeInDuelPacket *packet, char *buffer);
ChallengeInDuelPacket deserialize_ChallengeInDuelPacket(char *buffer, size_t n);




