#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "core.h"
#include "ui.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

const uint32_t PRIMARY_COLOR = 0x0000FF00;

const uint16_t MIN_ZOOM = 1;
const uint16_t MAX_ZOOM = 128;

const uint16_t FILLING_THRESHOLD = 0xF0;
const uint16_t FILLING_LIMIT = 0x3FF;

static void fill_field(Game *game) {
    for (size_t i = 0; i < game->count; ++i)
        game->cells[i] = (rand() & FILLING_LIMIT) < FILLING_THRESHOLD;
}

static void process_keyboard_event(Engine *engine, SDL_KeyboardEvent *event) {
    switch (event->keysym.sym) {
    case SDLK_ESCAPE:
        engine->running = 0;
        break;
    case SDLK_SPACE:
        engine->paused = !engine->paused;
        break;
    case SDLK_r:
        fill_field(engine->game);
        break;
    case SDLK_c:
        memset(engine->game->cells, 0, engine->game->count);
        break;
    default:
        break;
    }
}

static void update_camera_position(
    InputState *input,
    Camera *camera,
    int32_t drag_curr_x,
    int32_t drag_curr_y
) {
    camera->offset_x_px += input->drag_prev_x - drag_curr_x;
    camera->offset_y_px += input->drag_prev_y - drag_curr_y;
    input->drag_prev_x = drag_curr_x;
    input->drag_prev_y = drag_curr_y;
}

static void process_mouse_event(
    InputState *input,
    Camera *camera,
    SDL_MouseButtonEvent *event,
    bool pressed
) {
    switch (event->button) {
    case SDL_BUTTON_LEFT:
        if (pressed && !input->pressed_lmb) {
            input->drag_prev_x = event->x;
            input->drag_prev_y = event->y;
        } else if (pressed) {
            update_camera_position(input, camera, event->x, event->y);
        }

        input->pressed_lmb = pressed;
        break;
    default:
        break;
    }
}

static void process_mouse_wheel_event(
    InputState *input,
    Camera *camera,
    SDL_MouseWheelEvent *event
) {
    float world_x =
        (camera->offset_x_px + input->last_mouse_pos_x) / (float)camera->zoom;
    float world_y =
        (camera->offset_y_px + input->last_mouse_pos_y) / (float)camera->zoom;

    if (event->y < 0)
        camera->zoom = MAX(camera->zoom - 1, MIN_ZOOM);
    else
        camera->zoom = MIN(camera->zoom + 1, MAX_ZOOM);

    camera->offset_x_px =
        (int32_t)(world_x * camera->zoom - input->last_mouse_pos_x);
    camera->offset_y_px =
        (int32_t)(world_y * camera->zoom - input->last_mouse_pos_y);
}

static void process_mouse_motion_event(
    InputState *input,
    Camera *camera,
    SDL_MouseMotionEvent *event
) {
    input->last_mouse_pos_x = event->x;
    input->last_mouse_pos_y = event->y;

    if (input->pressed_lmb)
        update_camera_position(input, camera, event->x, event->y);
}

static void process_events(Engine *engine) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_KEYDOWN:
            process_keyboard_event(engine, &event.key);
            break;
        case SDL_MOUSEBUTTONDOWN:
            process_mouse_event(
                &engine->input,
                &engine->camera,
                &event.button,
                true
            );
            break;
        case SDL_MOUSEBUTTONUP:
            process_mouse_event(
                &engine->input,
                &engine->camera,
                &event.button,
                false
            );
            break;
        case SDL_MOUSEWHEEL:
            process_mouse_wheel_event(
                &engine->input,
                &engine->camera,
                &event.wheel
            );
            break;
        case SDL_MOUSEMOTION:
            process_mouse_motion_event(
                &engine->input,
                &engine->camera,
                &event.motion
            );
            break;
        case SDL_QUIT:
            engine->running = 0;
            break;
        default:
            break;
        }
    }
}

static inline int32_t wrap_fast(int32_t value, int32_t limit) {
    int32_t r = value % limit;
    return r < 0 ? r + limit : r;
}

