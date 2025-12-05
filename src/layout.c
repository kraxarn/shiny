#include "shiny/layout.h"
#include "shiny/theme.h"
#include "shiny/themekey.h"
#include "shiny/internal/element.h"

#include "clay.h"

static constexpr shiny_layout_flag_t layout_direction =
	SHINY_LAYOUT_LEFT_TO_RIGHT
	| SHINY_LAYOUT_TOP_TO_BOTTOM;

static constexpr shiny_layout_flag_t child_align_x =
	SHINY_ALIGN_X_LEFT
	| SHINY_ALIGN_X_CENTER
	| SHINY_ALIGN_X_RIGHT;

static constexpr shiny_layout_flag_t child_align_y =
	SHINY_ALIGN_Y_TOP
	| SHINY_ALIGN_Y_CENTER
	| SHINY_ALIGN_Y_BOTTOM;

static constexpr shiny_layout_flag_t padding =
	SHINY_PADDING_DEFAULT;

static int map_enum(const shiny_layout_flag_t flag)
{
	switch (flag)
	{
		case SHINY_LAYOUT_NONE:
			return 0;

		case SHINY_LAYOUT_LEFT_TO_RIGHT:
			return CLAY_LEFT_TO_RIGHT;

		case SHINY_LAYOUT_TOP_TO_BOTTOM:
			return CLAY_TOP_TO_BOTTOM;

		case SHINY_ALIGN_X_LEFT:
			return CLAY_ALIGN_X_LEFT;

		case SHINY_ALIGN_X_RIGHT:
			return CLAY_ALIGN_X_RIGHT;

		case SHINY_ALIGN_X_CENTER:
			return CLAY_ALIGN_X_CENTER;

		case SHINY_ALIGN_Y_TOP:
			return CLAY_ALIGN_Y_TOP;

		case SHINY_ALIGN_Y_BOTTOM:
			return CLAY_ALIGN_Y_BOTTOM;

		case SHINY_ALIGN_Y_CENTER:
			return CLAY_ALIGN_Y_CENTER;

		default:
			return -1;
	}
}

static Clay__SizingType map_sizing_type(const shiny_layout_flag_t flag)
{
	return flag > 0 ? CLAY__SIZING_TYPE_GROW : CLAY__SIZING_TYPE_FIT;
}

static Clay_Padding map_padding(const shiny_layout_flag_t flag)
{
	if (flag == SHINY_PADDING_DEFAULT)
	{
		return (Clay_Padding){
			.left = shiny_theme_gap(SHINY_GAP_DEFAULT),
			.right = shiny_theme_gap(SHINY_GAP_DEFAULT),
			.top = shiny_theme_gap(SHINY_GAP_DEFAULT),
			.bottom = shiny_theme_gap(SHINY_GAP_DEFAULT),
		};
	}

	return (Clay_Padding){};
}

void shiny_layout_begin(Clay_Context *context, const char *element_id, const shiny_layout_flag_t flags)
{
	shiny_element_open(context, element_id);

	const Clay_ElementDeclaration element = {
		.layout = (Clay_LayoutConfig){
			.layoutDirection = map_enum(flags & layout_direction),
			.childAlignment = (Clay_ChildAlignment){
				.x = map_enum(flags & child_align_x),
				.y = map_enum(flags & child_align_y),
			},
			.sizing = (Clay_Sizing){
				.width = (Clay_SizingAxis){
					.type = map_sizing_type(flags & SHINY_SIZE_GROW_X),
				},
				.height = (Clay_SizingAxis){
					.type = map_sizing_type(flags & SHINY_SIZE_GROW_Y),
				},
			},
			.padding = map_padding(flags & padding),
		},
	};
	shiny_element_configure(&element);
}

void shiny_layout_end()
{
	shiny_element_close();
}
