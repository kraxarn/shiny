#include "shiny/string.h"
#include "shiny/internal/string.h"

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

Clay_String shiny_clay_string(const shiny_string_t string)
{
	return (Clay_String){
		.isStaticallyAllocated = string.is_static,
		.length = string.length,
		.chars = string.value,
	};
}
