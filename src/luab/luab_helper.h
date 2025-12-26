#pragma once
#ifndef LUAB_LUAB_HELPER_h
#define LUAB_LUAB_HELPER_h

#include <pico_ecs.h>

#include "luab_core.h"

ecs_id_t luab_h_register_integer_component(luab_state* lb);
ecs_id_t luab_h_register_number_component(luab_state* lb);
ecs_id_t luab_h_register_string_component(luab_state* lb);

ecs_id_t luab_h_register_system(luab_state* lb, int lua_func_ref);

#endif  // !LUAB_LUAB_HELPER_h
