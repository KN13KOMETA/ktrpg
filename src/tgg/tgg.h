#pragma once
#ifndef TGG_TGG_H
#define TGG_TGG_H

#include <pico_ecs.h>

// The Great Generator
ecs_t* tgg_init(void);
void tgg_free(ecs_t* tgg_ecs);

#endif  // !TGG_TGG_H
