#ifndef UI_H
#define UI_H

#include <SDL2/SDL.h>

#include "core.h"

typedef struct {
    int32_t offset_x_px;
    int32_t offset_y_px;

    uint8_t zoom;

    float velocity_x;
    float velocity_y;
    float accum_x;
    float accum_y;
} Camera;

typedef struct {
    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_Texture *texture;

    uint32_t width;
    uint32_t height;

    size_t count;

    uint32_t *pixels;
} Screen;

typedef struct {
    bool pressed_lmb;

    int32_t drag_prev_x;
    int32_t drag_prev_y;

    int32_t last_mouse_pos_x;
    int32_t last_mouse_pos_y;

    uint32_t last_motion_time;

    float smooth_vel_x;
    float smooth_vel_y;
} InputState;

typedef struct {
    Camera camera;
    Screen screen;

    InputState input;

    Game *game;

    uint8_t tickrate;

    bool running;
    bool paused;
} Engine;

int engine_init(Engine *engine, Game *game, uint8_t tickrate);
void engine_deinit(Engine *engine);

void engine_run(Engine *engine);

#endif
