#pragma once

#define WIDTH 6
#define PLAYER_COUNT 2
#define CELL_COUNT (PLAYER_COUNT * WIDTH)
#define SEEDS_PER_CELL 4
#define SEEDS_COUNT (PLAYER_COUNT * WIDTH * SEEDS_PER_CELL)

enum PlayResult
{
    WRONG_TURN = -1,
    PLAYED = 0,
    INVALID_CELL = 1,
    INVALID_EMPTY = 2,
    INVALID_STARVATION = 3
};

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

int seeds_on_side(Awale *awale, int player);
int is_player_cell(int player, int cell);
int next_player(int turn);