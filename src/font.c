#include "shiny/font.h"
#include "shiny/color.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

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
	const Uint8 *font_data;
	Sint64 texture_size;
	SDL_Renderer *renderer;
	SDL_Surface *glyphs;
	SDL_Palette *palette;
	SDL_Texture *texture;
	stbtt_bakedchar chars[ascii_size];
	float font_size;
	shiny_color_t color;
} shiny_font_t;

Sint64 shiny_font_default_texture_size(SDL_Renderer *renderer)
{
	const SDL_PropertiesID renderer_props = SDL_GetRendererProperties(renderer);
	const Sint64 max_texture_size = SDL_GetNumberProperty(renderer_props, SDL_PROP_RENDERER_MAX_TEXTURE_SIZE_NUMBER, 0);
	constexpr Sint64 default_texture_size = 1024;
	return max_texture_size > 0 && default_texture_size > max_texture_size
		? max_texture_size
		: default_texture_size;
}

shiny_font_t *shiny_font_create(SDL_Renderer *renderer, const Uint8 *data)
{
	shiny_font_t *font = SDL_calloc(1, sizeof(shiny_font_t));
	if (font == nullptr)
	{
		return nullptr;
	}

	font->renderer = renderer;
	font->font_data = data;
	font->texture_size = shiny_font_default_texture_size(renderer);
	return font;
}

bool shiny_font_bake(shiny_font_t *font)
{
	if (font->texture != nullptr)
	{
		SDL_DestroyTexture(font->texture);
	}

	if (font->glyphs != nullptr)
	{
		SDL_DestroySurface(font->glyphs);
	}

	if (font->font_size <= 0.F)
	{
		return SDL_SetError("Invalid font size: %f", font->font_size);
	}

	font->glyphs = SDL_CreateSurface(
		(int) font->texture_size, (int) font->texture_size,
		SDL_PIXELFORMAT_INDEX8
	);
	if (font->glyphs == nullptr)
	{
		return false;
	}

	// TODO
	SDL_Color colors[256];
	for (auto i = 0; i < 256; i++)
	{
		colors[i].r = i;
		colors[i].g = i;
		colors[i].b = i;
	}
	font->palette = SDL_CreateSurfacePalette(font->glyphs);
	SDL_SetPaletteColors(font->palette, colors, 0, 256);

	const int font_offset = stbtt_GetFontOffsetForIndex(font->font_data, 0);
	stbtt_BakeFontBitmap(
		font->font_data, font_offset, font->font_size,
		font->glyphs->pixels, font->glyphs->w, font->glyphs->h,
		ascii_begin, ascii_size,
		font->chars
	);

	font->texture = SDL_CreateTextureFromSurface(font->renderer, font->glyphs);
	return font->texture != nullptr;
}

bool shiny_font_draw_text(const shiny_font_t *font, const char *text)
{
	auto result = true;

	auto x = 0.F;
	auto y = 0.F;

	for (const char *chr = text; chr != nullptr; chr++)
	{
		stbtt_aligned_quad quad;
		stbtt_GetBakedQuad(font->chars, font->glyphs->w, font->glyphs->h, (*chr) - ascii_begin, &x, &y, &quad, 1);

		const float width = quad.x1 - quad.x0;
		const float height = quad.y1 - quad.y0;

		SDL_FRect src = {
			.x = quad.s0 * (float) font->glyphs->w,
			.y = quad.t0 * (float) font->glyphs->h,
			.w = width,
			.h = height,
		};
		SDL_FRect dst = {
			.x = quad.x0,
			.y = (float) font->font_size + quad.y0,
			.w = width,
			.h = height,
		};

		result |= SDL_RenderTexture(font->renderer, font->texture, &src, &dst);
	}

	return result;
}

void shiny_font_close(shiny_font_t *font)
{
	SDL_DestroyPalette(font->palette);
	SDL_DestroySurface(font->glyphs);
	SDL_DestroyTexture(font->texture);
	SDL_free(font);
}

void shiny_font_set_size(shiny_font_t *font, const float size)
{
	font->font_size = size;
}

void shiny_font_set_color(shiny_font_t *font, const shiny_color_t color)
{
	font->color = color;
}
