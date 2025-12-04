#include "shiny/combobox.h"
#include "shiny/spacer.h"
#include "shiny/theme.h"
#include "shiny/themekey.h"
#include "shiny/internal/color.h"
#include "shiny/internal/element.h"

#include "clay.h"

#include <SDL3/SDL_stdinc.h>

static Uint32 current = 0;

static void on_hover(const Clay_ElementId element_id, const Clay_PointerData pointer_data,
	[[maybe_unused]] intptr_t user_data)
{
	if (pointer_data.state != CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
	{
		return;
	}

	if (current == 0 || current != element_id.id)
	{
		current = element_id.id;
	}
	else
	{
		current = 0;
	}
}

bool shiny_combobox_begin(Clay_Context *context, const char *element_id, const char *value, const Uint16 font_size)
{
	shiny_element_open(context, element_id);
	const bool is_open = shiny_element_hash_id(element_id) == current;

	const Clay_ElementDeclaration element = {
		.layout = (Clay_LayoutConfig){
			.layoutDirection = CLAY_LEFT_TO_RIGHT,
			.sizing = (Clay_Sizing){
				.width = CLAY_SIZING_FIXED(shiny_theme_size(SHINY_WIDTH_COMBOBOX)),
				.height = CLAY_SIZING_GROW(0),
			},
			.padding = CLAY_PADDING_ALL(shiny_theme_padding(SHINY_PADDING_CONTROL)),
		},
		.backgroundColor = shiny_clay_theme_color(SHINY_COLOR_CONTROL_BACKGROUND),
		.cornerRadius = (Clay_CornerRadius){
			.topLeft = shiny_theme_corner_radius(SHINY_CORNER_RADIUS_CONTROL),
			.topRight = shiny_theme_corner_radius(SHINY_CORNER_RADIUS_CONTROL),
			.bottomLeft = (int) is_open ? 0 : shiny_theme_corner_radius(SHINY_CORNER_RADIUS_CONTROL),
			.bottomRight = (int) is_open ? 0 : shiny_theme_corner_radius(SHINY_CORNER_RADIUS_CONTROL),
		},
	};
	shiny_element_configure(&element);

	shiny_text_element_open(context, value, font_size);
	shiny_h_spacer(context);
	shiny_text_element_open(context, (int) is_open ? "^" : "V", font_size); // TODO: Icons

	Clay_OnHover(on_hover, 0);

	return is_open;
}

void shiny_combobox_end()
{
	shiny_element_close();
}
