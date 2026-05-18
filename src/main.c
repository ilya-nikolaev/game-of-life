#include <time.h>

#include "config.h"
#include "core.h"
#include "ui.h"

const uint8_t TICKRATE = 60;

const size_t BOARD_SIZE_X = 4096;
const size_t BOARD_SIZE_Y = 4096;

const char *RULES_STRING = "B3S23";

int main() {
    srand(time(NULL));

    Rules rules;
    config_parse_rules(RULES_STRING, &rules);

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        goto error;

    Game game;
    if (game_init(&game, BOARD_SIZE_X, BOARD_SIZE_Y, rules.b, rules.s) != 0)
        goto error;

    Engine engine;
    if (engine_init(&engine, &game, TICKRATE) != 0)
        goto error;

    engine_run(&engine);

    engine_deinit(&engine);
    game_deinit(&game);

    SDL_Quit();
    return EXIT_SUCCESS;

error:
    if (SDL_GetError() != 0)
        fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
    fprintf(stderr, "Initialization failed\n");
    SDL_Quit();
    return EXIT_FAILURE;
}
