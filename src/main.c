#include <time.h>

#include <getopt.h>
#include <unistd.h>

#include "config.h"
#include "core.h"
#include "ui.h"

int main(int argc, char *argv[]) {
    srand(time(NULL));

    uint32_t primary_color = 0xFF00FF00, background_color = 0xFF000000;
    uint8_t max_FPS = 24;

    Rules rules;
    RulesBitmap16 birth = 1 << 3, survival = 1 << 2 | 1 << 3;

    int c;
    while ((c = getopt(argc, argv, "r:f:c:b:")) != -1) {
        switch (c) {
        case 'r':
            rules = config_parse_rules(optarg);
            birth = rules.birth;
            survival = rules.survival;
            break;
        case 'f':
            max_FPS = (uint8_t)atoi(optarg);
            break;
        case 'c':
            primary_color = config_parse_color(optarg);
            break;
        case 'b':
            background_color = config_parse_color(optarg);
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
    ui_init(&ui, &game, max_FPS, primary_color, background_color);

    ui_run(&ui);

    ui_deinit(&ui);
    game_deinit(&game);

    return EXIT_SUCCESS;
}
