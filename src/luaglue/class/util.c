#include "util.h"

#include <limits.h>

#include "../../functions.h"
#include "../core.h"
#include "lauxlib.h"
#include "lua.h"

#ifdef _WIN32

#include <windows.h>

#else

#include <unistd.h>

#endif  // ifdef _WIN32

#ifdef _WIN32
#else
#endif  // ifdef _WIN32

static ptr2ecs* ecs;

static int util_sleep(lua_State* L) {
  lua_Integer seconds = luaL_checkinteger(L, 1);

  if (seconds > UINT_MAX) {
    lua_pushboolean(L, 0);
    return 1;
  }

  // TODO: Check on windows
#ifdef _WIN32
  Sleep(seconds * 1000);
#else
  sleep((unsigned)seconds);
#endif  // ifdef _WIN32

  lua_pushboolean(L, 1);
  return 1;
}

static luaL_Reg util_class_methods[] = {{"sleep", util_sleep}, {NULL, NULL}};

static int util_register_content(lua_State* L) {
  lua_newtable(L);

  luaL_setfuncs(L, util_class_methods, 0);

  return 1;
}

void lg_util_create(lua_State* L) {
  DEBUG_LOG("LG: UTIL CREATE");

  lua_getfield(L, LUA_REGISTRYINDEX, "ecs");
  ecs = lua_touserdata(L, -1);
  lua_pop(L, 1);

  util_register_content(L);
}
void lg_util_destroy(void) { DEBUG_LOG("LG: UTIL DESTROY"); }
