#pragma once
#include "lua.h"
#ifndef LUAB_LUAB_CORE_H
#define LUAB_LUAB_CORE_H

#include <pico_ecs.h>

typedef struct {
  lua_State* L;
  ecs_t* ecs;
  ecs_comp_t* comps;
  ecs_id_t comp_count;
  ecs_system_t* systems;
  ecs_system_t wrapper_system;
  int* system_lua_refs;
  ecs_id_t system_count;
} luab_state;

luab_state luab_init(void);
void luab_free(luab_state* luab_state);

// Since we can't directly turn lua function into c function
// Instead we are registering wrapper_system everytime
// Which will call corresponding lua function
ecs_ret_t luab_system_wrapper(ecs_t* ecs, ecs_entity_t* entities,
                              size_t entity_count, void* udata);

#endif  // !LUAB_LUAB_CORE_H
