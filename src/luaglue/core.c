#include "core.h"

#include <lauxlib.h>

#include "../functions.h"
#include "../luah/alt.h"
#include "class/component.h"
#include "class/system.h"
#include "lua.h"

static int openclib(lua_State* L) {
  DEBUG_LOG("LG: OPENING C LIBRARY");

  lua_newtable(L);

  lg_component_create(L);
  lua_setfield(L, -2, "Component");
  lg_system_create(L);
  lua_setfield(L, -2, "System");

  return 1;
}

void lg_create(lua_State* L, ecs_t* ecs) {
  DEBUG_LOG("LG: CREATE");

  lua_pushlightuserdata(L, ecs);
  lua_setfield(L, LUA_REGISTRYINDEX, "ecs");

  luaL_requiref(L, "ktrpg", openclib, 0);
}
void lg_destroy(void) {
  DEBUG_LOG("LG: DESTROY");

  lg_component_destroy();
  lg_system_destroy();
}
