#include "awale.h"
#include <stdlib.h>
#include <time.h>

Awale init_game() {
    Awale awale;
    for (int i = 0; i < CELL_COUNT; i++) awale.cells[i] = SEEDS_PER_CELL;
    for (int i = 0; i < PLAYER_COUNT; i++) awale.score[0] = 0;
    srand(time( NULL ) );
    awale.turn = rand()%PLAYER_COUNT; // Nombre aléatoire entre 0 et PLAYER_COUNT-1
    awale.state = STATE_PLAYING;

    return awale;
}

int play(Awale *awale, int player, int cell) { // TODO clarify return value
     
    if (awale->state != STATE_PLAYING || awale->turn != player) return -1; // Not the player turns

    if (cell < 0 || CELL_COUNT <= cell || !(player*WIDTH <= cell && cell < (player+1)*WIDTH)) return 0; // Invalid cell

    int cells = awale->cells[cell];
    if (cell%WIDTH + cells >= WIDTH) return 0; // Would not place cells on the other side

    awale->cells[cell] = 0;
    for (int i = 0; i < cells; i++) {
        int c = (cell + 1 + i) % CELL_COUNT;
        awale->cells[c]++;

        // no capture 
        if (awale->cells[c] != 3 && awale->cells[c] != 4) continue;

        // capture the cells
        awale->score[player] += awale->cells[c];
        awale->cells[c] = 0;
    }

    post_play(awale, player);
    return 1;
}

int seeds_on_side(Awale* awale, int player) { 
    int seeds = 0;
    for (int i = awale->turn * WIDTH; i < (awale->turn + 1) * WIDTH; i++) seeds += awale->cells[i];
    return seeds;
}

int post_play(Awale* awale, int player) {
    // Win by majority seeds
    if (awale->score[player] > SEEDS_COUNT / 2) {
        awale->state = player;
        return awale->state;
    }

    switch_turn(awale);
}

int switch_turn(Awale* awale) {
    awale->turn = (awale->turn + 1) % PLAYER_COUNT;

    // the new player cannot play
    if (seeds_on_side(awale, awale->turn) == 0) end_game(awale);
    return awale->state;
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