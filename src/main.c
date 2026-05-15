#include <time.h>

#include <getopt.h>
#include <unistd.h>

#include "config.h"
#include "core.h"
#include "ui.h"

int main(int argc, char *argv[]) {
    srand(time(NULL));

    uint8_t max_FPS = 24;

    Rules rules;
    RulesBitmap16 birth = 1 << 3, survival = 1 << 2 | 1 << 3;

    int c;
    while ((c = getopt(argc, argv, "r:f:")) != -1) {
        switch (c) {
        case 'r':
            rules = config_parse_rules(optarg);
            birth = rules.birth;
            survival = rules.survival;
            break;
        case 'f':
            max_FPS = (uint8_t)atoi(optarg);
            break;
        default:
            break;
        }
    }

    SDL_Init(SDL_INIT_VIDEO);

    SDL_DisplayMode DM;
    SDL_GetDesktopDisplayMode(0, &DM);

    Game game;
    game_init(&game, DM.w, DM.h, birth, survival);

    UI ui;
    ui_init(&ui, &game, max_FPS);

    ui_run(&ui);

    ui_deinit(&ui);
    game_deinit(&game);

    return EXIT_SUCCESS;
}
