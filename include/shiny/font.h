#pragma once

#include "shiny/color.h"

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>

typedef struct shiny_font_t shiny_font_t;

bool shiny_font_init(SDL_Renderer *renderer);

bool shiny_font_destroy();

shiny_font_t *shiny_font_open(const Uint8 *data, long size);

void shiny_font_close(shiny_font_t *font);

bool shiny_font_set_size(const shiny_font_t *font, float size);

void shiny_font_set_color(shiny_font_t *font, shiny_color_t color);
