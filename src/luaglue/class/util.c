#include "util.h"

#include <limits.h>
#include <stdio.h>

#include "../../functions.h"
#include "../core.h"
#include "lauxlib.h"
#include "lua.h"

static ptr2ecs* ecs;

static int util_read(lua_State* L) {
  lua_Integer len = luaL_checkinteger(L, 1);
  char inp[len + 1];

  getchars_clear(inp, (size_t)len + 1);
  lua_pushstring(L, inp);

  return 1;
}

static int util_readchar(lua_State* L) {
  char c;

  getchar_clear(&c);
  lua_pushlstring(L, &c, 1);

  return 1;
}

static int util_ask_yn(lua_State* L) {
  const char* str = luaL_checkstring(L, 1);

  lua_pushboolean(L, !ask_yn(str));

  return 1;
}

static int util_writenl(lua_State* L) {
  const char* str = luaL_checkstring(L, 1);

  printf("%s\n", str);

  return 0;
}

static int util_write(lua_State* L) {
  const char* str = luaL_checkstring(L, 1);

  printf("%s", str);

  return 0;
}

static int util_sleep(lua_State* L) {
  lua_Integer seconds = luaL_checkinteger(L, 1);

  if (seconds > UINT_MAX) {
    lua_pushboolean(L, 0);
    return 1;
  }

  sleep_s((unsigned)seconds);

  lua_pushboolean(L, 1);
  return 1;
}

static luaL_Reg util_class_methods[] = {{"read", util_read},
                                        {"readchar", util_readchar},
                                        {"ask_yn", util_ask_yn},
                                        {"writenl", util_writenl},
                                        {"write", util_write},
                                        {"sleep", util_sleep},
                                        {NULL, NULL}};

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

// TODO: save/load system
// TODO: sleep, write, read functions (so i can make it sandbox safe)
