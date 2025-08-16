#include <stdlib.h>

#include "core.h"

static inline size_t loop_index(int64_t index, size_t length) {
    return (index < 0) ? index + length : (index % length);
}

static inline bool find_rule(RulesBitmap16 rules, uint8_t alive) {
    return rules & (1 << alive);
}

static inline bool cell_at(const Game *game, size_t x, size_t y) {
    return game->cells[y * game->width + x];
}

void game_init(
    Game *game, size_t width, size_t height, RulesBitmap16 birth,
    RulesBitmap16 survival
) {
    game->width = width;
    game->height = height;
    game->count = width * height;

    game->cells = malloc(sizeof(bool) * game->count);
    game->backbuffer = malloc(sizeof(bool) * game->count);

    game->birth = birth;
    game->survival = survival;
}

void game_deinit(Game *game) {
    free(game->cells);
    free(game->backbuffer);
}

void game_step(Game *game) {
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

        game->backbuffer[i] = game->cells[i] ? find_rule(game->survival, alive)
                                             : find_rule(game->birth, alive);
    }

    bool *tmp = game->cells;
    game->cells = game->backbuffer;
    game->backbuffer = tmp;
}
