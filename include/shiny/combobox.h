#pragma once

#include <SDL3/SDL_stdinc.h>

bool shiny_combobox_begin(const char *element_id, const char *value,
	Uint16 font_size, int item_count);

void shiny_combobox_end();
