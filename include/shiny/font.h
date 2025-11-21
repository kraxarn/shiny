#pragma once

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>

typedef Uint8 shiny_font_id;

bool shiny_font_init(SDL_Renderer *renderer);

bool shiny_font_destroy();

shiny_font_id shiny_font_load(const Uint8 *data, long size);
