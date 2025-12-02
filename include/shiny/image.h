#pragma once

#include "shiny/size.h"
#include "shiny/internal/context.h"

#include <SDL3/SDL_render.h>

void shiny_image(Clay_Context *context, SDL_Texture *texture, const shiny_size_t *size);
