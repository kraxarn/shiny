#pragma once

#include "shiny/internal/context.h"

#include <SDL3/SDL_stdinc.h>

bool shiny_combobox_begin(Clay_Context *context, const char *element_id, const char *value, Uint16 font_size);

void shiny_combobox_end();
