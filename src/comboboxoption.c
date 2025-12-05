#include "shiny/comboboxoption.h"
#include "shiny/theme.h"
#include "shiny/themekey.h"
#include "shiny/internal/color.h"
#include "shiny/internal/combobox.h"
#include "shiny/internal/element.h"

#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_stdinc.h>

static SDL_PropertiesID states;

static void on_hover(const Clay_ElementId element_id,
	const Clay_PointerData pointer_data, [[maybe_unused]] intptr_t user_data)
{
	if (states == 0)
	{
		states = SDL_CreateProperties();
	}

	SDL_SetNumberProperty(states, element_id.stringId.chars, pointer_data.state);
}

bool shiny_combobox_option(const char *element_id, const char *value, const Uint16 font_size)
{
	shiny_element_open(nullptr);
	{
		const Clay_ElementDeclaration wrapper = {
			.layout = (Clay_LayoutConfig){
				.layoutDirection = CLAY_LEFT_TO_RIGHT,
				.padding = CLAY_PADDING_ALL(shiny_theme_padding(SHINY_PADDING_COMBOBOX)/2),
				.sizing = (Clay_Sizing){
					.width = CLAY_SIZING_GROW(0),
				},
			},
		};
		shiny_element_configure(&wrapper);

		shiny_element_open(element_id);
		{
			const Clay_ElementDeclaration content = {
				.layout = (Clay_LayoutConfig){
					.layoutDirection = CLAY_LEFT_TO_RIGHT,
					.padding = CLAY_PADDING_ALL(shiny_theme_padding(SHINY_PADDING_COMBOBOX)/2),
					.sizing = (Clay_Sizing){
						.width = CLAY_SIZING_GROW(0),
					},
				},
				.cornerRadius = CLAY_CORNER_RADIUS(shiny_theme_corner_radius(SHINY_CORNER_RADIUS_CONTROL)),
				.backgroundColor = shiny_clay_theme_color((int) Clay_Hovered()
					? SHINY_COLOR_CONTROL_ACTIVE
					: SHINY_COLOR_CONTROL_BACKGROUND
				),
			};
			shiny_element_configure(&content);

			Clay_OnHover(on_hover, 0);
			shiny_text_element_open(value, font_size);
		}
		shiny_element_close();
	}
	shiny_element_close();

	const Clay_PointerDataInteractionState state = SDL_GetNumberProperty(states, element_id, -1);
	if (state != CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
	{
		return false;
	}

	SDL_ClearProperty(states, element_id);
	shiny_combobox_hide();
	return true;
}
