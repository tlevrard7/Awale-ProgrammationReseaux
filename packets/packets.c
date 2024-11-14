#include "packets.h"
#include "string.h"

size_t serialize_ConnectionPacket(ConnectionPacket* packet, char *buffer) {
    buffer[0] = PACKET_CONNECTION;
    strcpy(buffer + 1, packet->name);
    return 1 + strlen(packet->name)+1;
}

ConnectionPacket deserialize_ConnectionPacket(char *buffer, size_t n) {
    ConnectionPacket packet;
    strcpy(packet.name, buffer+1);
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
