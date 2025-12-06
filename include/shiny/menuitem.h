#pragma once

#include "shiny/size.h"

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>

bool shiny_menu_item(const char *element_id, const char *text, Uint16 font_size);

bool shiny_menu_item_icon(const char *element_id, const char *text, Uint16 font_size,
	SDL_Texture *icon, const shiny_size_t *icon_size);
