#include "shiny/font.h"
#include "shiny/themekey.h"
#include "shiny/internal/color.h"
#include "shiny/internal/logcategory.h"

#include <SDL3/SDL_assert.h>
#include <SDL3/SDL_blendmode.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>

// >.<
#define STBTT_ifloor(x) ((int) SDL_floor(x))
#define STBTT_iceil(x) ((int) SDL_ceil(x))
#define STBTT_sqrt(x) SDL_sqrt(x)
#define STBTT_pow(x,y) SDL_pow(x,y)
#define STBTT_fmod(x,y) SDL_fmod(x,y)
#define STBTT_cos(x) SDL_cos(x)
#define STBTT_acos(x) SDL_acos(x)
#define STBTT_fabs(x) SDL_fabs(x)
#define STBTT_malloc(x,u) ((void)(u),SDL_malloc(x))
#define STBTT_free(x,u) ((void)(u),SDL_free(x))
#define STBTT_assert(x) SDL_assert(x)
#define STBTT_strlen(x) SDL_strlen(x)
#define STBTT_memcpy SDL_memcpy
#define STBTT_memset SDL_memset

#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_rect_pack.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

// ASCII printable characters
static constexpr auto ascii_begin = 32;
static constexpr auto ascii_size = 95;

static constexpr auto font_size = 32;
static constexpr auto glyph_padding = 4;

typedef struct shiny_glyph_info_t
{
	int value; // TODO: Remove?
	int offset_x;
	int offset_y;
	int advance_x;
	SDL_Surface *image; // TODO: Move to temporary variable
} shiny_glyph_info_t;

typedef struct shiny_font_t
{
	const Uint8 *data;
	SDL_Renderer *renderer;
	SDL_Color color;

	SDL_Texture *texture;
	SDL_Rect recs[ascii_size];
	shiny_glyph_info_t glyphs[ascii_size];
} shiny_font_t;

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

	return SDL_SetPaletteColors(palette, colors, 0, color_count);
}

