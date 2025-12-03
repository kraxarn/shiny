#include "shiny/string.h"

#include "clay.h"

#include <SDL3/SDL_stdinc.h>

typedef struct shiny_string_t
{
	Clay_String value;
} shiny_string_t;

shiny_string_t shiny_string(const char *str)
{
	shiny_string_t string;
	string.value = (Clay_String){
		// This doesn't necessarily have to be true
		.isStaticallyAllocated = false,
		.length = SDL_strlen(str),
		.chars = str,
	};
	return string;
}
