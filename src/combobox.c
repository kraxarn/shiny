#include "shiny/combobox.h"
#include "shiny/spacer.h"
#include "shiny/theme.h"
#include "shiny/themekey.h"
#include "shiny/internal/color.h"
#include "shiny/internal/element.h"

#include "clay.h"

#include <SDL3/SDL_stdinc.h>

static Uint32 current_open_element_id = 0;
static Uint32 current_element_id;

static void on_hover(const Clay_ElementId element_id, const Clay_PointerData pointer_data,
	[[maybe_unused]] intptr_t user_data)
{
	if (pointer_data.state != CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
	{
		return;
	}

	if (current_open_element_id == 0 || current_open_element_id != element_id.id)
	{
		current_open_element_id = element_id.id;
	}
	else
	{
		current_open_element_id = 0;
	}
}

bool shiny_combobox_begin(Clay_Context *context, const char *element_id,
	const char *value, const Uint16 font_size, const int item_count)
{
	shiny_element_open(context, element_id);
	current_element_id = shiny_element_hash_id(element_id);
	const bool is_open = current_element_id == current_open_element_id;

	const Clay_ElementDeclaration container = {
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
	shiny_element_configure(&container);

	shiny_text_element_open(context, value, font_size);
	shiny_h_spacer(context);
	shiny_text_element_open(context, (int) is_open ? "^" : "V", font_size); // TODO: Icons

	Clay_OnHover(on_hover, 0);

	if (is_open)
	{
		shiny_element_open(context, nullptr);

		const Clay_ElementDeclaration options = {
			.floating = (Clay_FloatingElementConfig){
				.attachTo = CLAY_ATTACH_TO_PARENT,
				.attachPoints = (Clay_FloatingAttachPoints){
					.parent = CLAY_ATTACH_POINT_LEFT_BOTTOM,
				},
			},
			.layout = (Clay_LayoutConfig){
				.layoutDirection = CLAY_TOP_TO_BOTTOM,
				.sizing = (Clay_Sizing){
					.width = CLAY_SIZING_FIXED(shiny_theme_size(SHINY_WIDTH_COMBOBOX)),
					.height = CLAY_SIZING_GROW(shiny_theme_size(SHINY_HEIGHT_COMBOBOX) * item_count),
				},
			},
			.backgroundColor = shiny_clay_theme_color(SHINY_COLOR_CONTROL_BACKGROUND),
			.cornerRadius = (Clay_CornerRadius){
				.bottomLeft = shiny_theme_corner_radius(SHINY_CORNER_RADIUS_CONTROL),
				.bottomRight = shiny_theme_corner_radius(SHINY_CORNER_RADIUS_CONTROL),
			},
		};
		shiny_element_configure(&options);
	}

	return is_open;
}

void shiny_combobox_end()
{
	if (current_open_element_id == current_element_id)
	{
		shiny_element_close(); // options
	}

	shiny_element_close(); // combobox
}
