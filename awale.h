#pragma once

#define WIDTH 6
#define PLAYER_COUNT 2
#define CELL_COUNT PLAYER_COUNT * WIDTH
#define SEEDS_PER_CELL 4
#define SEEDS_COUNT PLAYER_COUNT * WIDTH * SEEDS_PER_CELL

enum AwaleState
{
    STATE_PLAYING = -1,
    STATE_DRAW = -2
};

typedef struct {
    int state;
    int cells[CELL_COUNT];
    int turn;
    int score[PLAYER_COUNT];
} Awale;

Awale init_game();
int play(Awale* awale, int player, int cell);
