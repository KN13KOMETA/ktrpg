#include "luab_helper.h"

#include "../functions.h"

ecs_id_t luab_h_register_system(luab_state* lb, int lua_func_ref) {
  lb->system_lua_refs[lb->system_count] = lua_func_ref;

  DEBUG_LOG("registered lua function by reference %d", lua_func_ref);

  return lb->system_count++;
}
