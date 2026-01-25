#include "system.h"

#include <lauxlib.h>
#include <lua.h>
#include <stdlib.h>
#include <string.h>

#include "../../constants.h"
#include "../../functions.h"
#include "component.h"

static ecs_t* ecs;
static lg_system* systems;
static ecs_id_t systems_count = 0;
static ecs_id_t systems_size = 0;

static int method_get_entity_count(lua_State* L) {
  lg_system* s = ((ptr2ptr*)luaL_checkudata(L, 1, "ClassSystemMT"))->ptr;

  lua_pushstring(L, s->name);

  return 1;
}

static int method_run(lua_State* L) {
  lg_system* s = ((ptr2ptr*)luaL_checkudata(L, 1, "ClassSystemMT"))->ptr;

  lua_pushstring(L, s->name);

  return 1;
}
static int method_on_run(lua_State* L) {
  lg_system* s = ((ptr2ptr*)luaL_checkudata(L, 1, "ClassSystemMT"))->ptr;

  lua_pushstring(L, s->name);

  return 1;
}

static int method_disable(lua_State* L) {
  lg_system* s = ((ptr2ptr*)luaL_checkudata(L, 1, "ClassSystemMT"))->ptr;

  lua_pushstring(L, s->name);

  return 1;
}
static int method_enable(lua_State* L) {
  lg_system* s = ((ptr2ptr*)luaL_checkudata(L, 1, "ClassSystemMT"))->ptr;

  lua_pushstring(L, s->name);

  return 1;
}

static int method_get_mask(lua_State* L) {
  lg_system* s = ((ptr2ptr*)luaL_checkudata(L, 1, "ClassSystemMT"))->ptr;

  lua_pushstring(L, s->name);

  return 1;
}
static int method_set_mask(lua_State* L) {
  lg_system* s = ((ptr2ptr*)luaL_checkudata(L, 1, "ClassSystemMT"))->ptr;

  lua_pushstring(L, s->name);

  return 1;
}

static int method_excludes(lua_State* L) {
  lg_system* s = ((ptr2ptr*)luaL_checkudata(L, 1, "ClassSystemMT"))->ptr;

  lua_pushstring(L, s->name);

  return 1;
}
static int method_requires(lua_State* L) {
  lg_system* s = ((ptr2ptr*)luaL_checkudata(L, 1, "ClassSystemMT"))->ptr;
  int n = lua_gettop(L);

  for (int i = 2; i < n + 1; i++) {
    lg_component* c =
        ((ptr2ptr*)luaL_checkudata(L, i, "ClassComponentMT"))->ptr;

    ecs_require_component(ecs, (ecs_system_t){s->id}, (ecs_comp_t){c->id});

    DEBUG_LOG("LG: " SYST_FL " REQUIRES " COMP_FL, SYST_FL_ARGS(s),
              COMP_FL_ARGS(c));
  }

  lua_pushvalue(L, 1);
  return 1;
}

static int method_get_name(lua_State* L) {
  lg_system* s = ((ptr2ptr*)luaL_checkudata(L, 1, "ClassSystemMT"))->ptr;

  lua_pushstring(L, s->name);

  return 1;
}

static struct luaL_Reg system_methods[] = {
    {"get_name", method_get_name},
    {"requires", method_requires},
    {"excludes", method_excludes},
    {"set_mask", method_set_mask},
    {"get_mask", method_get_mask},
    {"enable", method_enable},
    {"disable", method_disable},
    {"on_run", method_on_run},
    {"run", method_run},
    {"get_entity_count", method_get_entity_count},
    {NULL, NULL}};

static int system_gc(lua_State* L) {
  lg_system* s = ((ptr2ptr*)luaL_checkudata(L, 1, "ClassSystemMT"))->ptr;

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
  ptr2ptr* ud = lua_newuserdatauv(L, sizeof(*ud), 0);
  lg_system* s;

  // TODO: Add realloc
  s = &systems[systems_count++];
  ud->ptr = s;

  s->id = ecs_define_system(ecs, 0, system_lua_wrapper, NULL, NULL, NULL).id;

  s->name = malloc(strlen(cname) + 1);
  strcpy(s->name, cname);

  luaL_getmetatable(L, "ClassSystemMT");
  lua_setmetatable(L, -2);

  DEBUG_LOG("LG: REGISTERED " SYST_FL, SYST_FL_ARGS(s));

  return 1;
}

static int system_register_content(lua_State* L) {
  lua_newtable(L);
  lua_pushcfunction(L, system_new);
  lua_setfield(L, -2, "new");

  return 1;
}

void lg_system_create(lua_State* L) {
  DEBUG_LOG("LG: SYSTEM CREATE");

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
  DEBUG_LOG("LG: SYSTEM DESTROY");

  for (ecs_id_t i = 0; i < systems_count; i++) {
    free(systems[i].name);
  }

  free(systems);
  systems_count = systems_size = 0;
}
