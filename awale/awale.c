#include "awale.h"
#include <stdlib.h>
#include <time.h>

Awale init_game() {
    /*
        Fonction permettant d'initialiser une partie d'AWALE : initialise la grille, les scores et l'état du jeu. 
        Permet également de choisir aléatoirement le joueur qui commence.
        Retourne le jeu initialisé
    */
    Awale awale;
    for (int i = 0; i < CELL_COUNT; i++) awale.cells[i] = SEEDS_PER_CELL;
    for (int i = 0; i < PLAYER_COUNT; i++) awale.score[i] = 0;
    srand(time(NULL)); // Initialise le générateur de nombres aléatoires
    awale.turn = rand()%PLAYER_COUNT; // Nombre aléatoire entre 0 et PLAYER_COUNT-1
    awale.state = STATE_PLAYING;

    return awale;
}


int seeds_on_side(Awale *awale, int player) {
    /* 
        Renvoie le nombre de graines positionnées dans le camp de player
    */
    int nbSeeds = 0; int firstCell = player*WIDTH; int lastCell = firstCell+WIDTH - 1;
    for (int i = firstCell; i <= lastCell; i++) nbSeeds+=awale->cells[i];
    return nbSeeds;
}

int is_player_cell(int player, int cell) {
    return player * WIDTH <= cell && cell < (player + 1) * WIDTH;
}

int next_player(int turn)  {
    return (turn + 1) % PLAYER_COUNT;
}

int end_game(Awale *awale) {
    /*
        Finit une partie en calculant les scores finaux et en enregistrant le gagnant dans awale->state (si ce n'est pas un draw)
    */
    if (awale->state != STATE_PLAYING) return awale->state;

    awale->state = STATE_DRAW; // Draw by default

    int winner = 0;
    int draw = 1;
    // Computes the final scores of players
    for (int p = 0; p < PLAYER_COUNT; p++) {
        awale->score[p] += seeds_on_side(awale, p); // S'il reste des graines dans le camp de p, p les capture

        if(awale->score[p] > awale->score[winner]) {
            winner = p;
            draw = 0;
        } else if (awale->score[p] == awale->score[winner]) {
            draw = 1;
        }
    }
    awale->state = draw ? STATE_DRAW : winner;
    return awale->state;
}

int play_validate(Awale* awale, int player, int cell) {
    /*
        Vérifie la validité du coup joué par 'player' (qui a choisi la case 'cell') : 
            - le jeu est bien en cours et c'est bien le tour du joueur 'player'
            - la cell est bien une case du joueur player
            - la cell contient bien des graines
            - si l'adversaire n'a plus de graine, 'player' a bien joué un coup permettant de le nourrir (s'il existe)
    */
    if (awale->state != STATE_PLAYING || awale->turn != player)
        return WRONG_TURN; // Not the player turns

    if (!is_player_cell(player, cell))
        return INVALID_CELL; // Not a player cell
    if (awale->cells[cell] == 0)
        return INVALID_EMPTY; // The cell does not contain seeds

    int seeds = awale->cells[cell];
    if (seeds_on_side(awale, next_player(player)) == 0) {
        int canSave = 0;
        for (int i = player * WIDTH; i < (player + 1) * WIDTH; i++){
            if ((i % WIDTH) + awale->cells[i] >= WIDTH) {
                canSave = 1;
                break;
            }
        }
        
        if (canSave && (cell % WIDTH) + seeds < WIDTH)
            return INVALID_STARVATION; // Would not place seeds on the other side while it is possible
    }
    return PLAYED;
}

void play_do(Awale *awale, int player, int cell) {
    /*
        Met à jour la grille (en fonction de l'égrènement et des captures) et les scores en fonction du coup valide joué par 'player' (qui a choisi la case 'cell') :
            - (R1) Si le coup devait prendre toutes les graines adverses, le coup est joué mais aucune capture n'est faite (les scores ne bougent pas)
            - Captures : Si la dernière graine tombe dans une case du camp adverse qui possède (après le coup) 2 ou 3 graines, le score de 'player' est incrémenté de 2 ou 3 
              Ensuite, si le trou précédent est dans le camp adverse et possède (après le coup) 2 ou 3 graines, le score est incrémenté de 2 ou 3
              Et ainsi de suite jusqu'à ce qu'il arrive à son camp ou que une case ne contienne pas 2 ou 3 graines
            - Même s'il y a plus de 12 graines dans 'cell', on n'égrène pas dans 'cell'
    */

    // Égrènement
    int seeds = awale->cells[cell];
    awale->cells[cell] = 0;
    int c = cell;
    for (int i = 0; seeds > 0; i++) {
        c = (cell + 1 + i) % CELL_COUNT;
        if (c == cell) continue;
        awale->cells[c]++;
        seeds--;
    }
    
    // Captures
    int oldCells[CELL_COUNT];
    for (int i = 0; i < CELL_COUNT; i++) oldCells[i] = awale->cells[i];

    int captured = 0;
    for (c = c; !is_player_cell(player, c) && (awale->cells[c] == 2 || awale->cells[c] == 3); c--) {
        captured += awale->cells[c];
        awale->cells[c] = 0;
    }

    if (seeds_on_side(awale, next_player(player)) == 0) {
        for (int i = 0; i < CELL_COUNT; i++) awale->cells[i] = oldCells[i]; // (R1)
    }
    else
    {
        awale->score[player] += captured;
    }
}

void play_end_check(Awale* awale, int player) {
    // Win by majority seeds
    if (awale->score[player] > SEEDS_COUNT / 2) end_game(awale);
    
    // The next player can't play because there was no move allowing 'player' to feed next player (canSave=0, cf. play_validate() )
    else if (seeds_on_side(awale, next_player(player)) == 0) end_game(awale);
}

int play(Awale *awale, int player, int cell) {
    /*
        Permet au joueur 'player' de jouer son coup après avoir choisi la case 'cell' pour jouer
        Vérifie d'abord si le coup est valide, puis joue le coup et enfin vérifie si la partie est terminée
    */

    int code = play_validate(awale, player, cell);
    if (code != 0) return code; // si coup pas valide, on renvoie le code d'erreur

    play_do(awale, player, cell);

    play_end_check(awale, player);

    awale->turn = next_player(awale->turn);

    return PLAYED;
}
