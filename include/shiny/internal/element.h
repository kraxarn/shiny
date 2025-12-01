#pragma once

#include "clay.h"

void shiny_element_open(Clay_Context *context, const char *element_id);

void shiny_element_configure(const Clay_ElementDeclaration *declaration);

void shiny_element_close();
