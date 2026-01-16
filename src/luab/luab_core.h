#pragma once
#include <time.h>
#ifndef LUAB_LUAB_CORE_H
#define LUAB_LUAB_CORE_H

#include <lua.h>
#include <pico_ecs.h>
#include <stdint.h>

#include "../project_options.h"

#define DEFAULT_COMP_COUNT 01000
#define DEFAULT_SYSTEM_COUNT 0100
#define MAX_COMP_COUNT UINT16_MAX
#define MAX_SYSTEM_COUNT UINT16_MAX

typedef enum { COMP_INTEGER, COMP_NUMBER, COMP_STRING } COMP_TYPE;

typedef struct {
  lua_State* L;
  ecs_t* ecs;

  uint16_t comps_size;
  ecs_comp_t* comps;
  COMP_TYPE* comp_types;
  uint16_t comp_count;

  uint16_t systems_size;
  ecs_system_t* systems;
  int* system_lua_refs;
  uint16_t system_count;
} luab_state;

typedef struct {
  luab_state* lb;
  ecs_id_t index;
} luab_system;

luab_state luab_init(project_options* poptions);
void luab_free(luab_state* luab_state);

// Since we can't directly turn lua function into c function
// Instead we are registering wrapper_system everytime
// Which will call corresponding lua function
ecs_ret_t luab_system_wrapper(ecs_t* ecs, ecs_entity_t* entities,
                              size_t entity_count, void* udata);

ecs_ret_t luab_debug_system(ecs_t* ecs, ecs_entity_t* entities,
                            size_t entity_count, void* udata);

void luab_register_text_module(lua_State* L, char* name, char* content);

#endif  // !LUAB_LUAB_CORE_H
