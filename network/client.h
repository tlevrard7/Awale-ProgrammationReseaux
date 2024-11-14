#pragma once

#include "network.h"

SOCKET create_client(const char *address, int port);
void close_client(SOCKET client);

ssize_t receive_from(SOCKET client, char *buffer);