static void draw(Engine *engine) {
    Screen *screen = &engine->screen;
    Camera *camera = &engine->camera;
    Game *game = engine->game;

    memset(screen->pixels, 0, screen->count * sizeof(uint32_t));

    int32_t zoom = camera->zoom;
    int32_t start_x = camera->offset_x_px / zoom;
    int32_t start_y = camera->offset_y_px / zoom;
    int32_t end_x = start_x + screen->width / zoom + 2;
    int32_t end_y = start_y + screen->height / zoom + 2;

    int32_t screen_w = screen->width;
    int32_t screen_h = screen->height;

#pragma omp parallel for collapse(2) schedule(static)
    for (int32_t cell_y = start_y; cell_y < end_y; ++cell_y) {
        for (int32_t cell_x = start_x; cell_x < end_x; ++cell_x) {
            int32_t wrapped_x = wrap_fast(cell_x, game->width);
            int32_t wrapped_y = wrap_fast(cell_y, game->height);

            if (!game->cells[wrapped_x + wrapped_y * game->width])
                continue;

            int32_t screen_x = cell_x * zoom - camera->offset_x_px;
            int32_t screen_y = cell_y * zoom - camera->offset_y_px;

            int32_t cell_left = screen_x;
            int32_t cell_top = screen_y;
            int32_t cell_right = screen_x + zoom - 1;
            int32_t cell_bottom = screen_y + zoom - 1;

            int32_t draw_left = cell_left > 0 ? cell_left : 0;
            int32_t draw_top = cell_top > 0 ? cell_top : 0;
            int32_t draw_right =
                cell_right < screen_w - 1 ? cell_right : screen_w - 1;
            int32_t draw_bottom =
                cell_bottom < screen_h - 1 ? cell_bottom : screen_h - 1;

            if (draw_left <= draw_right && draw_top <= draw_bottom) {
                for (int32_t y = draw_top; y <= draw_bottom; ++y) {
                    uint32_t *row = &screen->pixels[y * screen_w];
                    for (int32_t x = draw_left; x <= draw_right; ++x) {
                        row[x] = PRIMARY_COLOR;
                    }
                }
            }
        }
    }

    SDL_UpdateTexture(
        screen->texture,
        NULL,
        screen->pixels,
        screen_w * sizeof(uint32_t)
    );
    SDL_RenderCopy(screen->renderer, screen->texture, NULL, NULL);
    SDL_RenderPresent(screen->renderer);
}

int initialize_screen(Screen *screen) {
    SDL_DisplayMode display_mode;
    if (SDL_GetDesktopDisplayMode(0, &display_mode) != 0)
        return -1;

    screen->width = display_mode.w;
    screen->height = display_mode.h;
    screen->count = screen->width * screen->height;

    int result;

    result = SDL_CreateWindowAndRenderer(
        screen->width,
        screen->height,
        0,
        &screen->window,
        &screen->renderer
    );
    if (result != 0)
        return -1;

    screen->texture = SDL_CreateTexture(
        screen->renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STATIC,
        screen->width,
        screen->height
    );
    if (screen->texture == NULL)
        return -1;

    result = SDL_SetWindowFullscreen(screen->window, SDL_WINDOW_FULLSCREEN);
    if (result != 0)
        return -1;

    screen->pixels = malloc(sizeof(uint32_t) * screen->width * screen->height);
    if (screen->pixels == NULL)
        return -1;

    return 0;
}

int engine_init(Engine *engine, Game *game, uint8_t tickrate) {
    engine->game = game;

    Screen screen;
    if (initialize_screen(&screen) != 0)
        return -1;
    engine->screen = screen;

    Camera camera;
    camera.offset_x_px = 0;
    camera.offset_y_px = 0;
    camera.zoom = 1;
    engine->camera = camera;

    InputState input;
    input.pressed_lmb = false;
    input.drag_prev_x = 0;
    input.drag_prev_y = 0;
    input.last_mouse_pos_x = 0;
    input.last_mouse_pos_y = 0;
    engine->input = input;

    engine->tickrate = tickrate;

    engine->running = true;
    engine->paused = false;

    fill_field(game);

    return 0;
}

void engine_deinit(Engine *engine) {
    free(engine->screen.pixels);

    SDL_DestroyTexture(engine->screen.texture);
    SDL_DestroyRenderer(engine->screen.renderer);
    SDL_DestroyWindow(engine->screen.window);

    SDL_Quit();
}

void engine_run(Engine *engine) {
    double max_delay =
        (engine->tickrate == 0) ? 0.0 : 1000.0 / engine->tickrate;

    uint32_t prev_tick = SDL_GetTicks();

    while (engine->running) {
        process_events(engine);
        draw(engine);

        uint32_t curr_tick = SDL_GetTicks();
        uint32_t delta = curr_tick - prev_tick;

        if (delta >= max_delay) {
            if (!engine->paused)
                game_step(engine->game);

            prev_tick = curr_tick;
        }
    }
}