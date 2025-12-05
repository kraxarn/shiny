#pragma once

#include "clay.h"

#include <SDL3/SDL_stdinc.h>

void shiny_element_open(const char *element_id);

void shiny_text_element_open(const char *text, Uint16 font_size);

void shiny_element_configure(const Clay_ElementDeclaration *declaration);

void shiny_element_close();

Uint32 shiny_element_hash_id(const char *element_id);
