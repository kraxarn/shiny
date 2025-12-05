#include "shiny/theme.h"
#include "shiny/themekey.h"

#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_stdinc.h>

SDL_Color colors[SHINY_COLOR_SIZE];
Uint16 gaps[SHINY_GAP_SIZE];
float corner_radiuses[SHINY_CORNER_RADIUS_SIZE];
Uint16 paddings[SHINY_PADDING_SIZE];
float sizes[SHINY_SIZE_SIZE];

SDL_Color shiny_theme_color(const shiny_theme_color_t key)
{
	return colors[key];
}

void shiny_theme_set_color(const shiny_theme_color_t key, const SDL_Color value)
{
	colors[key] = value;
}

Uint16 shiny_theme_gap(const shiny_theme_gap_t key)
{
	return gaps[key];
}

void shiny_theme_set_gap(const shiny_theme_gap_t key, const Uint16 value)
{
	gaps[key] = value;
}

float shiny_theme_corner_radius(const shiny_theme_corner_radius_t key)
{
	return corner_radiuses[key];
}

void shiny_theme_set_corner_radius(const shiny_theme_corner_radius_t key, const float value)
{
	corner_radiuses[key] = value;
}

Uint16 shiny_theme_padding(const shiny_theme_padding_t key)
{
	return paddings[key];
}

void shiny_theme_set_padding(const shiny_theme_padding_t key, const Uint16 value)
{
	paddings[key] = value;
}

float shiny_theme_size(const shiny_theme_size_t key)
{
	return sizes[key];
}

void shiny_theme_set_size(const shiny_theme_size_t key, const float value)
{
	sizes[key] = value;
}

void shiny_default_theme()
{
	// Oil 6 Palette by GrafxKid
	// https://lospec.com/palette-list/oil-6

	const SDL_Color color1 = {.r = 0xfb, .g = 0xf5, .b = 0xef, .a = SDL_ALPHA_OPAQUE};
	const SDL_Color color2 = {.r = 0xf2, .g = 0xd3, .b = 0xab, .a = SDL_ALPHA_OPAQUE};
	const SDL_Color color3 = {.r = 0xc6, .g = 0x9f, .b = 0xa5, .a = SDL_ALPHA_OPAQUE};
	const SDL_Color color4 = {.r = 0x8b, .g = 0x6d, .b = 0x9c, .a = SDL_ALPHA_OPAQUE};
	const SDL_Color color5 = {.r = 0x49, .g = 0x4d, .b = 0x7e, .a = SDL_ALPHA_OPAQUE};
	const SDL_Color color6 = {.r = 0x27, .g = 0x27, .b = 0x44, .a = SDL_ALPHA_OPAQUE};

	shiny_theme_set_color(SHINY_COLOR_FOREGROUND, color1);
	shiny_theme_set_color(SHINY_COLOR_WINDOW_BACKGROUND, color5);
	shiny_theme_set_color(SHINY_COLOR_WINDOW_HEADER, color4);
	shiny_theme_set_color(SHINY_COLOR_WINDOW_BORDER, color6);
	shiny_theme_set_color(SHINY_COLOR_CONTROL_BACKGROUND, color6);
	shiny_theme_set_color(SHINY_COLOR_CONTROL_HOVER, color4);
	shiny_theme_set_color(SHINY_COLOR_CONTROL_ACTIVE, color5);
	shiny_theme_set_color(SHINY_COLOR_CONTROL_CURSOR, color2);
	shiny_theme_set_color(SHINY_COLOR_CLEAR, color6);
	shiny_theme_set_color(SHINY_COLOR_BORDER, color6);

	shiny_theme_set_gap(SHINY_GAP_DEFAULT, 5);
	shiny_theme_set_gap(SHINY_GAP_MENUBAR, 20);
	shiny_theme_set_gap(SHINY_GAP_WINDOW_ITEM, 15);

	shiny_theme_set_corner_radius(SHINY_CORNER_RADIUS_DEFAULT, 4);
	shiny_theme_set_corner_radius(SHINY_CORNER_RADIUS_WINDOW, 8);
	shiny_theme_set_corner_radius(SHINY_CORNER_RADIUS_CONTROL, 6);

	shiny_theme_set_padding(SHINY_PADDING_MENUBAR, 10);
	shiny_theme_set_padding(SHINY_PADDING_MENU_ITEM, 10);
	shiny_theme_set_padding(SHINY_PADDING_WINDOW_HEADER_Y, 8);
	shiny_theme_set_padding(SHINY_PADDING_WINDOW_HEADER_X, 12);
	shiny_theme_set_padding(SHINY_PADDING_WINDOW_CONTENT, 15);
	shiny_theme_set_padding(SHINY_PADDING_CONTROL, 6);
	shiny_theme_set_padding(SHINY_PADDING_COMBOBOX, 8);

	shiny_theme_set_size(SHINY_WIDTH_COMBOBOX, 180);
	shiny_theme_set_size(SHINY_HEIGHT_COMBOBOX, 21
		+ (shiny_theme_padding(SHINY_PADDING_CONTROL) * 2));
}
