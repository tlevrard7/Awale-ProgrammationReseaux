#pragma once

#include "network.h"

SOCKET create_client(const char *address, int port);
void close_client(SOCKET client);

void receive_from(SOCKET client, void on_disconnect(SOCKET client), void on_receive(SOCKET client, Buffer* buffer));
