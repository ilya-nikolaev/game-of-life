#include <stdlib.h>

#include "core.h"

const uint16_t FILLING_THRESHOLD = 0xF0;
const uint16_t FILLING_LIMIT = 0x3FF;

static inline size_t loop_index(int64_t index, size_t length) {
    return (index < 0) ? index + length : (index % length);
}

static inline bool cell_at(const Game *game, size_t x, size_t y) {
    return game->cells[y * game->width + x];
}

void game_randomize_field(Game *game) {
    for (size_t i = 0; i < game->count; ++i)
        game->cells[i] = (rand() & FILLING_LIMIT) < FILLING_THRESHOLD;
}

int game_init(Game *game, size_t width, size_t height, bool *b, bool *s) {
    game->width = width;
    game->height = height;
    game->count = width * height;

    game->cells = malloc(sizeof(bool) * game->count);
    if (game->cells == NULL)
        return -1;

    game->backbuffer = malloc(sizeof(bool) * game->count);
    if (game->backbuffer == NULL)
        return -1;

    game->b = b;
    game->s = s;

    return 0;
}

void game_deinit(Game *game) {
    free(game->cells);
    free(game->backbuffer);
}

void game_step(Game *game) {
#pragma omp parallel for
    for (size_t i = 0; i < game->count; ++i) {
        size_t x = i % game->width, y = i / game->width;
        bool is_x_border = !x || x == game->width - 1;
        bool is_y_border = !y || y == game->height - 1;

        uint8_t alive = 0;
        for (int8_t dx = -1; dx <= 1; ++dx) {
            for (int8_t dy = -1; dy <= 1; ++dy) {
                if (!(dx || dy))
                    continue;

                size_t x_on_torus = is_x_border && dx
                    ? loop_index(x + dx, game->width)
                    : x + dx;
                size_t y_on_torus = is_y_border && dy
                    ? loop_index(y + dy, game->height)
                    : y + dy;

                alive += cell_at(game, x_on_torus, y_on_torus);
            }
        }

        game->backbuffer[i] = game->cells[i] ? game->s[alive] : game->b[alive];
    }

    bool *tmp = game->cells;
    game->cells = game->backbuffer;
    game->backbuffer = tmp;
}