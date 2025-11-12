#pragma once
#ifndef GAME_ENEMY_COMPONENTS_H
#define GAME_ENEMY_COMPONENTS_H

#include <pico_ecs.h>

#include "../constants.h"

typedef name_t gen_name_comp;
typedef hpint_t gen_min_health_comp;
typedef hpint_t gen_max_health_comp;

extern ecs_comp_t GEN_NAME_COMP;
extern ecs_comp_t GEN_MIN_HEALTH_COMP;
extern ecs_comp_t GEN_MAX_HEALTH_COMP;

void register_components(ecs_t* ecs);

#endif  // !GAME_ENEMY_COMPONENTS_H
