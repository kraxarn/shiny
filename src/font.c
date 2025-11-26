#include "shiny/font.h"
#include "shiny/themekey.h"
#include "shiny/internal/color.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include <SDL3/SDL_blendmode.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>

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
	return SDL_GetNumberProperty(props, name, default_value);
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

// TODO: Move around functions instead
bool shiny_font_bake(shiny_font_t *font);

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

	if (!SDL_SetSurfacePalette(surface, palette)) // TODO: Shouldn't be needed
	{
		return false;
	}

	return true;
}

bool shiny_font_bake(shiny_font_t *font)
{
	const auto atlas_size = (int) shiny_font_texture_size(font->renderer);
	SDL_Surface *surface = SDL_CreateSurface(atlas_size, atlas_size, SDL_PIXELFORMAT_INDEX8);
	if (surface == nullptr)
	{
		return false;
	}

	if (!shiny_build_palette(surface, font->color))
	{
		return SDL_SetError("Invalid surface palette");
	}

	stbtt_BakeFontBitmap(font->data, 0, font->size, surface->pixels,surface->w, surface->h,
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
		return false;
	}

	SDL_DestroySurface(surface);

	if (!SDL_SetTextureBlendMode(font->glyphs, SDL_BLENDMODE_BLEND))
	{
		return false;
	}

	return true;
}

bool shiny_font_draw_text(const shiny_font_t *font, const float x,
	const float y, const char *text, const Sint32 length)
{
	float curr_x = x;

	for (auto i = 0; i < length; i++)
	{
		const char chr = text[i];
		const stbtt_bakedchar *baked_char = font->baked_chars + chr - ascii_begin;

		const SDL_FRect src = {
			.x = (float) baked_char->x0,
			.y = (float) baked_char->y0,
			.w = (float) (baked_char->x1 - baked_char->x0),
			.h = (float) (baked_char->y1 - baked_char->y0),
		};
		const SDL_FRect dst = {
			.x = curr_x + baked_char->xoff,
			.y = y + baked_char->yoff,
			.w = src.w,
			.h = src.h,
		};

		if (!SDL_RenderTexture(font->renderer, font->glyphs, &src, &dst))
		{
			return false;
		}

		curr_x += baked_char->xadvance;
	}

	return true;
}

void shiny_font_destroy(shiny_font_t *font)
{
	SDL_DestroyTexture(font->glyphs);
	SDL_free(font);
}
