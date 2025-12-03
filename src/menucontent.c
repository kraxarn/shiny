#include "shiny/menucontent.h"
#include "shiny/theme.h"
#include "shiny/themekey.h"
#include "shiny/internal/element.h"

#include "clay.h"

void shiny_menu_content_begin(Clay_Context *context)
{
	shiny_element_open(context, nullptr);

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

void shiny_menu_content_end()
{
	shiny_element_close();
}
