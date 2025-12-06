#include "shiny/menuitem.h"
#include "shiny/image.h"
#include "shiny/size.h"
#include "shiny/theme.h"
#include "shiny/themekey.h"
#include "shiny/internal/color.h"
#include "shiny/internal/element.h"
#include "shiny/internal/logcategory.h"
#include "shiny/internal/menu.h"

#include "clay.h"

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>

static SDL_PropertiesID states;

static void on_hover([[maybe_unused]] const Clay_ElementId element_id,
	const Clay_PointerData pointer_data, [[maybe_unused]] intptr_t user_data)
{
	if (states == 0)
	{
		states = SDL_CreateProperties();
	}

	SDL_SetNumberProperty(states, element_id.stringId.chars, pointer_data.state);
}

void menu_item_begin(const char *element_id, const char *text, const Uint16 font_size,
	SDL_Texture *icon, const shiny_size_t *icon_size)
{
	// We need an element ID for hashing
	if (element_id == nullptr)
	{
		SDL_LogError(SHINY_LOG_CATEGORY_ELEMENT, "Menu item requires element ID");
		return;
	}

	shiny_element_open(element_id);

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

	shiny_element_open(nullptr);

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

	if (icon != nullptr)
	{
		shiny_image(icon, icon_size);
	}
	shiny_text_element_open(text, font_size);
}

static void menu_item_end()
{
	shiny_element_close(); // content
	shiny_element_close(); // container
}

static bool clicked(const char *element_id)
{
	const Clay_PointerDataInteractionState state = SDL_GetNumberProperty(states, element_id, -1);
	if (state != CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
	{
		return false;
	}

	SDL_ClearProperty(states, element_id);
	shiny_menu_hide();
	return true;
}

bool shiny_menu_item(const char *element_id, const char *text, const Uint16 font_size)
{
	menu_item_begin(element_id, text, font_size, nullptr, nullptr);
	menu_item_end();
	return clicked(element_id);
}

bool shiny_menu_item_icon(const char *element_id, const char *text, Uint16 font_size, SDL_Texture *icon,
	const shiny_size_t *icon_size)
{
	menu_item_begin(element_id, text, font_size, icon, icon_size);
	menu_item_end();
	return clicked(element_id);
}
