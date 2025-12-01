#pragma once

#include "clay.h"

#include <SDL3/SDL_stdinc.h>

void shiny_element_open(Clay_Context *context, const char *element_id);

void shiny_text_element_open(Clay_Context *context, const char *text, Uint16 font_size);

void shiny_element_configure(const Clay_ElementDeclaration *declaration);

void shiny_element_close();
