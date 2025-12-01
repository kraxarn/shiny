#include "shiny/internal/element.h"
#include "shiny/themekey.h"
#include "shiny/internal/color.h"

#include "clay.h"

#include <SDL3/SDL_stdinc.h>

void shiny_element_open(Clay_Context *context, const char *element_id)
{
	Clay_SetCurrentContext(context);

	if (element_id == nullptr)
	{
		Clay__OpenElement();
		return;
	}

	const Clay_String str = {
		.isStaticallyAllocated = true,
		.length = SDL_strlen(element_id),
		.chars = element_id
	};
	Clay__OpenElementWithId(CLAY_SID(str));
}

void shiny_text_element_open(Clay_Context *context, const char *text, const Uint16 font_size)
{
	Clay_SetCurrentContext(context);

	const Clay_String str = {
		.isStaticallyAllocated = false,
		.length = SDL_strlen(text),
		.chars = text
	};

	const Clay_TextElementConfig text_config = {
		.fontSize = font_size,
		.textColor = shiny_clay_theme_color(SHINY_COLOR_FOREGROUND),
	};

	Clay__OpenTextElement(str, Clay__StoreTextElementConfig(text_config));
}

void shiny_element_configure(const Clay_ElementDeclaration *declaration)
{
	Clay__ConfigureOpenElementPtr(declaration);
}

void shiny_element_close()
{
	Clay__CloseElement();
}
