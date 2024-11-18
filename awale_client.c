#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "packets/packets.h"
#include "network/client.h"
#include "player.h"

Player localPlayer;

void send_request_usernames_list(SOCKET client){
    RequestUsernamesListPacket requestUsernamesListPacket;
    Buffer buffer = serialize_RequestUsernamesListPacket(&requestUsernamesListPacket);
    send_to(client, &buffer);
}

void print_usernames_list(AnswerUsernamesListPacket answerUsernamesListPacket){
    printf("Voici la liste des joueurs connectés :\n");
    for(int i=0; i<answerUsernamesListPacket.nbPlayers; i++){
        printf(" - %s\n", answerUsernamesListPacket.playersNames[i]);
    }
}

void on_stdin(char* input, SOCKET client) {
    (void)input;
    if(strstr(input,"list_players")){
        send_request_usernames_list(client);
    }
}

void on_connection_ack(ConnectionAckPacket packet) {
    localPlayer.status = IN_LOBBY;
    localPlayer.id = packet.id;
}

// void on_player_list(PlayerListPacket packet) {

// }

void on_disconnected() {
    localPlayer.status = DISCONNECTED;
}

// void on_receive(SOCKET client, Buffer* buffer) {
//     switch (buffer->data[0]) {
//     case PACKET_ANSWER_USER_NAMES_LIST:
//         print_usernames_list(deserialize_AnswerUsernamesListPacket(buffer));
//         break;
//     }
// }

int main(int argc, char **argv){
    
    if(argc != 4)
    {
        printf("Usage : %s [address] [port] [pseudo]\n", argv[0]);
        return 1;
    }

    init_network();
    strcpy(localPlayer.name, argv[3]);
    localPlayer.id = -1;

    SOCKET client = create_client(argv[1], atoi(argv[2]));
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
            on_stdin(buffer, client);
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
            // case PACKET_PLAYER_LIST:
            //     on_player_list(deserialize_PlayerListPacket(&buffer));
            //     break;
            }
        }
    }

    close_client(client);

    end_network();
}