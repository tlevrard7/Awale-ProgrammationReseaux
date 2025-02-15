#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "packets/packets.h"
#include "network/client.h"
#include "player.h"

SOCKET client;
Player localPlayer;
Player opponent;
int awalePlayerIndex;

void print_chat_packet(ChatPacket chatpacket){
    printf("%s (%d) : %s \r\n", chatpacket.sender.name, chatpacket.sender.id, chatpacket.message);
}

void send_message(int id, char* message){
    ChatPacket packet;
    packet.sender = localPlayer;
    Player receiver;
    receiver.id = id;
    packet.receiver = receiver;
    strcpy(packet.message, message);
    Buffer buffer = serialize_ChatPacket(&packet);
    send_to(client, &buffer);
}

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
            opponent = packet.requester;
            awalePlayerIndex = 1;
            break;
        case OPPONENT_DOESNT_EXIST:
            if (localPlayer.status != CHALLENGED) return;
            localPlayer.status = IDLE;
            // On a challengé qlq mais le serveur nous a dit qu'il n'existait pas
            printf("Vous avez défié %d mais ce joueur n'existe pas ou n'est pas disponible\r\n",packet.opponent.id);
            break;
        case ACCEPTED:
            if (localPlayer.status != CHALLENGED) return;
            localPlayer.status = PLAYING;
            opponent = packet.opponent;
            awalePlayerIndex = 0;

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
    Player requester = localPlayer;
    challengeInDuelPacket.requester = requester; 
    Player opponent;
    opponent.id = id_opponent;
    challengeInDuelPacket.opponent = opponent;
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

void send_request_games_list(){
    RequestGamesListPacket requestGamesListPacket;
    Buffer buffer = serialize_RequestGamesListPacket(&requestGamesListPacket);
    send_to(client, &buffer);
}

void print_games_list(AnswerGamesListPacket packet){
    if (localPlayer.status != IDLE) return;
    printf("\r\nVoici la liste des parties en cours :\r\n\r\n");
    
    if(packet.nbGames==0) printf("\r\nIl n'y a pas de partie en cours\r\n"); 
    
    for(size_t i = 0; i < packet.nbGames; i++){
        printf("- Partie %d \r\n",packet.games[i].id);
        printf("    ");
        for(int j = 0 ; j < PLAYER_COUNT ; j++){
            Player current = packet.players[i][j];
            printf("%s(id=%d)", current.name, current.id);
            if(j != PLAYER_COUNT-1) printf("  VS  ");
        }
        printf("\r\n");
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

void print_score(Awale* awale, int player) {
    printf(
        "| %s %s : %dpts \r\n",
        awale->turn == player ? ">>" : "  ",
        (awalePlayerIndex == player ? localPlayer : opponent).name,
        awale->score[player]
    );
}

void display_awale(Awale* awale) {
    /*
 11 10  9  8  7  6 
+--+--+--+--+--+--+
| 4| 4| 4| 4| 4| 4| >> Player2 : 12p
+--+--+--+--+--+--+
| 4| 4| 4| 4| 4| 4|    Player1 : 24p
+--+--+--+--+--+--+
  0  1  2  3  4  5 

C'est a vous de jouer 

    */

    for (int i = 0; i < WIDTH; i++) printf(" %2d", CELL_COUNT - 1 - i); // Affichage des indices
    printf("\r\n");
    
    print_sep();

    for (int i = 0; i < WIDTH; i++) printf("|%2d", awale->cells[CELL_COUNT - 1 - i]);
    print_score(awale, 1);

    print_sep();

    for (int i = 0; i < WIDTH; i++) printf("|%2d", awale->cells[i]); // Affichage de la première ligne
    print_score(awale, 0);

    print_sep();
    
    for (int i = 0; i < WIDTH; i++) printf(" %2d", i); // Affichage des indices
    printf("\r\n");

    switch (awale->state) {
    case STATE_PLAYING:
        if (awale->turn == awalePlayerIndex) printf("C'est à vous de jouer. Choisissez une case.\r\n");
        else printf("En attente du coup de %s...\r\n", opponent.name);
        break;
    case STATE_DRAW:
        printf("Égalité\r\n");
        break;

    default: // Win
        if (awale->state == awalePlayerIndex) printf("Vous avez gagné !\r\n");
        else printf("%s a gagné\r\n", opponent.name);
        break;
    }
    printf("\r\n");
    printf("\r\n");
}

void on_awale_sync(AwaleSyncPacket packet) {
    if (localPlayer.status != PLAYING) return;
    display_awale(&packet.awale);
    if (packet.awale.state >= 0) localPlayer.status = IDLE;
}

void on_awale_reconnect(AwaleReconnectPacket packet) {
    if (localPlayer.status != IDLE) return;
    printf("Reconnection à la partie en cours \r\n");
    localPlayer.status = PLAYING;
    opponent = packet.opponent;
    awalePlayerIndex = packet.playerIndex;
    display_awale(&packet.awale);
}

void on_stdin(char* input) {
    switch (localPlayer.status) {
    case IDLE:
        if(input[0] == 'h' || strstr(input,"help")){
            printf("p/players : afficher la liste des joueurs connectés \r\n");
            printf("g/games : afficher la liste des parties qui se jouent actuellement \r\n");
            printf("m/message : envoyer un message à un autre joueur \r\n");
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
        else if(input[0] == 'm' || strstr(input,"message")){
            
            int id = -1;
            do {
                int input;
                printf("Saisissez l'id du joueur destinataire' : \r\n");
                if (scanf(" %d", &input) != 1) {
                    printf("Erreur : Saisie invalide.");
                    while (getchar() != '\n'); // Vide le buffer d'entrée
                }
                else if(input == (int) localPlayer.id){
                    printf("Vous ne pouvez pas vous envoyer de message à vous même ! \r\n\r\n");
                }
                else{
                     id = input;
                }
            } while (id == -1);
            
            char message[256] = "\0";
            do {
                char temp[256];
                printf("Saisissez le message que vous voulez envoyer' : \r\n");
                while (getchar() != '\n');
                if (fgets(temp, sizeof(temp), stdin)==NULL) {
                    printf("Erreur : Saisie invalide.");
                    while (getchar() != '\n'); // Vide le buffer d'entrée
                    continue;
                }
                strcpy(message,temp);
            } while (strcmp(message,"\0")==0);

            send_message(id,message);
        }
        else if (input[0] == 'g' || strstr(input,"games")){
            send_request_games_list(client);
        }
        break;
    case PLAYING: {
        char *end;
        long c = strtol(input, &end, 10);
        if (end == input) {
            printf("Erreur : Saisie invalide\r\n");
        } else {
            int cell = c;
            AwalePlayPacket awalePlayPacket = {cell};
            Buffer buffer = serialize_AwalePlayPacket(&awalePlayPacket);
            send_to(client, &buffer);
        }
        break;
    }
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
    if (argc != 4 && argc != 5) {
        printf("Usage : %s <address> <port> <pseudo> [id]\r\n", argv[0]);
        return 1;
    }

    init_network();
    strcpy(localPlayer.name, argv[3]);
    localPlayer.id = argc == 4 ? 0 : atoi(argv[4]);

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
            case PACKET_ANSWER_GAMES_LIST:
                print_games_list(deserialize_AnswerGamesListPacket(&buffer));
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
            case PACKET_AWALE_RECONNECT:
                on_awale_reconnect(deserialize_AwaleReconnectPacket(&buffer));
                break;
            case PACKET_CHAT:
                print_chat_packet(deserialize_ChatPacket(&buffer));
                break;
            }
        } 
    }  

    close_client(client);

    end_network();
}