#pragma once
#ifndef GAME_ECS_SYSTEMS_H
#define GAME_ECS_SYSTEMS_H

#include <pico_ecs.h>

ecs_ret_t debug_system(ecs_t* ecs, ecs_entity_t* entities, size_t entity_count,
                       void* udata);

extern ecs_system_t DEBUG_SYSTEM;

void register_systems(ecs_t* ecs);

#endif  // !GAME_ECS_SYSTEMS_H
