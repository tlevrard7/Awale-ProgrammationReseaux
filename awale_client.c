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
            if (localPlayer.status != IDLE) {
                packet.etat = REFUSED;
            } else {
                // On a été challengé
                localPlayer.status = CHALLENGED;
                do {
                    char r;
                    printf("%s vous a défié\r\n",packet.requester.name);
                    printf("Acceptez vous le challenge ? \r\n yes : y \r\n no : n ");
                    scanf(" %c", &r); 
                    if (r == 'y') {
                        packet.etat = ACCEPTED;
                    } else if (r == 'n') {
                        packet.etat = REFUSED;
                    } else {
                        printf("Entrée invalide. Veuillez recommencer\r\n");
                        while (getchar() != '\n'); // Vide le buffer 
                        continue;
                    }
                } while (packet.etat == SENT);
            }
            Buffer buffer = serialize_ChallengeInDuelPacket(&packet);
            send_to(client, &buffer);
            localPlayer.status = packet.etat == ACCEPTED ? PLAYING : IDLE;
            break;
        case OPPONENT_DOESNT_EXIST:
            if (localPlayer.status != CHALLENGED) return;
            localPlayer.status = IDLE;
            // On a challengé qlq mais le serveur nous a dit qu'il n'existait pas
            printf("Vous avez défié %d mais ce joueur n'existe pas\r\n",packet.opponent.id);
            break;
        case ACCEPTED:
            if (localPlayer.status != CHALLENGED) return;
            localPlayer.status = PLAYING;
            printf("%s a accepté votre challenge ! La partie va commencer.\r\n",packet.opponent.name);
            break;
        case REFUSED:
            if (localPlayer.status != CHALLENGED) return;
            localPlayer.status = IDLE;

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
    localPlayer.status = CHALLENGED;
}

void send_request_usernames_list(){
    RequestUsernamesListPacket requestUsernamesListPacket;
    Buffer buffer = serialize_RequestUsernamesListPacket(&requestUsernamesListPacket);
    send_to(client, &buffer);
}

void print_usernames_list(AnswerUsernamesListPacket packet){
    if (localPlayer.status != IDLE) return;
    printf("Voici la liste des joueurs connectés :\r\n");
    for(size_t i = 0; i < packet.count; i++){
        printf(" - %s (%d)\r\n", packet.players[i].name, packet.players[i].id);
    }
}

void on_awale_play_ack(AwalePlayAckPacket packet) {
    if (localPlayer.status != PLAYING || packet.result == PLAYED) return;
    switch (packet.result) {
    case WRONG_TURN:
        printf("Ce n'est pas votre tour de jouer\r\n");
        break;
    case INVALID_CELL:
    case INVALID_EMPTY:
        printf("Veuillez choisir une cellule valide\r\n");
        break;
    case INVALID_STARVATION:
        printf("Ce coup causerait une famine et ne peut pas être joué\r\n");
        break;
    default:
        break;
    }
}

void print_sep() {
    // +--+--+
    for (int i = 0; i < WIDTH; i++) printf("+--");
    printf("+\r\n");
}

void display_awale(Awale* awale) {
    print_sep();

    for (int i = 0; i < WIDTH; i++) printf("|%2d", awale->cells[CELL_COUNT - 1 - i]);
    printf("|\r\n");

    print_sep();

    for (int i = 0; i < WIDTH; i++) printf("|%2d", awale->cells[i]);
    printf("|\r\n");

    print_sep();
}

void on_awale_sync(AwaleSyncPacket packet) {
    if (localPlayer.status != PLAYING) return;
    display_awale(&packet.awale);
    if (packet.awale.state >= 0) {
        localPlayer.status = IDLE;
    }
}

void on_stdin(char* input) {
    switch (localPlayer.status) {
    case IDLE:
        if(input[0] == 'h' || strstr(input,"help")){
            printf("p/players : afficher la liste des joueurs connectés \r\n");
            printf("c/challenge : défier un joueur en duel' : \r\n");
        }
        if(input[0] == 'p' || strstr(input,"players")){
            send_request_usernames_list(client);
        }
        else if(input[0] == 'c' || strstr(input,"challenge")){
            int id = -1;
            do {
                int input;
                printf("Saisissez l'id du joueur que vous voulez défier' : \r\n");
                if (scanf(" %d", &input) != 1) {
                    printf("Erreur : Saisie invalide.");
                    while (getchar() != '\n'); // Vide le buffer d'entrée
                    continue;
                }
                id = input;
            } while (id == -1);

            send_challenge_in_duel(id);
        }
        break;
    case PLAYING:
        int cell = -1;
        do {
            int input;
            printf("Saisissez un numéro de cellule' : ");
            if (scanf(" %d", &input) != 1) {
                printf("Erreur : Saisie invalide.");
                while (getchar() != '\n'); // Vide le buffer d'entrée
                continue;
            }
            cell = input;
        } while (cell == -1);
        AwalePlayPacket awalePlayPacket = {cell};
        Buffer buffer = serialize_AwalePlayPacket(&awalePlayPacket);
        send_to(client, &buffer);
        break;
    default:
        break;
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
        printf("Usage : %s <address> <port> <pseudo>\r\n", argv[0]);
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
            case PACKET_AWALE_PLAY_ACK:
                on_awale_play_ack(deserialize_AwalePlayAckPacket(&buffer));
                break;
            case PACKET_AWALE_SYNC:
                on_awale_sync(deserialize_AwaleSyncPacket(&buffer));
                break;
            }
        } 
    }  

    close_client(client);

    end_network();
}