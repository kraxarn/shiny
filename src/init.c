#include "shiny/init.h"
#include "shiny/font.h"
#include "shiny/internal/logcategory.h"

#include "clay.h"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>

#define SDL3_CLAY_NO_SDL_TTF
#include "renderers/SDL3/clay_renderer_SDL3.h"

typedef struct shiny_state_t
{
	SDL_Renderer *renderer;
	shiny_font_t **fonts;
	Uint8 fonts_size;
	Clay_Arena arena;
	Clay_Context *context;
	SDL_Rect safe_area;
} shiny_state_t;

static Clay_Dimensions measure_text(const Clay_StringSlice text, Clay_TextElementConfig *config, void *user_data)
{
	const auto state = (shiny_state_t *) user_data;
	const shiny_font_t *font = state->fonts[config->fontId];

	Clay_Dimensions dimensions;
	shiny_font_measure_text(font, text.chars, text.length, config->fontSize,
		&dimensions.width, &dimensions.height);

	return dimensions;
}

static void handle_clay_error(const Clay_ErrorData data)
{
	SDL_LogError(SHINY_LOG_CATEGORY_CORE, "Internal error %d: %s", data.errorType, data.errorText.chars);
}

shiny_state_t *shiny_state_create(SDL_Renderer *renderer)
{
	shiny_state_t *state = SDL_malloc(sizeof(shiny_state_t));
	if (state == nullptr)
	{
		return nullptr;
	}

	state->renderer = renderer;
	state->fonts = nullptr;
	state->fonts_size = 0;

	SDL_Window *window = SDL_GetRenderWindow(renderer);
	if (window == nullptr || !SDL_GetWindowSafeArea(window, &state->safe_area))
	{
		SDL_free(state);
		return nullptr;
	}

	int render_width;
	int render_height;
	if (!SDL_GetRenderOutputSize(renderer, &render_width, &render_height))
	{
		SDL_free(state);
		return nullptr;
	}

	const Uint32 mem_size = Clay_MinMemorySize();
	state->arena = Clay_CreateArenaWithCapacityAndMemory(mem_size, SDL_malloc(mem_size));

	const Clay_Dimensions dimensions = {
		.width = (float) render_width,
		.height = (float) render_height,
	};
	const Clay_ErrorHandler error_handler = {
		.errorHandlerFunction = handle_clay_error,
	};
	state->context = Clay_Initialize(state->arena, dimensions, error_handler);

	Clay_SetCurrentContext(state->context);
	Clay_SetMeasureTextFunction(measure_text, (void *) state);

	return state;
}

void shiny_state_destroy(shiny_state_t *state)
{
	for (auto i = 0; i < state->fonts_size; i++)
	{
		shiny_font_destroy(state->fonts[i]);
	}

	SDL_free((void *) state->arena.memory);
	SDL_free((void *) state->fonts);
	SDL_free((void *) state);
}

Clay_Context *shiny_state_clay_context(const shiny_state_t *state)
{
	return state->context;
}

bool shiny_state_add_font(shiny_state_t *state, shiny_font_t *font)
{
	if (state->fonts == nullptr)
	{
		state->fonts = (shiny_font_t **) SDL_malloc(sizeof(shiny_font_t *));
		if (state->fonts == nullptr)
		{
			return false;
		}

		state->fonts[0] = font;
		state->fonts_size++;
		return true;
	}

	const auto fonts = (shiny_font_t **) SDL_realloc(
		(void *) state->fonts,
		(state->fonts_size + 1) * sizeof(shiny_font_t **)
	);
	if (fonts == nullptr)
	{
		return false;
	}

	fonts[state->fonts_size + 1] = font;
	state->fonts = fonts;
	state->fonts_size++;
	return true;
}

shiny_font_t *shiny_state_font(const shiny_state_t *state, const Uint16 font_id)
{
	if (font_id < 0 || font_id >= state->fonts_size)
	{
		SDL_SetError("Invalid font id: %d", font_id);
		return nullptr;
	}

	return state->fonts[font_id];
}

void shiny_state_event(shiny_state_t *state, const float delta_time, const SDL_Event *event)
{
	Clay_SetCurrentContext(state->context);

	if (event->type == SDL_EVENT_WINDOW_RESIZED)
	{
		const Clay_Dimensions dimensions = {
			.width = (float) event->window.data1,
			.height = (float) event->window.data2,
		};
		Clay_SetLayoutDimensions(dimensions);
		return;
	}

	if (event->type == SDL_EVENT_WINDOW_SAFE_AREA_CHANGED)
	{
		SDL_Window *window = SDL_GetRenderWindow(state->renderer);
		if (window == nullptr || !SDL_GetWindowSafeArea(window, &state->safe_area))
		{
			SDL_LogError(SHINY_LOG_CATEGORY_CORE, "Safe area failed: %s", SDL_GetError());
		}
		return;
	}

	if (event->type == SDL_EVENT_MOUSE_MOTION
		|| event->type == SDL_EVENT_MOUSE_BUTTON_DOWN
		|| event->type == SDL_EVENT_MOUSE_BUTTON_UP)
	{
		const Clay_Vector2 position = {
			.x = event->motion.x,
			.y = event->motion.y,
		};
		const bool down = (event->motion.state & SDL_BUTTON_LEFT) > 0;
		Clay_SetPointerState(position, down);
		return;
	}

	if (event->type == SDL_EVENT_MOUSE_WHEEL)
	{
		const Clay_Vector2 delta = {
			.x = event->wheel.x,
			.y = event->wheel.y,
		};
		Clay_UpdateScrollContainers(true, delta, delta_time);
		return;
	}

	if (event->type == SDL_EVENT_KEY_DOWN
		&& event->key.key == SDLK_I)
	{
		Clay_SetDebugModeEnabled((bool) !Clay_IsDebugModeEnabled());
		return;
	}
}

void shiny_state_render_begin(const shiny_state_t *state)
{
	Clay_SetCurrentContext(state->context);
	Clay_BeginLayout();
}

void shiny_state_render_end(const shiny_state_t *state)
{
	Clay_RenderCommandArray commands = Clay_EndLayout();

	// TODO: This is probably not a good way to override font rendering
	for (auto i = 0; i < commands.length; i++)
	{
		Clay_RenderCommand *cmd = Clay_RenderCommandArray_Get(&commands, i);
		if (cmd->commandType == CLAY_RENDER_COMMAND_TYPE_TEXT)
		{
			const Clay_TextRenderData *data = &cmd->renderData.text;
			const shiny_font_t *font = shiny_state_font(state, data->fontId);
			if (!shiny_font_draw_text(font, cmd->boundingBox.x, cmd->boundingBox.y, data->fontSize,
				data->stringContents.chars, data->stringContents.length))
			{
				SDL_LogError(SHINY_LOG_CATEGORY_CORE, "Text draw failed: %s", SDL_GetError());
			}

			continue;
		}

		Clay_RenderCommandArray temp = {
			.capacity = commands.capacity,
			.length = 1,
			.internalArray = cmd,
		};
		Clay_SDL3RendererData renderer_data = {
			.renderer = state->renderer,
		};
		SDL_Clay_RenderClayCommands(&renderer_data, &temp);
	}
}
