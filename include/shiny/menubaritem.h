#pragma once

#include "shiny/internal/context.h"

bool shiny_menubar_item_begin(Clay_Context *context, const char *element_id);

void shiny_menubar_item_end();

void shiny_menubar_content_begin(Clay_Context *context);

void shiny_menubar_content_end();
