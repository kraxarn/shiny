#include "shiny/menubar.h"
#include "shiny/theme.h"
#include "shiny/themekey.h"
#include "shiny/internal/color.h"
#include "shiny/internal/element.h"

#include "clay.h"

void shiny_menubar_begin(Clay_Context *context, const char *element_id)
{
	shiny_element_open(context, element_id);

	const Clay_ElementDeclaration element = {
		.layout = (Clay_LayoutConfig){
			.layoutDirection = CLAY_LEFT_TO_RIGHT,
			.sizing = (Clay_Sizing){
				.width = CLAY_SIZING_GROW(0),
				.height = CLAY_SIZING_FIXED(0),
			},
			.padding = CLAY_PADDING_ALL(shiny_theme_padding(SHINY_PADDING_MENUBAR)),
			.childGap = shiny_theme_gap(SHINY_GAP_MENUBAR),
		},
		.cornerRadius = CLAY_CORNER_RADIUS(shiny_theme_corner_radius(SHINY_CORNER_RADIUS_DEFAULT)),
		.backgroundColor = shiny_clay_theme_color(SHINY_COLOR_WINDOW_BACKGROUND),
	};
	shiny_element_configure(&element);
}

void shiny_menubar_end()
{
	shiny_element_close();
}
