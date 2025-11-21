#include "shiny/font.h"
#include "shiny/internal/logcategory.h"

#include "stb_rect_pack.h"

#include <freetype/freetype.h>
#include <freetype/fterrors.h>
#include <freetype/ftmodapi.h>
#include <freetype/ftsystem.h>
#include <freetype/fttypes.h>

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>

static FT_Library ft_library = nullptr;
static FT_Long ft_face_size = 0;
static FT_Face ft_faces[SDL_MAX_UINT8];

static SDL_Renderer *sdl_renderer = nullptr;
static Sint64 sdl_texture_size = 0;

static SDL_PropertiesID props;

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
	sdl_texture_size = SDL_min(max_texture_size, 1024);

	if (sdl_texture_size <= 0)
	{
		return SDL_SetError("Invalid texture atlas size: %lld", sdl_texture_size);
	}

	props = SDL_CreateProperties();
	if (props == 0)
	{
		return false;
	}

	return true;
}

bool shiny_font_destroy()
{
	const FT_Error error = FT_Done_FreeType(ft_library);
	if (error != FT_Err_Ok)
	{
		return SDL_SetError("%s", FT_Error_String(error));
	}

	ft_face_size = 0;
	ft_library = nullptr;

	return true;
}

shiny_font_id shiny_font_load(const Uint8 *data, const long size)
{
	if (ft_face_size >= sizeof(ft_faces) / sizeof(FT_Face))
	{
		SDL_SetError("Too many fonts already loaded, why do you need so many?");
		return 0;
	}

	FT_Face face;
	FT_Error error = FT_New_Memory_Face(ft_library, data, size, 0, &face);
	if (error != FT_Err_Ok)
	{
		SDL_SetError("%s", FT_Error_String(error));
		return 0;
	}

	error = FT_Select_Charmap(face, FT_ENCODING_UNICODE);
	if (error != FT_Err_Ok)
	{
		SDL_SetError("%s", FT_Error_String(error));
		return 0;
	}

	ft_faces[ft_face_size++] = face;
	return (shiny_font_id) ft_face_size;
}
