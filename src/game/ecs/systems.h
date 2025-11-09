#pragma once
#ifndef SYSTEMS_H
#define SYSTEMS_H

#include <pico_ecs.h>

ecs_ret_t debug_system(ecs_t* ecs, ecs_entity_t* entities, size_t entity_count,
                       void* udata);

extern ecs_system_t DEBUG_SYSTEM;

void register_systems(ecs_t* ecs);

#endif  // !SYSTEMS_H
