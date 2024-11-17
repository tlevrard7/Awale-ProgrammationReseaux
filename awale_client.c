#include "packets/packets.h"
#include "network/client.h"
#include <stdlib.h>
#include <unistd.h>


void on_disconnected(SOCKET client) {

}

void print_usernames_list(AnswerUsernamesListPacket answerUsernamesListPacket){
    printf("Voici la liste des joueurs connectés :\n");
    for(int i=0; i<answerUsernamesListPacket.nbPlayers; i++){
        printf("%s\n", answerUsernamesListPacket.playersNames[i]);
    }
}

void send_request_usernames_list(SOCKET client, char *buffer){
    RequestUsernamesListPacket requestUsernamesListPacket;
    int n = serialize_RequestUsernamesListPacket(&requestUsernamesListPacket, buffer);
    send_to(client, buffer, n);
}

void on_receive(SOCKET client, char *buffer, size_t n) {
    switch (buffer[0]) {
        case PACKET_ANSWER_USER_NAMES_LIST:
            print_usernames_list(deserialize_AnswerUsernamesListPacket(buffer, n));
            break;
        default:
            // entrée stdin

        }
}

int main(int argc, char **argv){
    
    if(argc != 4)
    {
        printf("Usage : %s [address] [port] [pseudo]\n", argv[0]);
        return 1;
    }

    char *address = argv[1];
    char *pseudo = argv[2];

    init_network();

    SOCKET client = create_client(address, atoi(argv[2]));

    char buffer[BUF_SIZE];
    ConnectionPacket connectionPacket = {"p1"};
    int n = serialize_ConnectionPacket(&connectionPacket, buffer);
    send_to(client, buffer, n);

    /*
    ConnectionPacket connectionPacket2 = {"p2"};
    n = serialize_ConnectionPacket(&connectionPacket2, buffer);
    send_to(client, buffer, n);

    send_request_usernames_list(client, buffer);
    */

    
    while (1) {
        receive_from(client, on_disconnected, on_receive);
    }

    close_client(client);

    end_network();
}