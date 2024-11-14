#include "packets/packets.h"
#include "network/client.h"

void on_receive(SOCKET client, char *buffer) {

}

int main() {
    init_network();

    SOCKET client = create_client("localhost", 2025);

    ConnectionPacket connectionPacket = {"P1"};
    char buffer[BUF_SIZE];
    int n = serialize_ConnectionPacket(&connectionPacket, buffer);
    send_to(client, buffer, n);

    // while (1) {
    //     char buffer[BUF_SIZE];
    //     if (receive_from(client, &buffer)) on_receive(client, &buffer);
    // }

    close_client(client);

    end_network();
}