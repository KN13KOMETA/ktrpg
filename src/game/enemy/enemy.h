#pragma once
#ifndef GAME_ENEMY_ENEMY_H
#define GAME_ENEMY_ENEMY_H

#include <pico_ecs.h>

ecs_t* gen_enemy_generator(void);

ecs_entity_t gen_enemy(ecs_t* generator);

#endif  // !GAME_ENEMY_ENEMY_H
