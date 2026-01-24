#include "system.h"

#include <lauxlib.h>
#include <stdlib.h>
#include <string.h>

static ecs_t* ecs;
static lg_system* systems;
static ecs_id_t systems_count = 0;
static ecs_id_t systems_size = 0;

static int method_get_name(lua_State* L) {
  lg_system* s = luaL_checkudata(L, 1, "ClassSystemMT");

  lua_pushstring(L, s->name);

  return 1;
}

static struct luaL_Reg system_methods[] = {{"get_name", method_get_name},
                                           {NULL, NULL}};

static int system_gc(lua_State* L) {
  lg_system* s = luaL_checkudata(L, 1, "ClassSystemMT");

  free(s->name);

  return 0;
}

static void system_init_metatable(lua_State* L) {
  luaL_newmetatable(L, "ClassSystemMT");
  lua_pushvalue(L, -1);

  lua_setfield(L, -2, "__index");
  luaL_setfuncs(L, system_methods, 0);

  lua_pushcfunction(L, system_gc);
  lua_setfield(L, -2, "__gc");

  lua_pop(L, 1);
}

static int system_new(lua_State* L) {
  const char* cname = luaL_checkstring(L, 2);
  lg_system* s = lua_newuserdatauv(L, sizeof(*s), 0);

  s->name = malloc(strlen(cname) + 1);
  strcpy(s->name, cname);

  // TODO: Add realloc
  systems[systems_count++] = *s;

  luaL_getmetatable(L, "ClassSystemMT");
  lua_setmetatable(L, -2);

  return 1;
}

static int system_register_content(lua_State* L) {
  lua_newtable(L);
  lua_pushcfunction(L, system_new);
  lua_setfield(L, -2, "new");

  return 1;
}

void lg_system_create(lua_State* L) {
  lua_getfield(L, LUA_REGISTRYINDEX, "ecs");
  ecs = lua_touserdata(L, -1);
  lua_pop(L, 1);

  systems_size = UINT8_MAX;
  systems_count = 0;
  systems = malloc(sizeof(*systems) * systems_size);

  system_init_metatable(L);

  system_register_content(L);
}
void lg_system_destroy(void) {
  free(systems);
  systems_count = systems_size = 0;
}
