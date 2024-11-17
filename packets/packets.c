#include "packets.h"
#include "string.h"

Buffer serialize_ConnectionPacket(ConnectionPacket *packet)
{
    Buffer buffer = new_buffer();
    serialize_uint8(&buffer, PACKET_CONNECTION);
    serialize_str(&buffer, packet->name);
    return buffer;
}

ConnectionPacket deserialize_ConnectionPacket(Buffer* buffer) {
    ConnectionPacket packet;
    deserialize_uint8(buffer);
    deserialize_str(buffer, packet.name);
    return packet;
}

Buffer serialize_ChatPacket(ChatPacket* packet) {
    Buffer buffer = new_buffer();
    serialize_uint8(&buffer, PACKET_CHAT);
    serialize_uint8(&buffer, packet->sender);
    serialize_str(&buffer, packet->message);
    return buffer;
}

ChatPacket deserialize_ChatPacket(Buffer* buffer) {
    ChatPacket packet;
    deserialize_uint8(buffer);
    packet.sender = deserialize_uint8(buffer);
    deserialize_str(buffer, packet.message);
    return packet;
}

Buffer serialize_RequestUsernamesListPacket(RequestUsernamesListPacket* packet){
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
    serialize_uint32(&buffer, packet->nbPlayers);
    for(int i = 0; i<packet->nbPlayers; i++) serialize_str(&buffer, packet->playersNames[i]);
    return buffer;
}

AnswerUsernamesListPacket deserialize_AnswerUsernamesListPacket(Buffer* buffer){
    //TO DO : gestion des dépassements
    AnswerUsernamesListPacket packet;
    deserialize_uint8(buffer);
    packet.nbPlayers = deserialize_uint32(buffer);
    for(int i = 0; i<packet.nbPlayers; i++) deserialize_str(buffer, packet.playersNames[i]);
    return packet;
}


Buffer serialize_ChallengeInDuelPacket(ChallengeInDuelPacket *packet) {
    Buffer buffer = new_buffer();
    serialize_uint8(&buffer, PACKET_CHALLENGE_IN_DUEL);
    serialize_str(&buffer, packet->requesterName);
    serialize_str(&buffer, packet->opponentName);
    serialize_uint8(&buffer, packet->etat);
    return buffer; 
}

ChallengeInDuelPacket deserialize_ChallengeInDuelPacket(Buffer* buffer) {
    ChallengeInDuelPacket packet;
    deserialize_uint8(&buffer);
    deserialize_str(&buffer, packet.requesterName);
    deserialize_str(&buffer, packet.opponentName);
    packet.etat = deserialize_uint8(&buffer);
    return packet;
}



