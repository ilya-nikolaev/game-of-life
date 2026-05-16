#include <time.h>

#include "config.h"
#include "core.h"
#include "ui.h"

int main() {
    srand(time(NULL));

    uint8_t max_FPS = 60;

    RulesBitmap16 birth = 1u << 3;
    RulesBitmap16 survival = (1u << 2) | (1u << 3);

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_DisplayMode DM;
    if (SDL_GetDesktopDisplayMode(0, &DM) != 0) {
        fprintf(stderr, "SDL_GetDesktopDisplayMode: %s\n", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    Game game;
    game_init(&game, DM.w, DM.h, birth, survival);

    UI ui;
    ui_init(&ui, &game, max_FPS);

    ui_run(&ui);

    ui_deinit(&ui);
    game_deinit(&game);

    SDL_Quit();

    return EXIT_SUCCESS;
}
