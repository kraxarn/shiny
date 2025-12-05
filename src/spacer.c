#include "shiny/spacer.h"
#include "shiny/internal/element.h"

void shiny_v_spacer()
{
	shiny_element_open(nullptr);

	const Clay_ElementDeclaration element = {
		.layout = (Clay_LayoutConfig){
			.sizing = (Clay_Sizing){
				.height = CLAY_SIZING_GROW(0),
			},
		},
	};
	shiny_element_configure(&element);

	shiny_element_close();
}

void shiny_h_spacer()
{
	shiny_element_open(nullptr);

	const Clay_ElementDeclaration element = {
		.layout = (Clay_LayoutConfig){
			.sizing = (Clay_Sizing){
				.width = CLAY_SIZING_GROW(0),
			},
		},
	};
	shiny_element_configure(&element);

	shiny_element_close();
}
