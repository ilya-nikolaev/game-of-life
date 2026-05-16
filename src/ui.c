#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "core.h"
#include "ui.h"

const uint32_t PRIMARY_COLOR = 0x0000FF00;
const uint32_t BACKGROUND_COLOR = 0x00000000;

const uint16_t MIN_ZOOM = 1;
const uint16_t MAX_ZOOM = 16;

const uint16_t FILLING_PERCENTAGE = 0xF0;
const uint16_t FILLING_LIMIT = 0x3FF;

static void fill_field(UI *ui) {
    for (size_t i = 0; i < ui->game->count; ++i)
        ui->game->cells[i] = (rand() & FILLING_LIMIT) < FILLING_PERCENTAGE;
}

static inline void clear_field(UI *ui) {
    memset(ui->game->cells, 0, ui->game->count);
}

static void process_keyboard_event(UI *ui, SDL_KeyboardEvent *event) {
    switch (event->keysym.sym) {
    case SDLK_ESCAPE:
        ui->is_running = 0;
        break;
    case SDLK_SPACE:
        ui->is_paused = !ui->is_paused;
        break;
    case SDLK_r:
        fill_field(ui);
        break;
    case SDLK_c:
        clear_field(ui);
        break;
    default:
        break;
    }
}

static void update_camera_position(
    UI *ui, uint32_t camera_drag_curr_x, uint32_t camera_drag_curr_y
) {
    int32_t dx = ui->camera_drag_prev_x - camera_drag_curr_x;
    int32_t dy = ui->camera_drag_prev_y - camera_drag_curr_y;

    ui->camera_position_x = (ui->camera_position_x + dx) % ui->game->width;
    ui->camera_position_y = (ui->camera_position_y + dy) % ui->game->height;

    ui->camera_drag_prev_x = camera_drag_curr_x;
    ui->camera_drag_prev_y = camera_drag_curr_y;
}

static void
process_mouse_event(UI *ui, SDL_MouseButtonEvent *event, bool pressed) {
    switch (event->button) {
    case SDL_BUTTON_LEFT:
        if (pressed && !ui->is_LMB_pressed) {
            ui->camera_drag_prev_x = event->x;
            ui->camera_drag_prev_y = event->y;
        } else if (pressed) {
            update_camera_position(ui, event->x, event->y);
        }

        ui->is_LMB_pressed = pressed;
        break;
    default:
        break;
    }
}

static void process_mouse_wheel_event(UI *ui, SDL_MouseWheelEvent *event) {
    if (event->y < 0) {
        ui->zoom--;
        if (ui->zoom < MIN_ZOOM)
            ui->zoom = MIN_ZOOM;
    } else {
        ui->zoom++;
        if (ui->zoom > MAX_ZOOM)
            ui->zoom = MAX_ZOOM;
    }
}

static void process_mouse_motion_event(UI *ui, SDL_MouseMotionEvent *event) {
    if (ui->is_LMB_pressed)
        update_camera_position(ui, event->x, event->y);
}

static void process_events(UI *ui) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_KEYDOWN:
            process_keyboard_event(ui, &event.key);
            break;
        case SDL_MOUSEBUTTONDOWN:
            process_mouse_event(ui, &event.button, true);
            break;
        case SDL_MOUSEBUTTONUP:
            process_mouse_event(ui, &event.button, false);
            break;
        case SDL_MOUSEWHEEL:
            process_mouse_wheel_event(ui, &event.wheel);
            break;
        case SDL_MOUSEMOTION:
            process_mouse_motion_event(ui, &event.motion);
            break;
        case SDL_QUIT:
            ui->is_running = 0;
            break;
        default:
            break;
        }
    }
}

static void draw(UI *ui) {
    size_t shifted_index;
    size_t shift =
        ui->camera_position_x + ui->camera_position_y * ui->game->width;

    for (size_t index = 0; index < ui->game->count; ++index) {
        shifted_index = (index + shift) % ui->game->count;
        ui->pixels[index] =
            ui->game->cells[shifted_index] ? PRIMARY_COLOR : BACKGROUND_COLOR;
    }

    SDL_UpdateTexture(
        ui->texture, NULL, ui->pixels, ui->game->width * sizeof(uint32_t)
    );
    SDL_RenderCopy(ui->renderer, ui->texture, NULL, NULL);
    SDL_RenderPresent(ui->renderer);
}

int ui_init(UI *ui, Game *game, uint8_t max_FPS) {
    ui->game = game;

    int result;

    result = SDL_CreateWindowAndRenderer(
        ui->game->width, ui->game->height, 0, &ui->window, &ui->renderer
    );
    if (result != 0)
        return -1;

    ui->texture = SDL_CreateTexture(
        ui->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC,
        ui->game->width, ui->game->height
    );
    if (ui->texture == NULL)
    return -1;

    result = SDL_SetWindowFullscreen(ui->window, SDL_WINDOW_FULLSCREEN);
    if (result != 0)
        return -1;

    ui->pixels = malloc(sizeof(uint32_t) * ui->game->count);
    if (ui->pixels == NULL)
        return -1;

    ui->max_FPS = max_FPS;

    ui->is_running = true;
    ui->is_paused = false;

    ui->is_LMB_pressed = false;

    ui->camera_drag_prev_x = 0;
    ui->camera_drag_prev_y = 0;

    ui->camera_position_x = 0;
    ui->camera_position_y = 0;

    ui->zoom = 1;

    fill_field(ui);

    return 0;
}

void ui_deinit(UI *ui) {
    free(ui->pixels);

    SDL_DestroyTexture(ui->texture);
    SDL_DestroyRenderer(ui->renderer);
    SDL_DestroyWindow(ui->window);

    SDL_Quit();
}

void ui_run(UI *ui) {
    double max_delay = (ui->max_FPS == 0) ? 0.0 : 1000.0 / ui->max_FPS;

    uint32_t prev = SDL_GetTicks();

    while (ui->is_running) {
        process_events(ui);
        draw(ui);

        uint32_t curr = SDL_GetTicks();
        uint32_t delta = curr - prev;

        if (delta >= max_delay) {
            if (!(ui->is_paused))
                game_step(ui->game);

            prev = curr;
        }
    }
}
