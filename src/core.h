#ifndef CORE_H
#define CORE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct Game {
    bool *cells;
    bool *backbuffer;

    size_t width;
    size_t height;
    size_t count;

    bool* b;
    bool* s;
} Game;

int game_init(Game *game, size_t width, size_t height, bool* b, bool* s);
void game_deinit(Game *game);

void game_step(Game *game);

#endif
