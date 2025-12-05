#pragma once

#include "shiny/internal/context.h"

#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_stdinc.h>

void shiny_window_begin(Clay_Context *context, const char *element_id, SDL_FRect size,
	const char *title, Uint16 title_font_size);

void shiny_window_end();
