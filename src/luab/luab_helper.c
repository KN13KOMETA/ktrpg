#include "luab_helper.h"

#include "../functions.h"

ecs_id_t luab_h_register_system(luab_state* lb, int lua_func_ref) {
  ecs_system_t s =
      ecs_define_system(lb->ecs, 0, luab_system_wrapper, NULL, NULL, lb);

  lb->systems[lb->system_count] = s;
  lb->system_lua_refs[lb->system_count] = lua_func_ref;
  lb->system_count++;

  DEBUG_LOG("registered lua function by reference %d", lua_func_ref);

  return s.id;
}
