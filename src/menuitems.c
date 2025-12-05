#include "shiny/menuitems.h"
#include "shiny/theme.h"
#include "shiny/themekey.h"
#include "shiny/internal/color.h"
#include "shiny/internal/element.h"

#include "clay.h"

void shiny_menu_items_begin()
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

void shiny_menu_items_end()
{
	shiny_element_close();
}
