#pragma once

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>

typedef struct shiny_font_t shiny_font_t;

shiny_font_t *shiny_font_create(SDL_Renderer *renderer, const Uint8 *data, float font_size);

void shiny_font_destroy(shiny_font_t *font);

bool shiny_font_draw_text(const shiny_font_t *font, float x, float y, const char *text, Sint32 length);
