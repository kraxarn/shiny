#include "shiny/label.h"
#include "shiny/internal/element.h"

#include <SDL3/SDL_stdinc.h>

void shiny_label(const char *text, const Uint16 font_size)
{
	shiny_text_element_open(text, font_size);
}
