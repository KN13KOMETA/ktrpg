#include "core.h"

#include <lauxlib.h>

#include "../functions.h"
#include "../luah/alt.h"
#include "class/component.h"
#include "lua.h"

static int openclib(lua_State* L) {
  lua_newtable(L);

  lg_component_create(L);
  lua_setfield(L, -2, "Component");

  return 1;
}

void lg_create(lua_State* L, ecs_t* ecs) {
  lua_pushlightuserdata(L, ecs);
  lua_setfield(L, LUA_REGISTRYINDEX, "ecs");

  luaL_requiref(L, "ktrpg", openclib, 0);
  DEBUG_LOG("AMONGUS");

  if (luaL_dostring(L, alt_lua) != LUA_OK) {
    printf("EERRRRR: %s\n", lua_tostring(L, -1));
    lua_pop(L, -1);
  }
  DEBUG_LOG("AMONGUS");
}
void lg_destroy(void) { lg_component_destroy(); }
