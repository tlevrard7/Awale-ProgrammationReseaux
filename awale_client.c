#include "packets/packets.h"
#include "network/client.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


void on_disconnected(SOCKET client) {

}

void print_usernames_list(AnswerUsernamesListPacket answerUsernamesListPacket){
    printf("Voici la liste des joueurs connectés :\n");
    for(int i=0; i<answerUsernamesListPacket.nbPlayers; i++){
        printf("%s\n", answerUsernamesListPacket.playersNames[i]);
    }
}

void send_request_usernames_list(SOCKET client){
    RequestUsernamesListPacket requestUsernamesListPacket;
    Buffer buffer = serialize_RequestUsernamesListPacket(&requestUsernamesListPacket);
    send_to(client, &buffer);
}

void on_receive(SOCKET client, Buffer* buffer) {
    switch (buffer->data[0]) {
    case PACKET_ANSWER_USER_NAMES_LIST:
        print_usernames_list(deserialize_AnswerUsernamesListPacket(buffer));
        break;
    }
}

int main(int argc, char **argv){
    
    if(argc != 4)
    {
        printf("Usage : %s [address] [port] [pseudo]\n", argv[0]);
        return 1;
    }

    char *address = argv[1];
    char *pseudo = argv[3];

    init_network();

    SOCKET client = create_client(address, atoi(argv[2]));

    ConnectionPacket packet;
    strcpy(packet.name, argv[3]);
    Buffer buffer = serialize_ConnectionPacket(&packet);
    send_to(client, &buffer); 

    // ConnectionPacket packet2 = {"Player 2"};
    // Buffer buffer2 = serialize_ConnectionPacket(&packet2);
    // send_to(client, &buffer2);

    while (1) {
        receive_from(client, on_disconnected, on_receive);
    }

    close_client(client);

    end_network();
}