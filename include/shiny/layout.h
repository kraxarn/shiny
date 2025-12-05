#pragma once

#include <SDL3/SDL_stdinc.h>

typedef enum shiny_layout_flag_t: Uint16
{
	SHINY_LAYOUT_NONE = 0,

	// Layout direction
	SHINY_LAYOUT_LEFT_TO_RIGHT = 1 << 0,
	SHINY_LAYOUT_TOP_TO_BOTTOM = 1 << 1,

	// Child alignment
	SHINY_ALIGN_X_LEFT   = 1 << 2,
	SHINY_ALIGN_X_RIGHT  = 1 << 3,
	SHINY_ALIGN_X_CENTER = 1 << 4,
	SHINY_ALIGN_Y_TOP    = 1 << 5,
	SHINY_ALIGN_Y_BOTTOM = 1 << 6,
	SHINY_ALIGN_Y_CENTER = 1 << 7,

	// Sizing
	SHINY_SIZE_GROW_X = 1 << 8,
	SHINY_SIZE_GROW_Y = 1 << 9,

	// Padding
	SHINY_PADDING_DEFAULT = 1 << 10,
} shiny_layout_flag_t;

void shiny_layout_begin(const char *element_id, shiny_layout_flag_t flags);

void shiny_layout_end();
