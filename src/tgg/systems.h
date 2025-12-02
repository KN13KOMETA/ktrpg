#pragma once
#ifndef TGG_SYSTEMS_H
#define TGG_SYSTEMS_H

#include <pico_ecs.h>

typedef enum { TGG_DEBUG_FLAG = 01, TGG_VALIDATION_FLAG = 02 } tgg_flags;

ecs_ret_t tgg_debug_system(ecs_t* ecs, ecs_entity_t* entities,
                           size_t entity_count, void* udata);
ecs_ret_t tgg_validation_system(ecs_t* ecs, ecs_entity_t* entities,
                                size_t entity_count, void* udata);

static ecs_system_t TGG_DEBUG_SYSTEM;
static ecs_system_t TGG_VALIDATION_SYSTEM;

void tgg_register_systems(ecs_t* ecs);

#endif  // !TGG_SYSTEMS_H
