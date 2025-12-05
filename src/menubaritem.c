#include "shiny/menubaritem.h"
#include "shiny/internal/menubaritem.h"
#include "shiny/internal/element.h"

#include <SDL3/SDL_stdinc.h>

static auto visible = false;
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

void shiny_menubar_item_hide()
{
	visible = false;
}

bool shiny_menubar_item_begin(const char *element_id)
{
	shiny_element_open(element_id);

	Clay_OnHover(on_hover, 0);

	if (!visible)
	{
		return false;
	}

	const Clay_String str = {
		.isStaticallyAllocated = true,
		.length = SDL_strlen(element_id),
		.chars = element_id,
	};
	return Clay__HashString(str, 0).id == current;
}

void shiny_menubar_item_end()
{
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
