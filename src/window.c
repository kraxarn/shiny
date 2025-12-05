#include "shiny/window.h"
#include "shiny/theme.h"
#include "shiny/themekey.h"
#include "shiny/internal/color.h"
#include "shiny/internal/element.h"

#include "clay.h"

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_stdinc.h>

#include "shiny/internal/logcategory.h"

static SDL_PropertiesID offsets = 0;
static SDL_PropertiesID positions = 0;

static void cleanup([[maybe_unused]] void *userdata, void *value)
{
	SDL_free(value);
}

static void on_window_title_hover(const Clay_ElementId element_id, const Clay_PointerData pointer_data,
	[[maybe_unused]] const intptr_t user_data)
{
	const auto prop_name = (const char *) user_data;

	if (offsets == 0)
	{
		offsets = SDL_CreateProperties();
	}

	if (positions == 0)
	{
		positions = SDL_CreateProperties();
	}

	if (pointer_data.state == CLAY_POINTER_DATA_PRESSED)
	{
		SDL_FPoint *offset = SDL_GetPointerProperty(offsets, prop_name, nullptr);
		if (offset == nullptr)
		{
			offset = SDL_malloc(sizeof(SDL_FPoint));
			SDL_SetPointerPropertyWithCleanup(offsets, prop_name, offset, cleanup, nullptr);

			const auto data = Clay_GetElementData(element_id);
			offset->x = pointer_data.position.x - data.boundingBox.x;
			offset->y = pointer_data.position.y - data.boundingBox.y;
		}

		SDL_FPoint *position = SDL_GetPointerProperty(positions, prop_name, nullptr);
		if (position == nullptr)
		{
			position = SDL_malloc(sizeof(SDL_FPoint));
			SDL_SetPointerPropertyWithCleanup(positions, prop_name, position, cleanup, nullptr);
		}

		position->x = pointer_data.position.x - offset->x;
		position->y = pointer_data.position.y - offset->y;
		SDL_LogDebug(SHINY_LOG_CATEGORY_ELEMENT, "%.0f %.0f", position->x, position->y);
	}

	if (pointer_data.state == CLAY_POINTER_DATA_RELEASED_THIS_FRAME)
	{
		SDL_ClearProperty(offsets, prop_name);
	}
}

static void window_title(Clay_Context *context, const char *element_id,
	const char *title, const Uint16 font_size)
{
	shiny_element_open(context, nullptr);
	{
		const Clay_ElementDeclaration element = {
			.layout = (Clay_LayoutConfig){
				.sizing = (Clay_Sizing){
					.width = CLAY_SIZING_GROW(0),
				},
				.padding = (Clay_Padding){
					.left = shiny_theme_padding(SHINY_PADDING_WINDOW_HEADER_X),
					.right = shiny_theme_padding(SHINY_PADDING_WINDOW_HEADER_X),
					.top = shiny_theme_padding(SHINY_PADDING_WINDOW_HEADER_Y),
					.bottom = shiny_theme_padding(SHINY_PADDING_WINDOW_HEADER_Y),
				},
			},
			.backgroundColor = shiny_clay_theme_color(SHINY_COLOR_WINDOW_HEADER),
			.cornerRadius = (Clay_CornerRadius){
				.topLeft = shiny_theme_corner_radius(SHINY_CORNER_RADIUS_WINDOW),
				.topRight = shiny_theme_corner_radius(SHINY_CORNER_RADIUS_WINDOW),
			},
		};
		shiny_element_configure(&element);

		Clay_OnHover(on_window_title_hover, (intptr_t) element_id);
		shiny_text_element_open(context, title, font_size);
	}
	shiny_element_close();
}

static void window_content_begin(Clay_Context *context)
{
	shiny_element_open(context, nullptr);

	const Clay_ElementDeclaration wrapper = {
		.layout = (Clay_LayoutConfig){
			.layoutDirection = CLAY_TOP_TO_BOTTOM,
			.childGap = shiny_theme_gap(SHINY_GAP_WINDOW_ITEM),
			.sizing = (Clay_Sizing){
				.width = CLAY_SIZING_GROW(0),
				.height = CLAY_SIZING_GROW(0),
			},
			.padding = CLAY_PADDING_ALL(shiny_theme_padding(SHINY_PADDING_WINDOW_CONTENT)),
		},
		.backgroundColor = shiny_clay_theme_color(SHINY_COLOR_WINDOW_BACKGROUND),
		.cornerRadius = (Clay_CornerRadius){
			.bottomLeft = shiny_theme_corner_radius(SHINY_CORNER_RADIUS_WINDOW),
			.bottomRight = shiny_theme_corner_radius(SHINY_CORNER_RADIUS_WINDOW),
		},
	};
	shiny_element_configure(&wrapper);
}

static void window_content_end()
{
	shiny_element_close();
}

void shiny_window_begin(Clay_Context *context, const char *element_id, const SDL_FRect size,
	const char *title, const Uint16 title_font_size)
{
	shiny_element_open(context, element_id);

	Clay_ElementDeclaration element = {
		.floating = (Clay_FloatingElementConfig){
			.attachTo = CLAY_ATTACH_TO_ROOT,
		},
		.layout = (Clay_LayoutConfig){
			.layoutDirection = CLAY_TOP_TO_BOTTOM,
			.sizing = (Clay_Sizing){
				.width = CLAY_SIZING_FIXED(size.w),
				.height = CLAY_SIZING_FIXED(size.h),
			},
		},
	};

	const SDL_FPoint *position = SDL_GetPointerProperty(positions, element_id, nullptr);
	if (position != nullptr)
	{
		element.floating.attachPoints.parent = CLAY_ATTACH_POINT_LEFT_TOP;
		element.floating.offset.x = position->x;
		element.floating.offset.y = position->y;
	}
	else
	{
		element.floating.attachPoints.parent = CLAY_ATTACH_POINT_CENTER_CENTER;
		element.floating.offset.x = -size.w / 2.F;
		element.floating.offset.y = -size.h / 2.F;
	}

	shiny_element_configure(&element);

	window_title(context, element_id, title, title_font_size);
	window_content_begin(context);
}

void shiny_window_end()
{
	window_content_end();
	shiny_element_close();
}
