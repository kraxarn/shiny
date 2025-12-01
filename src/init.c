#include "shiny/init.h"
#include "shiny/font.h"

#include "clay.h"

static Clay_Dimensions measure_text(const Clay_StringSlice text, Clay_TextElementConfig *config, void *user_data)
{
	const auto fonts = (shiny_font_t **) user_data;
	const shiny_font_t *font = fonts[config->fontId];

	Clay_Dimensions dimensions;
	shiny_font_measure_text(font, text.chars, text.length, config->fontSize,
		&dimensions.width, &dimensions.height);

	return dimensions;
}

bool shiny_set_fonts(Clay_Context *context, shiny_font_t **fonts)
{
	Clay_SetCurrentContext(context);
	Clay_SetMeasureTextFunction(measure_text, (void *) fonts);
	return true;
}
