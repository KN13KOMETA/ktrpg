#pragma once
#ifndef LUAB_LUAB_HELPER_h
#define LUAB_LUAB_HELPER_h

#include <pico_ecs.h>
#include <stdint.h>

#include "luab_core.h"

uint16_t luab_h_register_integer_component(luab_state* lb);
uint16_t luab_h_register_number_component(luab_state* lb);
uint16_t luab_h_register_string_component(luab_state* lb);

uint16_t luab_h_register_system(luab_state* lb, int lua_func_ref);

uint32_t luab_h_ecs_entity_create(luab_state* lb);

uintptr_t luab_h_ecs_comp_add(luab_state* lb, uint32_t entity_id,
                              uint16_t comp_id);

#endif  // !LUAB_LUAB_HELPER_h