bool shiny_font_bake(shiny_font_t *font)
{
	const Uint64 start = SDL_GetTicks();

	shiny_glyph_info_t *glyphs = font->glyphs;

	stbtt_fontinfo font_info;
	if (!stbtt_InitFont(&font_info, font->data, 0))
	{
		return false;
	}

	const float scale = stbtt_ScaleForPixelHeight(&font_info, (float) font_size);

	int ascent;
	int descent;
	int line_gap;
	stbtt_GetFontVMetrics(&font_info, &ascent, &descent, &line_gap);

	for (auto i = 0; i < ascii_size; i++)
	{
		const int codepoint = ascii_begin + i;
		shiny_glyph_info_t *glyph = font->glyphs + i;

		const int index = stbtt_FindGlyphIndex(&font_info, codepoint);
		if (index <= 0)
		{
			return SDL_SetError("Invalid codepoint: %d", codepoint);
		}

		glyphs[i].value = codepoint;

		stbtt_GetGlyphBitmapBox(&font_info, index, scale, scale,
			&glyph->offset_x, &glyph->offset_y, nullptr, nullptr
		);

		stbtt_GetCodepointHMetrics(&font_info, codepoint, &glyph->advance_x, nullptr);
		glyph->advance_x = (int) ((float) glyph->advance_x * scale);
		glyph->offset_y += (int) ((float) ascent * scale);

		int glyph_width;
		int glyph_height;

		if (codepoint == ' ')
		{
			glyph_width = glyph->advance_x;
			glyph_height = font_size;
		}
		else
		{
			int box_x0;
			int box_y0;
			int box_x1;
			int box_y1;
			stbtt_GetCodepointBitmapBox(&font_info, codepoint, scale, scale,
				&box_x0, &box_y0, &box_x1, &box_y1
			);

			glyph_width = box_x1 - box_x0;
			glyph_height = box_y1 - box_y0;
		}

		glyph->image = SDL_CreateSurface(glyph_width, glyph_height, SDL_PIXELFORMAT_INDEX8);
		if (glyphs[i].image == nullptr || !shiny_build_palette(glyph->image, font->color))
		{
			return false;
		}

		stbtt_MakeCodepointBitmap(&font_info, glyph->image->pixels, glyph_width, glyph_height,
			glyph_width, scale, scale, codepoint
		);
	}

	auto total_width = 0;
	auto max_glyph_width = 0;

	for (auto i = 0; i < ascii_size; i++)
	{
		max_glyph_width = SDL_max(max_glyph_width, glyphs[i].image->w);
		total_width += glyphs[i].image->w + (2 * glyph_padding);
	}

	constexpr int padded_font_size = font_size + (2 * glyph_padding);
	const float total_area = (float) total_width * (float) padded_font_size * 1.2F;
	const float image_min_size = SDL_sqrtf(total_area);
	const auto image_size = (int) SDL_powf(2, SDL_ceilf(SDL_logf(image_min_size) / SDL_logf(2)));

	const int atlas_width = image_size;
	const int atlas_height = (int) total_area < ((image_size * image_size) / 2) ? image_size / 2 : image_size;
	SDL_Surface *atlas = SDL_CreateSurface(atlas_width, atlas_height, SDL_PIXELFORMAT_INDEX8);

	if (atlas == nullptr || !shiny_build_palette(atlas, font->color))
	{
		return false;
	}

	stbrp_context context;
	stbrp_node nodes[ascii_size];

	stbrp_init_target(&context, atlas->w, atlas->h, nodes, ascii_size);
	stbrp_rect rects[ascii_size];

	for (auto i = 0; i < ascii_size; i++)
	{
		rects[i].id = i;
		rects[i].w = glyphs[i].image->w + (2 * glyph_padding);
		rects[i].h = glyphs[i].image->h + (2 * glyph_padding);
	}

	if (!stbrp_pack_rects(&context, rects, ascii_size))
	{
		return SDL_SetError("Font packing failed");
	}

	for (auto i = 0; i < ascii_size; i++)
	{
		if (!rects[i].was_packed)
		{
			SDL_LogWarn(SHINY_LOG_CATEGORY_FONT, "Invalid character for packaging: '%c'", glyphs[i].value);
			continue;
		}

		const shiny_glyph_info_t *glyph = glyphs + i;;

		font->recs[i].x = rects[i].x + glyph_padding;
		font->recs[i].y = rects[i].y + glyph_padding;
		font->recs[i].w = glyph->image->w;
		font->recs[i].h = glyph->image->h;

		// TODO: This corrupts some characters for some reason
		// if (!SDL_BlitSurface(glyph->image, nullptr, atlas, &font->recs[i]))
		// {
		// 	return false;
		// }

		// TODO: Remove
		for (auto y = 0; y < glyphs[i].image->h; y++)
		{
			for (auto x = 0; x < glyphs[i].image->w; x++)
			{
				((unsigned char *) atlas->pixels)[((rects[i].y + glyph_padding + y) * atlas->w) + (rects[i].x +
					glyph_padding + x)] = ((unsigned char *) glyphs[i].image->pixels)[(y * glyphs[i].image->w) + x];
			}
		}

		SDL_DestroySurface(glyph->image);
	}

	font->texture = SDL_CreateTextureFromSurface(font->renderer, atlas);
	SDL_DestroySurface(atlas);
	if (font->texture == nullptr)
	{
		return false;
	}

	if (!SDL_SetTextureBlendMode(font->texture, SDL_BLENDMODE_BLEND)
		|| !SDL_SetTextureScaleMode(font->texture, SDL_SCALEMODE_LINEAR))
	{
		return false;
	}

	const Uint64 end = SDL_GetTicks();
	SDL_LogDebug(SHINY_LOG_CATEGORY_FONT, "Baked font in %lld ms", end - start);

	return true;
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
	font->color = shiny_sdl_theme_color(SHINY_COLOR_FOREGROUND);

	if (!shiny_font_bake(font))
	{
		SDL_free(font);
		return nullptr;
	}

	return font;
}

bool shiny_font_draw_text(const shiny_font_t *font, const float x,
	const float y, const float text_size, const char *text, const Sint32 length)
{
	auto offset_x = 0.F;
	auto offset_y = 0.F;
	const float scale = text_size / (float) font_size;

	for (auto i = 0; i < length; i++)
	{
		const auto codepoint = (int) text[i];
		const int index = codepoint - ascii_begin;

		const SDL_Rect *rect = font->recs + index;
		const shiny_glyph_info_t *glyph = font->glyphs + index;

		if (codepoint == '\n')
		{
			constexpr auto line_spacing = 2.F;

			offset_x = 0.F;
			offset_y += font_size + line_spacing;
			continue;
		}

		SDL_FRect src = {
			.x = (float) rect->x - (float) glyph_padding,
			.y = (float) rect->y - (float) glyph_padding,
			.w = ((float) rect->w + ((float) glyph_padding * 2.F)),
			.h = ((float) rect->h + ((float) glyph_padding * 2.F)),
		};
		SDL_FRect dst = {
			.x = x + offset_x + ((float) glyph->offset_x * scale),
			.y = y + offset_y + ((float) glyph->offset_y * scale),
			.w = ((float) rect->w + ((float) glyph_padding * 2.F)) * scale,
			.h = ((float) rect->h + ((float) glyph_padding * 2.F)) * scale,
		};

		SDL_RenderTexture(font->renderer, font->texture, &src, &dst);

		const int width = glyph->advance_x == 0 ? rect->w : glyph->advance_x;
		offset_x += (float) width * scale;
	}

	return true;
}

void shiny_font_destroy(shiny_font_t *font)
{
	SDL_DestroyTexture(font->texture);
	SDL_free(font);
}
