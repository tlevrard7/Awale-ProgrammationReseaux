#ifndef CLIENT_H
#define CLIENT_H

#include "../network.h"

static void app(const char *address, const char *name);
static int init_connection(const char *address);
static int read_server(SOCKET sock, char *buffer);
static void write_server(SOCKET sock, const char *buffer);

#endif /* guard */
