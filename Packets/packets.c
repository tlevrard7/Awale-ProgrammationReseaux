#include "packets.h"
#include "string.h"

ConnectionPacket buffer_to_ConnectionPacket(char *buffer) {
    ConnectionPacket packet;
    strcpy(packet.name, buffer+1);
    return packet;
}

void ConnectionPacket_to_buffer(ConnectionPacket* packet, char *buffer) {
    buffer[0] = PACKET_CONNECTION;
    strcpy(buffer + 1, packet->name);
}