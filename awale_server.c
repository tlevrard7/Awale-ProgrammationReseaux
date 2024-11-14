#include "network/server.h"
#include "packets/packets.h"
#include "stdio.h"

void on_connection(Server* server, SOCKET newClient) {

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

    SOCKET newClient;
    char buffer[BUF_SIZE];
    int n;
    int recvFrom;
    while (1) {
        if (accept_connection(&server, &newClient)) on_connection(&server, newClient);
        if ((n = receive_any(&server, &recvFrom, buffer)) > 0) on_receive(&server, recvFrom, buffer, n);
    }

    close_server(&server);

    end_network();
}
