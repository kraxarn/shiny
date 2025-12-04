#pragma once

#include "shiny/internal/context.h"

#include <SDL3/SDL_stdinc.h>

bool shiny_combobox_option(Clay_Context *context, const char *element_id,
	const char *value, Uint16 font_size);
