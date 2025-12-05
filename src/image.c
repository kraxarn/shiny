#include "shiny/image.h"
#include "shiny/size.h"
#include "shiny/internal/element.h"

#include <SDL3/SDL_render.h>

void shiny_image(SDL_Texture *texture, const shiny_size_t *size)
{
	shiny_element_open(nullptr);

	const Clay_ElementDeclaration element = {
		.layout = (Clay_LayoutConfig){
			.sizing = (Clay_Sizing){
				.width = CLAY_SIZING_FIXED(size != nullptr ? size->width : texture->w),
				.height = CLAY_SIZING_FIXED(size != nullptr ? size->height : texture->h),
			},
		},
		.image = (Clay_ImageElementConfig){
			.imageData = texture,
		},
	};
	shiny_element_configure(&element);

	shiny_element_close();
}
