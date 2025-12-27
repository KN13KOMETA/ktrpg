#include "luab_helper.h"

#include <stdlib.h>

#include "../functions.h"

ecs_id_t luab_h_register_integer_component(luab_state* lb) {
  ecs_comp_t comp =
      ecs_define_component(lb->ecs, sizeof(lua_Integer), NULL, NULL);

  lb->comps[lb->comp_count] = comp;
  lb->comp_types[lb->comp_count] = COMP_INTEGER;

  return lb->comp_count++;
}
ecs_id_t luab_h_register_number_component(luab_state* lb);
ecs_id_t luab_h_register_string_component(luab_state* lb);

ecs_id_t luab_h_register_system(luab_state* lb, int lua_func_ref) {
  // WARN: This memory doesnt free anywhere
  luab_system* lbs = malloc(sizeof(luab_system));
  ecs_system_t s =
      ecs_define_system(lb->ecs, 0, luab_system_wrapper, NULL, NULL, lbs);

  lbs->lb = lb;
  lbs->index = lb->system_count;

  lb->systems[lb->system_count] = s;
  lb->system_lua_refs[lb->system_count] = lua_func_ref;
  lb->system_count++;

  DEBUG_LOG("Registered lua function by reference %d", lua_func_ref);

  return s.id;
}
