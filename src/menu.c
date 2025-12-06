#include "shiny/menu.h"
#include "shiny/internal/menu.h"
#include "shiny/menucontent.h"
#include "shiny/menuitems.h"
#include "shiny/internal/element.h"

#include <SDL3/SDL_stdinc.h>

static auto visible = false;
static bool open;
static Uint32 current = 0;

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

	shiny_menu_content_begin();
	shiny_menu_items_begin();

	open = true;
	return true;
}

void shiny_menu_end()
{
	if (open)
	{
		shiny_menu_items_end();
		shiny_menu_content_end();
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
