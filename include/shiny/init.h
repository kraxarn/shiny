#pragma once

#include "shiny/font.h"
#include "shiny/internal/context.h"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>

typedef struct shiny_state_t shiny_state_t;

shiny_state_t *shiny_state_create(SDL_Renderer *renderer);

void shiny_state_destroy(shiny_state_t *state);

Clay_Context *shiny_state_clay_context(const shiny_state_t *state);

bool shiny_state_add_font(shiny_state_t *state, shiny_font_t *font);

shiny_font_t *shiny_state_font(const shiny_state_t *state, Uint16 font_id);

void shiny_state_event(shiny_state_t *state, float delta_time, const SDL_Event *event);

void shiny_state_render_begin(const shiny_state_t *state);

void shiny_state_render_end(const shiny_state_t *state);
