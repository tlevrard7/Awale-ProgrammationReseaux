#include "packets.h"
#include "string.h"


size_t serialize_ConnectionPacket(ConnectionPacket* packet, char *buffer) {
    buffer[0] = PACKET_CONNECTION;
    strcpy(buffer + 1, packet->name);
    return 1 + strlen(packet->name)+1;
}

ConnectionPacket deserialize_ConnectionPacket(char *buffer, size_t n) {
    ConnectionPacket packet;
    strncpy(packet.name, buffer + 1, MAX_NAME_SIZE - 1);
    return packet;
}

size_t serialize_ChatPacket(ChatPacket* packet, char *buffer) {
    buffer[0] = PACKET_CHAT;
    buffer[1] = packet->sender;
    strcpy(buffer+2, packet->message);
    return 2 + strlen(packet->message)+1;
}

ChatPacket deserialize_ChatPacket(char *buffer, size_t n) {
    ChatPacket packet;
    packet.sender = buffer[1];
    strcpy(packet.message, buffer + 2);
    return packet;
}

size_t serialize_RequestUsernamesListPacket(RequestUsernamesListPacket *packet, char *buffer){
    buffer[0] = PACKET_REQUEST_USER_NAMES_LIST;
    return 1;
}

RequestUsernamesListPacket deserialize_RequestUsernamesListPacket(char *buffer, size_t n){
    RequestUsernamesListPacket packet;
    return packet;
}

size_t serialize_AnswerUsernamesListPacket(AnswerUsernamesListPacket *packet, char *buffer){
    //TO DO : gestion des dépassements
    buffer[0] = PACKET_ANSWER_USER_NAMES_LIST;
    
    int nbCaractTot = 0;
    // On concatène les str du char** pour former un seul char*
    buffer[1] = '\0';
    for(int i = 0; i<packet->nbPlayers; i++){
        nbCaractTot += strlen(packet->playersNames[i]);
        strcat(buffer,packet->playersNames[i]);
        if (i < packet->nbPlayers - 1) {
            strcat(buffer, "-");
            nbCaractTot += 1;
        }
    }
    return 1 + nbCaractTot + 1;
}

AnswerUsernamesListPacket deserialize_AnswerUsernamesListPacket(char *buffer, size_t n){
    //TO DO : gestion des dépassements
    AnswerUsernamesListPacket packet;

    int currentNumPlayer = 0;
    char currentName[MAX_NAME_SIZE]; int currentCursor = 0; 
    for(int i=0; i<BUF_SIZE; i++){
        char currentChar = buffer[i];
        if(currentChar=='-'){
            currentName[currentCursor] = '\0';
            strcpy(packet.playersNames[currentNumPlayer], currentName);
            packet.playersNames[currentNumPlayer][MAX_NAME_SIZE - 1] = '\0';
            currentNumPlayer++; currentCursor = 0;
            continue;
        }
        currentName[currentCursor] = currentChar;
        currentCursor++;
    }
    packet.nbPlayers = currentNumPlayer;

    return packet;
}


size_t serialize_ChallengeInDuelPacket(ChallengeInDuelPacket *packet, char *buffer) {
    buffer[0] = PACKET_CHALLENGE_IN_DUEL;
    buffer[1] = '\0'; 

    strcat(buffer, packet->requesterName); strcat(buffer, "-");
    strcat(buffer, packet->opponentName);  strcat(buffer, "-");
    char str[1]; str[0] = packet->etat;
    strncat(buffer, str, 1);  strcat(buffer, "-");

    return 3 + strlen(packet->requesterName) + strlen(packet->opponentName) + 1 + 1; 
}

ChallengeInDuelPacket deserialize_ChallengeInDuelPacket(char *buffer, size_t n) {
    ChallengeInDuelPacket packet;

    char* data = buffer+1;
    // requesterName
    char *delimiter1 = strchr(data, '-');
    size_t requesterNameLength = delimiter1 - data;
    strncpy(packet.requesterName, data, requesterNameLength); packet.requesterName[requesterNameLength] = '\0';
    // opponentName
    char *delimiter2 = strchr(delimiter1 + 1, '-');
    size_t opponentNameLength = delimiter2 - (delimiter1 + 1);
    strncpy(packet.opponentName, delimiter1 + 1, opponentNameLength); packet.opponentName[opponentNameLength] = '\0'; 
    // etat
    packet.etat = delimiter2[1];  

    return packet;
}



