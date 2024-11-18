#pragma once

#include "network.h"

SOCKET create_client(const char *address, int port);
void close_client(SOCKET client);

void fd_set_client(SOCKET client, fd_set *rdfs);
int fd_is_set_client(SOCKET client, fd_set *rdfs);

Buffer receive_client(SOCKET client);