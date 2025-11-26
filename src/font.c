#include "shiny/font.h"
#include "shiny/color.h"
#include "shiny/internal/logcategory.h"

#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_rect_pack.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>

// ASCII printable characters
static constexpr auto ascii_begin = 32;
static constexpr auto ascii_size = 96;

static constexpr auto atlas_width = 1024;
static constexpr auto atlas_height = 1024;

typedef struct shiny_font_t
{
	const Uint8 *data;
	SDL_Renderer *renderer;

	SDL_Surface *glyphs;
	SDL_Texture *glyphs_texture;

	stbtt_packedchar packed_chars[ascii_size];
	stbtt_aligned_quad aligned_quads[ascii_size];

	float size;

	// SDL_Palette *palette;
	// SDL_Texture *texture;
	// stbtt_bakedchar chars[ascii_size];
	// float font_size;
	// shiny_color_t color;
} shiny_font_t;

Sint64 shiny_font_default_texture_size(SDL_Renderer *renderer)
{
	const SDL_PropertiesID renderer_props = SDL_GetRendererProperties(renderer);
	const Sint64 max_texture_size = SDL_GetNumberProperty(renderer_props,
		SDL_PROP_RENDERER_MAX_TEXTURE_SIZE_NUMBER, 0);
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
	font->data = data;

	return font;
}

static void shiny_font_pack(shiny_font_t *font)
{
	stbtt_pack_context ctx;

	constexpr auto stride_in_bytes = 0;
	constexpr auto padding = 1;

	stbtt_PackBegin(&ctx, font->glyphs->pixels, font->glyphs->w,
		font->glyphs->h, stride_in_bytes, padding, nullptr);
	{
		constexpr int first_codepoint = ascii_begin;
		constexpr int num_chars = ascii_size;

		stbtt_PackFontRange(&ctx, font->data, 0, font->size,
			first_codepoint, num_chars, font->packed_chars);
	}
	stbtt_PackEnd(&ctx);

	for (auto i = 0; i < ascii_size; i++)
	{
		auto temp_x = 0.F;
		auto temp_y = 0.F;

		stbtt_GetPackedQuad(font->packed_chars, font->glyphs->w, font->glyphs->h, i,
			&temp_x, &temp_y, font->aligned_quads + i, 0);
	}
}

bool shiny_font_bake(shiny_font_t *font)
{
	if (font->glyphs_texture != nullptr)
	{
		SDL_DestroyTexture(font->glyphs_texture);
	}

	if (font->glyphs != nullptr)
	{
		SDL_DestroySurface(font->glyphs);
	}

	// if (font->font_size <= 0.F)
	// {
	// 	return SDL_SetError("Invalid font size: %f", font->font_size);
	// }

	font->glyphs = SDL_CreateSurface(atlas_width, atlas_height,
		SDL_PIXELFORMAT_INDEX8);
	if (font->glyphs == nullptr)
	{
		return false;
	}

	shiny_font_pack(font);

	// TODO
	SDL_Color colors[256];
	for (auto i = 0; i < 256; i++)
	{
		colors[i].r = i;
		colors[i].g = i;
		colors[i].b = i;
	}
	SDL_Palette *palette = SDL_CreateSurfacePalette(font->glyphs);
	SDL_SetPaletteColors(palette, colors, 0, 256);
	SDL_SetSurfacePalette(font->glyphs, palette);

	// const int font_offset = stbtt_GetFontOffsetForIndex(font->font_data, 0);
	// stbtt_BakeFontBitmap(
	// 	font->font_data, font_offset, font->font_size,
	// 	font->glyphs->pixels, font->glyphs->w, font->glyphs->h,
	// 	ascii_begin, ascii_size,
	// 	font->chars
	// );
	//
	// font->texture = SDL_CreateTextureFromSurface(font->renderer, font->glyphs);
	// return font->texture != nullptr;

	return true;
}

bool shiny_font_draw_text(shiny_font_t *font, const float x,
	const float y, const char *text, const Sint32 length)
{
	auto result = true;

	auto x_offset = 0.F;
	auto y_offset = 0.F;

	for (auto i = 0; i < length; i++)
	{
		const int char_index = text[i] - ascii_begin;
		const stbtt_packedchar *packed_char = &font->packed_chars[char_index];
		const stbtt_aligned_quad *aligned_quad = &font->aligned_quads[char_index];

		const float width = aligned_quad->x1 - aligned_quad->x0;
		const float height = aligned_quad->y1 - aligned_quad->y0;

		SDL_FRect src = {
			.x = aligned_quad->s0 * (float) font->glyphs->w,
			.y = aligned_quad->t0 * (float) font->glyphs->h,
			.w = width,
			.h = height,
		};
		SDL_FRect dst = {
			.x = x + aligned_quad->x0,
			.y = font->size + y + aligned_quad->y0,
			.w = width,
			.h = height,
		};

		result |= SDL_RenderTexture(font->renderer, font->glyphs_texture, &src, &dst);
	}

	return result;
}

void shiny_font_destroy(shiny_font_t *font)
{
	SDL_DestroySurface(font->glyphs);
	SDL_DestroyTexture(font->glyphs_texture);
	SDL_free(font);
}

void shiny_font_set_size(shiny_font_t *font, const float size)
{
	font->size = size;
}

void shiny_font_set_color(shiny_font_t *font, const shiny_color_t color)
{
	// font->color = color;
}
