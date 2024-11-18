#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "packets/packets.h"
#include "network/client.h"
#include "player.h"

SOCKET client;
Player localPlayer;

void send_request_usernames_list(){
    RequestUsernamesListPacket requestUsernamesListPacket;
    Buffer buffer = serialize_RequestUsernamesListPacket(&requestUsernamesListPacket);
    send_to(client, &buffer);
}

void print_usernames_list(AnswerUsernamesListPacket packet){
    if (localPlayer.status != IDLE) return;
    printf("Voici la liste des joueurs connectés :\n");
    for(size_t i = 0; i < packet.count; i++){
        printf(" - %s (%d)\r\n", packet.players[i].name, packet.players[i].id);
    }
}

void on_stdin(char* input) {
    if(input[0] == 'p' || strstr(input,"players")){
        send_request_usernames_list(client);
    }
}

void on_connection_ack(ConnectionAckPacket packet) {
    if (localPlayer.status != CONNECTING) return;
    localPlayer.status = IDLE;
    localPlayer.id = packet.id;
    send_request_usernames_list();
}

void on_disconnected() {
    localPlayer.status = DISCONNECTED;
}

int main(int argc, char **argv){
    
    if(argc != 4)
    {
        printf("Usage : %s [address] [port] [pseudo]\n", argv[0]);
        return 1;
    }

    init_network();
    strcpy(localPlayer.name, argv[3]);
    localPlayer.id = -1;

    client = create_client(argv[1], atoi(argv[2]));
    ConnectionPacket packet = {localPlayer};
    Buffer buffer = serialize_ConnectionPacket(&packet);
    send_to(client, &buffer);
    localPlayer.status = CONNECTING;

    while (1) {
        fd_set rdfs;
        FD_ZERO(&rdfs);
        fd_set_client(client, &rdfs);
        FD_SET(STDIN_FILENO, &rdfs);
        int n;
        if ((n = select(client+1, &rdfs, NULL, NULL, NULL)) == -1) {
            perror("select()");
            exit(errno);
        }
        if (FD_ISSET(STDIN_FILENO, &rdfs)) {
            char buffer[BUF_SIZE];
            fgets(buffer, BUF_SIZE - 1, stdin);
            on_stdin(buffer);
        } else {
            Buffer buffer = receive_client(client);
            if (buffer.size == 0) {
                on_disconnected();
                break;
            }
            switch (buffer.data[0]) {
            case PACKET_CONNECTION_ACK:
                on_connection_ack(deserialize_ConnectionAckPacket(&buffer));
                break;
            case PACKET_ANSWER_USER_NAMES_LIST:
                print_usernames_list(deserialize_AnswerUsernamesListPacket(&buffer));
            }
        }
    }

    close_client(client);

    end_network();
}