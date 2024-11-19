#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "packets/packets.h"
#include "network/client.h"
#include "player.h"

SOCKET client;
Player localPlayer;

void process_challenge_in_duel_packet(ChallengeInDuelPacket packet){
    switch(packet.etat){
        case SENT:
            // On a été challengé
            char r;
            questionReponseDefi:
                printf("%s vous a défié\n",packet.requester.name);
                printf("Acceptez vous le challenge ? \n yes : y \n no : n ");
                scanf(" %c", &r); 
                if (r == 'y') {
                    packet.etat = ACCEPTED;
                } else if (r == 'n') {
                    packet.etat = REFUSED;
                } else {
                    printf("Entrée invalide. Veuillez recommencer\n");
                    goto questionReponseDefi; 
                }
            
            Buffer buffer = serialize_ChallengeInDuelPacket(&packet);
            send_to(client, &buffer);
            break;
        case OPPONENT_DOESNT_EXIST:
            // On a challengé qlq mais le serveur nous a dit qu'il n'existait pas
            printf("Vous avez défié %d mais ce joueur n'existe pas\n",packet.opponent.id);
            break;
        case ACCEPTED:
            printf("%s a accepté votre challenge ! La partie va commencer. \n",packet.opponent.name);
            // TO DO : Intégrer le début d'une partie
            break;
        case REFUSED:
            printf("%s a refusé votre challenge !\n",packet.opponent.name);
            break;
    }
}

void send_challenge_in_duel(int id_opponent){
    ChallengeInDuelPacket challengeInDuelPacket;
    Player requester = localPlayer; challengeInDuelPacket.requester = requester; 
    Player opponent; opponent.id = id_opponent; challengeInDuelPacket.opponent = opponent;
    challengeInDuelPacket.etat = SENT;
    Buffer buffer = serialize_ChallengeInDuelPacket(&challengeInDuelPacket);
    send_to(client, &buffer);
}


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
    else if(input[0] == 'c' || strstr(input,"challenge")){
        int id;
        questionDemandeDefi : 
            printf("Saisissez l'id du joueur que vous voulez défier' : \n");
            if (scanf(" %d", &id) != 1) {
                printf("Erreur : Saisie invalide.");
                goto questionDemandeDefi;
            }

        send_challenge_in_duel(id);
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
    localPlayer.id = 0;

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
                break;
            case PACKET_CHALLENGE_IN_DUEL:
                process_challenge_in_duel_packet(deserialize_ChallengeInDuelPacket(&buffer));
                break;
            }
        } 
    }  

    close_client(client);

    end_network();
}