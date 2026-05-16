#include <time.h>

#include "config.h"
#include "core.h"
#include "ui.h"

int main() {
    srand(time(NULL));

    uint8_t max_FPS = 60;

    RulesBitmap16 birth = 1u << 3;
    RulesBitmap16 survival = (1u << 2) | (1u << 3);

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        goto error;

    SDL_DisplayMode DM;
    if (SDL_GetDesktopDisplayMode(0, &DM) != 0)
        goto error;

    Game game;
    if (game_init(&game, DM.w, DM.h, birth, survival) != 0)
        goto error;

    UI ui;
    if (ui_init(&ui, &game, max_FPS) != 0)
        goto error;

    ui_run(&ui);

    ui_deinit(&ui);
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
