#ifndef CORE_H
#define CORE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef uint16_t RulesBitmap16;

typedef struct Game {
    bool *cells;
    bool *backbuffer;

    size_t width;
    size_t height;
    size_t count;

    RulesBitmap16 birth;
    RulesBitmap16 survival;
} Game;

void game_init(
    Game *game, size_t width, size_t height, RulesBitmap16 birth,
    RulesBitmap16 survival
);
void game_deinit(Game *game);

void game_step(Game *game);

#endif
