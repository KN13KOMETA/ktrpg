#pragma once
#ifndef TGG_SYSTEMS_H
#define TGG_SYSTEMS_H

#include <pico_ecs.h>

ecs_ret_t tgg_debug_system(ecs_t* ecs, ecs_entity_t* entities,
                           size_t entity_count, void* udata);

static ecs_system_t TGG_DEBUG_SYSTEM;

void tgg_register_systems(ecs_t* ecs);

#endif  // !TGG_SYSTEMS_H
