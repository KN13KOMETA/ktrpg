#include "luab_helper.h"

#include <stdlib.h>

#include "../functions.h"

uint16_t luab_h_register_integer_component(luab_state* lb) {
  ecs_comp_t comp =
      ecs_define_component(lb->ecs, sizeof(lua_Integer), NULL, NULL);

  lb->comps[lb->comp_count] = comp;
  lb->comp_types[lb->comp_count] = COMP_INTEGER;

  return lb->comp_count++;
}
uint16_t luab_h_register_number_component(luab_state* lb) {
  ecs_comp_t comp =
      ecs_define_component(lb->ecs, sizeof(lua_Number), NULL, NULL);

  lb->comps[lb->comp_count] = comp;
  lb->comp_types[lb->comp_count] = COMP_NUMBER;

  return lb->comp_count++;
}
uint16_t luab_h_register_string_component(luab_state* lb) {
  ecs_comp_t comp = ecs_define_component(lb->ecs, sizeof(char*), NULL, NULL);

  lb->comps[lb->comp_count] = comp;
  lb->comp_types[lb->comp_count] = COMP_STRING;

  return lb->comp_count++;
}

uint16_t luab_h_register_system(luab_state* lb, int lua_func_ref) {
  // WARN: This memory doesnt free anywhere
  luab_system* lbs = malloc(sizeof(luab_system));
  ecs_system_t s =
      ecs_define_system(lb->ecs, 0, luab_system_wrapper, NULL, NULL, lbs);

  lbs->lb = lb;
  lbs->index = lb->system_count;

  lb->systems[lb->system_count] = s;
  lb->system_lua_refs[lb->system_count] = lua_func_ref;

  DEBUG_LOG("Registered lua function by reference %d", lua_func_ref);

  return lb->system_count++;
}

uint32_t luab_h_ecs_entity_create(luab_state* lb) {
  ecs_entity_t entity = ecs_create(lb->ecs);

  return entity.id;
}
