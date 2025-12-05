#include "shiny/internal/color.h"
#include "shiny/theme.h"
#include "shiny/themekey.h"

#include <SDL3/SDL_pixels.h>

static Clay_Color shiny_clay_color(const SDL_Color color)
{
	return (Clay_Color){
		.r = color.r,
		.g = color.g,
		.b = color.b,
		.a = color.a,
	};
}

Clay_Color shiny_clay_theme_color(const shiny_theme_color_t color)
{
	return shiny_clay_color(shiny_theme_color(color));
}
