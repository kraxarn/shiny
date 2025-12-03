#pragma once

#include <SDL3/SDL_stdinc.h>

typedef struct shiny_string_t shiny_string_t;

#define SHINY_STRING(str) shiny_static_string((str), sizeof((str)) / sizeof((str)[0]) - sizeof((str)[0]))

shiny_string_t shiny_string(const char *str);

shiny_string_t shiny_static_string(const char *str, Sint32 length);
