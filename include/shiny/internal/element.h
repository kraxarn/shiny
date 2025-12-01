#pragma once

#include "clay.h"

// TODO: Remove context parameter once we know context is only in shiny

void shiny_element_open(Clay_Context *context, const char *element_id);

void shiny_element_configure(const Clay_ElementDeclaration *declaration);

void shiny_element_close();
