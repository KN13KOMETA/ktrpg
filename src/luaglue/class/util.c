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

static int print_tostring(lua_State* L, int i) {
  switch (lua_type(L, i)) {
    case LUA_TNUMBER:
    case LUA_TSTRING:
      printf("%s", lua_tostring(L, i));
      break;
    case LUA_TNIL:
      printf("nil");
      break;
    case LUA_TBOOLEAN:
      printf(lua_toboolean(L, i) == 0 ? "false" : "true");
      break;
    case LUA_TTABLE: {
      printf("TODO: table, %llu\n", lua_rawlen(L, i));

      lua_pushnil(L);
      while (lua_next(L, i) != 0) {
        if (lua_type(L, -2) == LUA_TSTRING) {
          printf("[\"%s\"] = ", lua_tostring(L, -2));
        } else if (lua_type(L, -2) == LUA_TNUMBER) {
          printf("[%g] = ", lua_tonumber(L, -2));
        } else {
          printf("[%s] = ", luaL_typename(L, -2));
        }

        print_tostring(L, lua_absindex(L, -1));
        printf(",\n");

        lua_pop(L, 1);
      }

      break;
    }
    case LUA_TFUNCTION:
      printf("function: %p", lua_topointer(L, i));
      break;
    case LUA_TUSERDATA:
      printf("userdata: %p", lua_topointer(L, i));
      break;
    case LUA_TTHREAD:
      printf("thread: %p", lua_topointer(L, i));
      break;
    case LUA_TLIGHTUSERDATA:
      printf("lightuserdata: %p", lua_topointer(L, i));
      break;
  }

  return 0;
}

// TODO: table and userdata printing
static int util_writenl(lua_State* L) {
  if (lua_gettop(L) != 0) print_tostring(L, 1);

  putchar('\n');

  return 0;
}

static int util_write(lua_State* L) {
  if (lua_gettop(L) == 0) return 0;

  print_tostring(L, 1);

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
// I am not sure if I can do that with current ecs implementation
// It doesn't have builtin serialization/deserialization
// So I planned to just go through every entity and check what components it
// have Yeah that would be a bit slow but working But here's another problem,
// since I can't change entity id's
// When loading I can't set them id that I want,
// so this breaks id-based targeting system
