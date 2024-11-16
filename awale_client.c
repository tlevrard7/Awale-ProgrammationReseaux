#include "packets/packets.h"
#include "network/client.h"

void on_disconnected(SOCKET client) {

}

void on_receive(SOCKET client, char *buffer, size_t n) {

}

int main() {
    init_network();

    SOCKET client = create_client("localhost", 2025);

    ConnectionPacket connectionPacket = {"P1"};
    char buffer[BUF_SIZE];
    int n = serialize_ConnectionPacket(&connectionPacket, buffer);
    send_to(client, buffer, n);

    while (1) {
        receive_from(client, on_disconnected, on_receive);
    }

    close_client(client);

    end_network();
}