#include "network/server.h"
#include "packets/packets.h"
#include "stdio.h"

void on_connection(Server* server, SOCKET newClient) {
    printf("a new socket connected\n");
}

void on_receive(Server *server, int recvFrom, char *buffer, size_t n) {
    switch (buffer[0]) {
    case PACKET_CONNECTION:
        ConnectionPacket packet = deserialize_ConnectionPacket(buffer, n);
        printf("%s joined\n", packet.name);
        break;
    }   
}

int main() {
    init_network();

    Server server = create_server(2025);

    while (1) {
        SOCKET newClient;
        char buffer[BUF_SIZE];
        if (accept_connection(&server, &newClient)) on_connection(&server, newClient);
        int n;
        int recvFrom;
        if ((n = receive_any(&server, &recvFrom, buffer)) > 0) on_receive(&server, recvFrom, buffer, n);
    }

    close_server(&server);

    end_network();
}
