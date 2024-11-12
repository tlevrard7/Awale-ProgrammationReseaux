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
    srand(time(NULL)); // Initialise le générateur de nombre aléatoire
    awale.turn = rand()%PLAYER_COUNT; // Nombre aléatoire entre 0 et PLAYER_COUNT-1
    awale.state = STATE_PLAYING;

    return awale;
}


int seeds_on_side(Awale *awale, int player) {
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
    if (awale->state != STATE_PLAYING) return awale->state;

    awale->state = STATE_DRAW; // Draw by default

    int winner = 0;
    int draw = 1;
    // Computes the final scores of players
    for (int p = 0; p < PLAYER_COUNT; p++) {
        awale->score[p] += seeds_on_side(awale, p);

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
    if (awale->state != STATE_PLAYING || awale->turn != player)
        return WRONG_TURN; // Not the player turns

    if (!is_player_cell(player, cell))
        return INVALID_CELL; // Invalid cell
    if (awale->cells[cell] == 0)
        return INVALID_EMPTY; // Invalid cell

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
            return INVALID_STARVATION; // Would not place cells on the other side
    }
    return PLAYED;
}

void play_do(Awale *awale, int player, int cell) {
    int seeds = awale->cells[cell];
    awale->cells[cell] = 0;
    int c = cell;
    for (int i = 0; seeds > 0; i++) {
        c = (cell + 1 + i) % CELL_COUNT;
        if (c == cell) continue;
        awale->cells[c]++;
        seeds--;
    }
    int oldCells[CELL_COUNT];
    for (int i = 0; i < CELL_COUNT; i++) oldCells[i] = awale->cells[i];

    int captured = 0;
    for (c = c; !is_player_cell(player, c) && (awale->cells[c] == 2 || awale->cells[c] == 3); c--) {
        captured += awale->cells[c];
        awale->cells[c] = 0;
    }

    if (seeds_on_side(awale, next_player(player)) == 0) {
        for (int i = 0; i < CELL_COUNT; i++) awale->cells[i] = oldCells[i];
    }
    else
    {
        awale->score[player] += captured;
    }
}

void play_end_check(Awale* awale, int player) {
    // Win by majority seeds
    if (awale->score[player] > SEEDS_COUNT / 2) end_game(awale);
    
    // The new player cannot play
    else if (seeds_on_side(awale, next_player(player)) == 0) end_game(awale);
}

int play(Awale *awale, int player, int cell) {
    /*
        Fonction permettant au joueur 'player' de jouer son coup après avoir choisi la case 'cell' pour jouer
        Vérifie la validité du coup en vérifiant que : 
            - le jeu est bien en cours et c'est bien le tour du joueur 'player' -> OK
            - la cell est bien une case du joueur player -> OK
            - la cell contient bien des graines -> OK
            - si l'adversaire n'a plus de graine, 'player' a bien joué un coup permettant de le nourrir (s'il existe) -> OK
        Si le coup est valide :
            - Et qu'il devait prendre toutes les graines adverses, le coup est joué mais aucune capture n'est faite (les scores ne bougent pas) -> OK
            - Et si la dernière graine tombe dans une case du camp adverse qui possède (après le coup) 2 ou 3 graines, le score de 'player' est incrémenté de 2 ou 3 
              Ensuite, si le trou précédent est dans le camp adverse et possède (après le coup) 2 ou 3 graines, le score est incrémenté de 2 ou 3
              Et ainsi de suite jusqu'à ce qu'il arrive à son camp ou que une case ne contienne pas 2 ou 3 graines
            - la grille est mise à jour pour prendre en compte l'égrènement causé par le coup (si awale.cells[cell]>=12, il ne faut pas égrener dans 'cell')
        A la fin d'un coup, on doit vérifier si la partie est terminée :
            - Si un joueur possède plus de la moitié des graines, il a gagné -> OK
            - S'il n'est plus possible de capturer d'autres graines
            - Si un joueur player1 n'a plus de graine et qu'il n'existe pas de coup permettant à l'autre joueur player2 de le nourrir, player2 récupère toutes les graines restantes
    */

    int code = play_validate(awale, player, cell);
    if (code != 0) return code;

    play_do(awale, player, cell);

    play_end_check(awale, player);

    awale->turn = next_player(awale->turn);

    return PLAYED;
}
