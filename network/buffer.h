#pragma once

#include <stdint.h>

#define BUF_SIZE 1024

typedef struct Buffer {
    uint8_t data[BUF_SIZE];
    short size;
    short position;
} Buffer;

Buffer new_buffer();

void serialize_uint8 (Buffer* buffer, uint8_t value);
uint8_t deserialize_uint8  (Buffer* buffer);

void serialize_uint16(Buffer* buffer, uint16_t value);
uint16_t deserialize_uint16(Buffer* buffer);

void serialize_uint32(Buffer* buffer, uint32_t value);
uint32_t deserialize_uint32(Buffer* buffer);

void serialize_str(Buffer* buffer, const char* value);
void deserialize_str(Buffer* buffer, char *value);