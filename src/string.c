#include "shiny/string.h"

#include <SDL3/SDL_stdinc.h>

shiny_string_t shiny_string(const char *str)
{
	shiny_string_t string;
	string.is_static = false;
	string.length = SDL_strlen(str);
	string.value = str;
	return string;
}

shiny_string_t shiny_static_string(const char *str, const Sint32 length)
{
	shiny_string_t string;
	string.is_static = true;
	string.length = length;
	string.value = str;
	return string;
}
