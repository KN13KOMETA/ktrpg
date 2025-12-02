#pragma once
#ifndef TGG_TGG_H
#define TGG_TGG_H

#include <pico_ecs.h>
#include <stdbool.h>

#define TGG_ENTITY_COUNT 64

// The Great Generator
ecs_t* tgg_init(void);

int tgg_load_content(ecs_t* tgg_ecs, char* script_path);

void tgg_free(ecs_t* tgg_ecs);

#endif  // !TGG_TGG_H
