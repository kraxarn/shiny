#pragma once

#include "shiny/themekey.h"

#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_stdinc.h>

SDL_Color shiny_theme_color(shiny_theme_color_t key);
void shiny_theme_set_color(shiny_theme_color_t key, SDL_Color value);

Uint16 shiny_theme_gap(shiny_theme_gap_t key);
void shiny_theme_set_gap(shiny_theme_gap_t key, Uint16 value);

float shiny_theme_corner_radius(shiny_theme_corner_radius_t key);
void shiny_theme_set_corner_radius(shiny_theme_corner_radius_t key, float value);

Uint16 shiny_theme_padding(shiny_theme_padding_t key);
void shiny_theme_set_padding(shiny_theme_padding_t key, Uint16 value);

float shiny_theme_size(shiny_theme_size_t key);
void shiny_theme_set_size(shiny_theme_size_t key, float value);

void shiny_default_theme();
