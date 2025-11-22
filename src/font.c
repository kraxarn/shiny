#include "shiny/font.h"
#include "shiny/internal/logcategory.h"

#include "stb_rect_pack.h"

#include <freetype/freetype.h>
#include <freetype/fterrors.h>
#include <freetype/ftmodapi.h>
#include <freetype/ftsystem.h>
#include <freetype/fttypes.h>

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>

static FT_Library ft_library = nullptr;

static SDL_Renderer *sdl_renderer = nullptr;
static Sint64 sdl_texture_size = 0;

typedef struct shiny_font_t
{
	FT_Face face;
} shiny_font_t;

void *ft_alloc([[maybe_unused]] FT_Memory memory, const long size)
{
	return SDL_malloc(size);
}

void ft_free([[maybe_unused]] FT_Memory memory, void *block)
{
	SDL_free(block);
}

void *ft_realloc([[maybe_unused]] FT_Memory memory, [[maybe_unused]] long cur_size,
	const long new_size, void *block)
{
	return SDL_realloc(block, new_size);
}

static struct FT_MemoryRec_ ft_memory = {
	.user = nullptr,
	.alloc = ft_alloc,
	.free = ft_free,
	.realloc = ft_realloc,
};

bool shiny_font_init(SDL_Renderer *renderer)
{
	if (ft_library != nullptr)
	{
		SDL_LogWarn(SHINY_LOG_CATEGORY_FONT, "Font library already initialised");
		return true;
	}

	sdl_renderer = renderer;

	const FT_Error error = FT_New_Library(&ft_memory, &ft_library);
	if (error != FT_Err_Ok)
	{
		return SDL_SetError("%s", FT_Error_String(error));
	}

	FT_Add_Default_Modules(ft_library);

	const SDL_PropertiesID renderer_props = SDL_GetRendererProperties(renderer);
	const Sint64 max_texture_size = SDL_GetNumberProperty(renderer_props, SDL_PROP_RENDERER_MAX_TEXTURE_SIZE_NUMBER, 0);
	constexpr Sint64 default_texture_size = 1024;
	sdl_texture_size = max_texture_size > 0 && default_texture_size > max_texture_size
		? max_texture_size
		: default_texture_size;

	return true;
}

bool shiny_font_destroy()
{
	const FT_Error error = FT_Done_FreeType(ft_library);
	if (error != FT_Err_Ok)
	{
		return SDL_SetError("%s", FT_Error_String(error));
	}

	ft_library = nullptr;
	return true;
}

shiny_font_t *shiny_font_open(const Uint8 *data, const long size)
{
	FT_Face face;
	FT_Error error = FT_New_Memory_Face(ft_library, data, size, 0, &face);
	if (error != FT_Err_Ok)
	{
		SDL_SetError("%s", FT_Error_String(error));
		return nullptr;
	}

	error = FT_Select_Charmap(face, FT_ENCODING_UNICODE);
	if (error != FT_Err_Ok)
	{
		SDL_SetError("%s", FT_Error_String(error));
		return nullptr;
	}

	shiny_font_t *font = SDL_malloc(sizeof(shiny_font_t));
	font->face = face;
	return font;
}

void shiny_font_close(shiny_font_t *font)
{
	FT_Done_Face(font->face);
	SDL_free(font);
}

bool shiny_font_set_size(const shiny_font_t *font, const float size)
{
	const FT_Error error = FT_Set_Char_Size(font->face, 0, (long) SDL_roundf(size * 64), 0, 0);
	if (error != FT_Err_Ok)
	{
		return SDL_SetError("%s", FT_Error_String(error));
	}
	return true;
}
