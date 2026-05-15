#ifndef UI_H
#define UI_H

#include <SDL2/SDL.h>

#include "core.h"

typedef struct UI {
    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_Texture *texture;

    uint32_t *pixels;

    Game *game;

    uint8_t max_FPS;

    bool is_running;
    bool is_paused;

    bool is_LMB_pressed;

    uint32_t camera_drag_prev_x;
    uint32_t camera_drag_prev_y;

    uint32_t camera_position;
} UI;

void ui_init(UI *ui, Game *game, uint8_t max_FPS);
void ui_deinit(UI *ui);

void ui_run(UI *ui);

#endif
