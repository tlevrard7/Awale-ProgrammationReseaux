#include "network/server.h"
#include "packets/packets.h"
#include "stdio.h"

void on_connection(Server *server, int client) { }

void on_connected(Server *server, int client, ConnectionPacket packet) {
    printf("%s joined\n", packet.name);
    on_connection(server, client);
}


void on_receive(Server *server, int recvFrom, char *buffer, size_t n) {
    switch (buffer[0]) {
    case PACKET_CONNECTION:
        on_connected(server, recvFrom, deserialize_ConnectionPacket(buffer, n));
        break;
    }   
}

void on_disconnect(Server* server, int client) {

}

int players[MAX_CLIENTS];

int main() {
    init_network();

    Server server = create_server(2025);
    while (1) {
        accept_connection(&server, on_connection);
        receive_any(&server, on_disconnect, on_receive);
    }

    close_server(&server);

    end_network();
}
