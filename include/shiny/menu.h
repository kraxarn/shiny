#pragma once

#include <SDL3/SDL_stdinc.h>

bool shiny_menu_begin(const char *element_id, const char *text, Uint16 font_size);

void shiny_menu_end();

void shiny_menubar_content_begin();

void shiny_menubar_content_end();
