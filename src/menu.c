#include "shiny/menu.h"
#include "shiny/internal/menu.h"
#include "shiny/theme.h"
#include "shiny/themekey.h"
#include "shiny/internal/color.h"
#include "shiny/internal/element.h"

#include <SDL3/SDL_stdinc.h>

static auto visible = false;
static bool open;
static Uint32 current = 0;

static void menu_content_begin()
{
	shiny_element_open(nullptr);

	const Clay_ElementDeclaration element = {
		.floating = (Clay_FloatingElementConfig){
			.attachTo = CLAY_ATTACH_TO_PARENT,
			.attachPoints = (Clay_FloatingAttachPoints){
				.parent = CLAY_ATTACH_POINT_LEFT_BOTTOM,
			},
		},
		.layout = (Clay_LayoutConfig){
			.sizing = (Clay_Sizing){
				.width = CLAY_SIZING_FIXED(200),
			},
			.padding = (Clay_Padding){
				.top = shiny_theme_padding(SHINY_PADDING_MENUBAR)
				+ shiny_theme_gap(SHINY_GAP_DEFAULT),
			},
		},
	};
	shiny_element_configure(&element);
}

static void menu_content_end()
{
	shiny_element_close();
}

static void menu_items_begin()
{
	shiny_element_open(nullptr);

	const Clay_ElementDeclaration element = {
		.layout = (Clay_LayoutConfig){
			.layoutDirection = CLAY_TOP_TO_BOTTOM,
			.sizing = (Clay_Sizing){
				.width = CLAY_SIZING_FIXED(200),
			},
		},
		.cornerRadius = CLAY_CORNER_RADIUS(shiny_theme_corner_radius(SHINY_CORNER_RADIUS_DEFAULT)),
		.backgroundColor = shiny_clay_theme_color(SHINY_COLOR_WINDOW_BACKGROUND),
	};
	shiny_element_configure(&element);
}

static void menu_items_end()
{
	shiny_element_close();
}

static void on_hover(const Clay_ElementId element_id, const Clay_PointerData pointer_data,
	[[maybe_unused]] intptr_t user_data)
{
	current = element_id.id;

	if (pointer_data.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
	{
		visible = (bool) !visible;
	}
}

void shiny_menu_hide()
{
	visible = false;
}

bool shiny_menu_begin(const char *element_id, const char *text, const Uint16 font_size)
{
	shiny_element_open(element_id);

	shiny_text_element_open(text, font_size);

	Clay_OnHover(on_hover, 0);

	if (!visible || shiny_element_hash_id(element_id) != current)
	{
		open = false;
		return false;
	}

	menu_content_begin();
	menu_items_begin();

	open = true;
	return true;
}

void shiny_menu_end()
{
	if (open)
	{
		menu_items_end();
		menu_content_end();
	}

	shiny_element_close();
}

void shiny_menubar_content_begin()
{
	shiny_element_open(nullptr);
}

void shiny_menubar_content_end()
{
	shiny_element_close();
}
