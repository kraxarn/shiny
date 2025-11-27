#include "shiny/font.h"
#include "shiny/themekey.h"
#include "shiny/internal/color.h"
#include "shiny/internal/logcategory.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include <SDL3/SDL_blendmode.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>

// ASCII printable characters
static constexpr auto ascii_begin = 32;
static constexpr auto ascii_size = 96;

typedef struct shiny_font_t
{
	const Uint8 *data;
	SDL_Renderer *renderer;
	SDL_Texture *glyphs;

	SDL_Color color;

	stbtt_bakedchar baked_chars[ascii_size];
	stbtt_fontinfo info;

	float size;
	float scale;
	int ascent;
	int descent;
	int line_gap;
	float baseline;
} shiny_font_t;

static Sint64 shiny_font_texture_size(SDL_Renderer *renderer)
{
	const SDL_PropertiesID props = SDL_GetRendererProperties(renderer);
	const char *name = SDL_PROP_RENDERER_MAX_TEXTURE_SIZE_NUMBER;
	constexpr Sint64 default_value = 1024;
	const Sint64 max_size = SDL_GetNumberProperty(props, name, default_value);
	return SDL_min(max_size, default_value);
}

static bool shiny_font_parse(shiny_font_t *font)
{
	if (!stbtt_InitFont(&font->info, font->data, 0))
	{
		return SDL_SetError("Font initialisation error");
	}

	font->scale = stbtt_ScaleForPixelHeight(&font->info, font->size);
	stbtt_GetFontVMetrics(&font->info, &font->ascent, &font->descent, &font->line_gap);
	font->baseline = (float) font->ascent * font->scale;

	return true;
}

static bool shiny_build_palette(SDL_Surface *surface, const SDL_Color color)
{
	constexpr auto color_count = 256; // INDEX8

	SDL_Palette *palette = SDL_CreateSurfacePalette(surface);
	if (palette == nullptr)
	{
		return false;
	}

	SDL_Color colors[color_count];
	for (auto i = 0; i < color_count; i++)
	{
		colors[i].r = color.r;
		colors[i].g = color.g;
		colors[i].b = color.b;
		colors[i].a = i;
	}

	if (!SDL_SetPaletteColors(palette, colors, 0, color_count))
	{
		return false;
	}

	return true;
}

bool shiny_font_bake(shiny_font_t *font)
{
	const Uint64 start = SDL_GetTicks();

	const auto atlas_size = (int) shiny_font_texture_size(font->renderer);
	SDL_LogDebug(SHINY_LOG_CATEGORY_FONT, "Baking font with size %d", atlas_size);

	SDL_Surface *surface = SDL_CreateSurface(atlas_size, atlas_size, SDL_PIXELFORMAT_INDEX8);
	if (surface == nullptr)
	{
		return false;
	}

	if (!shiny_build_palette(surface, font->color))
	{
		return SDL_SetError("Invalid surface palette");
	}

	stbtt_BakeFontBitmap(font->data, 0, font->size, surface->pixels, surface->w, surface->h,
		ascii_begin, ascii_size, font->baked_chars
	);

	for (auto i = 0; i < ascii_size; i++) // TODO: For vertical alignment, but is it needed?
	{
		if (font->baked_chars[i].yoff > font->baseline)
		{
			font->baseline = font->baked_chars[i].yoff;
		}
	}

	font->glyphs = SDL_CreateTextureFromSurface(font->renderer, surface);
	if (font->glyphs == nullptr)
	{
		SDL_DestroySurface(surface);
		return false;
	}

	SDL_DestroySurface(surface);

	if (!SDL_SetTextureBlendMode(font->glyphs, SDL_BLENDMODE_BLEND))
	{
		return false;
	}

	const Uint64 end = SDL_GetTicks();
	SDL_LogDebug(SHINY_LOG_CATEGORY_FONT, "Baked font in %lld ms", end - start);

	return true;
}

shiny_font_t *shiny_font_create(SDL_Renderer *renderer, const Uint8 *data, const float font_size)
{
	shiny_font_t *font = SDL_calloc(1, sizeof(shiny_font_t));
	if (font == nullptr)
	{
		return nullptr;
	}

	font->renderer = renderer;
	font->data = data;
	font->size = font_size;
	font->color = shiny_sdl_theme_color(SHINY_COLOR_FOREGROUND);

	if (!shiny_font_parse(font) || !shiny_font_bake(font))
	{
		SDL_free(font);
		return nullptr;
	}

	return font;
}

bool shiny_font_draw_text(const shiny_font_t *font, const float x,
	const float y, const char *text, const Sint32 length)
{
	float curr_x = x;

	for (auto i = 0; i < length; i++)
	{
		const auto codepoint = (int) text[i];
		int advance_width;
		int left_side_bearing;
		stbtt_GetCodepointHMetrics(&font->info, codepoint, &advance_width, &left_side_bearing);

		int bm_x0;
		int bm_y0;
		int bm_x1;
		int bm_y1;
		stbtt_GetCodepointBitmapBox(&font->info, codepoint, font->scale, font->scale,
			&bm_x0, &bm_y0, &bm_x1, &bm_y1);

		const float curr_y = y + ((float) font->ascent * font->scale) + (float) bm_y0;

		// TODO: This is probably the definition of terrible
		SDL_Surface *surface = SDL_CreateSurface(advance_width, font->size, SDL_PIXELFORMAT_INDEX8);
		if (surface == nullptr)
		{
			return false;
		}

		if (!shiny_build_palette(surface, font->color))
		{
			return false;
		}

		stbtt_MakeCodepointBitmap(&font->info, surface->pixels, surface->w, surface->h,
			surface->w, font->scale, font->scale, codepoint);

		// TODO: This is also REALLY bad
		SDL_Texture *texture = SDL_CreateTextureFromSurface(font->renderer, surface);
		if (texture == nullptr)
		{
			SDL_DestroySurface(surface);
			return false;
		}

		if (!SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND))
		{
			return false;
		}

		const SDL_FRect dst = {
			.x = curr_x,
			.y = curr_y,
			.w = (float) texture->w,
			.h = (float) texture->h,
		};
		SDL_RenderTexture(font->renderer, texture, nullptr, &dst);

		SDL_DestroyTexture(texture);
		SDL_DestroySurface(surface);

		curr_x += (float) advance_width * font->scale;

		if (i < length - 1)
		{
			const auto next_codepoint = (int) text[i + 1];
			const int kern = stbtt_GetCodepointKernAdvance(&font->info, codepoint, next_codepoint);
			curr_x += (float) kern * font->scale;
		}
	}

	return true;
}

void shiny_font_destroy(shiny_font_t *font)
{
	SDL_DestroyTexture(font->glyphs);
	SDL_free(font);
}
