#include <stdlib.h>

#include "core.h"
#include "ui.h"

void fill_field(UI *ui)
{
    for (size_t i = 0; i < ui->game->count; ++i)
        ui->game->cells[i] = rand() % 100 < ui->filling_percentage;
}

void clear_field(UI *ui)
{
    for (size_t i = 0; i < ui->game->count; ++i)
        ui->game->cells[i] = false;
}

void process_keyboard_event(UI *ui, SDL_KeyboardEvent *event)
{
    switch (event->keysym.sym)
    {
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

void process_mouse_event(UI *ui, SDL_MouseButtonEvent *event, bool pressed)
{
    switch (event->button)
    {
    case SDL_BUTTON_LEFT:
        ui->is_LMB_pressed = pressed;
        if (pressed)
            ui->game->cells[event->x + event->y * ui->game->width] = true;
        break;
    case SDL_BUTTON_RIGHT:
        ui->is_RMB_pressed = pressed;
        if (pressed)
            ui->game->cells[event->x + event->y * ui->game->width] = false;
        break;
    default:
        break;
    }
}

void process_mouse_motion_event(UI *ui, SDL_MouseMotionEvent *event)
{
    if (ui->is_LMB_pressed)
        ui->game->cells[event->x + event->y * ui->game->width] = 1;
    else if (ui->is_RMB_pressed)
        ui->game->cells[event->x + event->y * ui->game->width] = 0;
}

void process_events(UI *ui)
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_KEYDOWN:
            process_keyboard_event(ui, &event.key);
            break;
        case SDL_MOUSEBUTTONDOWN:
            process_mouse_event(ui, &event.button, true);
            break;
        case SDL_MOUSEBUTTONUP:
            process_mouse_event(ui, &event.button, false);
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

void draw(UI *ui)
{
    for (size_t i = 0; i < ui->game->count; ++i)
        ui->pixels[i] = ui->game->cells[i] ? ui->primary_color : ui->background_color;

    SDL_UpdateTexture(ui->texture, NULL, ui->pixels, ui->game->width * sizeof(uint32_t));
    SDL_RenderCopy(ui->renderer, ui->texture, NULL, NULL);
    SDL_RenderPresent(ui->renderer);
}

void ui_init(UI *ui, Game *game, uint8_t max_FPS, uint32_t primary_color, uint32_t background_color)
{

    ui->game = game;

    SDL_CreateWindowAndRenderer(ui->game->width, ui->game->height, 0, &ui->window, &ui->renderer);
    ui->texture = SDL_CreateTexture(ui->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, ui->game->width, ui->game->height);

    SDL_SetWindowFullscreen(ui->window, SDL_WINDOW_FULLSCREEN);

    ui->pixels = malloc(sizeof(uint32_t) * ui->game->count);

    ui->filling_percentage = 20;

    ui->max_FPS = max_FPS;

    ui->is_running = true;
    ui->is_paused = false;

    ui->is_LMB_pressed = false;
    ui->is_RMB_pressed = false;

    ui->primary_color = primary_color;
    ui->background_color = background_color;

    fill_field(ui);
}

void ui_deinit(UI *ui)
{
    free(ui->pixels);

    SDL_DestroyTexture(ui->texture);
    SDL_DestroyRenderer(ui->renderer);
    SDL_DestroyWindow(ui->window);

    SDL_Quit();
}

void ui_run(UI *ui)
{
    double max_delay = 1000.0 / ui->max_FPS;

    uint32_t prev = SDL_GetTicks();

    while (ui->is_running)
    {
        process_events(ui);
        draw(ui);

        uint32_t curr = SDL_GetTicks();
        uint32_t delta = curr - prev;

        if (delta >= max_delay)
        {
            if (!(ui->is_paused))
                game_step(ui->game);

            prev = curr;
        }
    }
}
