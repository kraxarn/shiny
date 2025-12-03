#include "shiny/menuitem.h"
#include "shiny/theme.h"
#include "shiny/themekey.h"
#include "shiny/internal/color.h"
#include "shiny/internal/element.h"
#include "shiny/internal/logcategory.h"
#include "shiny/internal/menubaritem.h"

#include "clay.h"

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_properties.h>

static SDL_PropertiesID states;

static const char *current_element_id;

static void on_hover([[maybe_unused]] const Clay_ElementId element_id,
	const Clay_PointerData pointer_data, [[maybe_unused]] intptr_t user_data)
{
	if (states == 0)
	{
		states = SDL_CreateProperties();
	}

	SDL_SetNumberProperty(states, element_id.stringId.chars, pointer_data.state);
}

void shiny_menu_item_begin(Clay_Context *context, const char *element_id)
{
	// We need an element ID for hashing
	if (element_id == nullptr)
	{
		SDL_LogError(SHINY_LOG_CATEGORY_ELEMENT, "Menu item requires element ID");
		return;
	}

	shiny_element_open(context, element_id);

	const Clay_ElementDeclaration container = {
		.layout = (Clay_LayoutConfig){
			.sizing = (Clay_Sizing){
				.width = CLAY_SIZING_GROW(0),
			},
		},
	};
	shiny_element_configure(&container);

	if (Clay_Hovered())
	{
		Clay_OnHover(on_hover, 0);
	}

	shiny_element_open(context, nullptr);

	const Clay_ElementDeclaration content = {
		.layout = (Clay_LayoutConfig){
			.sizing = (Clay_Sizing){
				.width = CLAY_SIZING_GROW(0),
			},
			.padding = CLAY_PADDING_ALL(shiny_theme_padding(SHINY_PADDING_MENU_ITEM)),
			.childAlignment = (Clay_ChildAlignment){
				.y = CLAY_ALIGN_Y_CENTER,
			},
			.childGap = shiny_theme_gap(SHINY_GAP_DEFAULT),
		},
		.cornerRadius = CLAY_CORNER_RADIUS(shiny_theme_corner_radius(SHINY_CORNER_RADIUS_DEFAULT)),
		.backgroundColor = shiny_clay_theme_color((int) Clay_Hovered()
			? SHINY_COLOR_CONTROL_HOVER
			: SHINY_COLOR_CONTROL_ACTIVE),
	};
	shiny_element_configure(&content);

	current_element_id = element_id;
}

bool shiny_menu_item_end()
{
	shiny_element_close(); // content
	shiny_element_close(); // container

	const Clay_PointerDataInteractionState state = SDL_GetNumberProperty(states, current_element_id, -1);
	if (state != CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
	{
		return false;
	}

	shiny_menubar_item_hide();
	return true;
}
