#include "network.h"
#include <stdio.h>
#include <string.h>
#include "buffer.h"

Buffer new_buffer() {
    Buffer buffer;
    buffer.size = 0;
    buffer.position = 0;
    return buffer;
}

void on_serialize(Buffer *buffer, short count) {
    buffer->position += count;
    if (buffer->position > buffer->size) buffer->size = buffer->position;
}
void on_deserialize(Buffer *buffer, short count) {
    buffer->position += count;
}

void serialize_uint8(Buffer *buffer, uint8_t value) {
    buffer->data[buffer->position] = value;
    on_serialize(buffer, sizeof(uint8_t));
}
uint8_t deserialize_uint8(Buffer *buffer) {
    uint8_t value = buffer->data[buffer->position];
    on_deserialize(buffer, sizeof(uint8_t));
    return value;
}

void serialize_uint16(Buffer *buffer, uint16_t value) {
    *((uint16_t *)(buffer->data + buffer->position)) = htons(value);
    on_serialize(buffer, sizeof(u_int16_t));
}
uint16_t deserialize_uint16(Buffer *buffer) {
    uint16_t value = ntohs(*((uint16_t *)(buffer->data + buffer->position)));
    on_deserialize(buffer, sizeof(u_int16_t));
    return value;
}

void serialize_uint32(Buffer *buffer, uint32_t value) {
    *((uint32_t *)(buffer->data + buffer->position)) = htonl(value);
    on_serialize(buffer, sizeof(uint32_t));
}
uint32_t deserialize_uint32(Buffer *buffer) {
    uint32_t value = ntohl(*((uint32_t *)(buffer->data + buffer->position)));
    on_deserialize(buffer, sizeof(uint32_t));
    return value;
}

void serialize_str(Buffer* buffer, const char* value){
    size_t len = strlen(value);
    strcpy((char*)(buffer->data + buffer->position), value);
    buffer->data[buffer->position + len] = '\0';
    on_serialize(buffer, len + 1);
}

void deserialize_str(Buffer* buffer, char* value) {
    strcpy(value, (char *)(buffer->data + buffer->position));
    size_t len = strlen(value);
    on_deserialize(buffer, len + 1);
}
