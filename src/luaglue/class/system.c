#include "system.h"

#include <lauxlib.h>
#include <lua.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../constants.h"
#include "../../functions.h"
#include "component.h"
#include "entity.h"

static ecs_t* ecs;
static lua_State* lstate;
static lg_system* systems;
static ecs_id_t systems_count = 0;
static ecs_id_t systems_max = 0;

static uint8_t debug_system_valid = 0;
static ecs_id_t debug_system = 0;

static ecs_ret_t lua_runner_system(ecs_t* ecs, ecs_entity_t* raw_entities,
                                   size_t entity_count, void* udata) {
  lua_State* L = lstate;
  lg_system* s = udata;

  (void)ecs;

  if (s->lua_ref == LUA_NOREF) {
    DEBUG_LOG("LG: LRS " SYST_FL " DOES NOT HAVE LUA FUNCTION",
              SYST_FL_ARGS(s));
  }

  DEBUG_LOG("LG: LRS RUNNING " SYST_FL, SYST_FL_ARGS(s));

  {
    lg_entity* entities = malloc(sizeof(*entities) * entity_count);

    lua_rawgeti(L, LUA_REGISTRYINDEX, s->lua_ref);

    // Create lua table with entities id
    lua_createtable(L, entity_count, 0);
    for (size_t i = 0; i < entity_count; i++) {
      {
        ptr2ptr* ud = lua_newuserdatauv(L, sizeof(*ud), 0);
        lg_entity* e = &entities[i];

        ud->ptr = e;

        e->id = raw_entities[i].id;

        luaL_getmetatable(L, "ClassEntityMT");
        lua_setmetatable(L, -2);

        lua_rawseti(L, -2, i + 1);
      }
    }

    lua_pushinteger(L, entity_count);

    lua_pcall(L, 2, 0, 1);

    free(entities);
  }

  return 0;
}

static int method_get_entity_count(lua_State* L) {
  lg_system* s = ((ptr2ptr*)luaL_checkudata(L, 1, "ClassSystemMT"))->ptr;

  lua_pushinteger(L, ecs_get_system_entity_count(ecs, ID2SYST(s->id)));

  return 1;
}

static int method_run(lua_State* L) {
  lg_system* s = ((ptr2ptr*)luaL_checkudata(L, 1, "ClassSystemMT"))->ptr;

  ecs_run_system(ecs, ID2SYST(s->id), ecs_get_system_mask(ecs, ID2SYST(s->id)));

  lua_pushvalue(L, 1);
  return 1;
}
static int method_on_run(lua_State* L) {
  lg_system* s = ((ptr2ptr*)luaL_checkudata(L, 1, "ClassSystemMT"))->ptr;

  luaL_checktype(L, 2, LUA_TFUNCTION);
  lua_pushvalue(L, 2);
  s->lua_ref = luaL_ref(L, LUA_REGISTRYINDEX);

  lua_pushvalue(L, 1);
  return 1;
}

static int method_disable(lua_State* L) {
  lg_system* s = ((ptr2ptr*)luaL_checkudata(L, 1, "ClassSystemMT"))->ptr;

  ecs_disable_system(ecs, ID2SYST(s->id));

  lua_pushvalue(L, 1);
  return 1;
}
static int method_enable(lua_State* L) {
  lg_system* s = ((ptr2ptr*)luaL_checkudata(L, 1, "ClassSystemMT"))->ptr;

  ecs_enable_system(ecs, ID2SYST(s->id));

  lua_pushvalue(L, 1);
  return 1;
}

static int method_get_mask(lua_State* L) {
  lg_system* s = ((ptr2ptr*)luaL_checkudata(L, 1, "ClassSystemMT"))->ptr;

  // NOTE: This conversation may lose data
  // but since we are only setting system mask
  // from lua_Integer (signed) this should be fine
  lua_pushinteger(L, (lua_Integer)ecs_get_system_mask(ecs, ID2SYST(s->id)));

  return 1;
}
static int method_set_mask(lua_State* L) {
  lg_system* s = ((ptr2ptr*)luaL_checkudata(L, 1, "ClassSystemMT"))->ptr;
  lua_Integer mask = luaL_checkinteger(L, 2);

  if (mask < 0) return luaL_argerror(L, 2, "expected positive integer");

  ecs_set_system_mask(ecs, ID2SYST(s->id), (ecs_mask_t)mask);

  lua_pushvalue(L, 1);
  return 1;
}

static int method_excludes(lua_State* L) {
  lg_system* s = ((ptr2ptr*)luaL_checkudata(L, 1, "ClassSystemMT"))->ptr;
  int n = lua_gettop(L);

  for (int i = 2; i < n + 1; i++) {
    lg_component* c =
        ((ptr2ptr*)luaL_checkudata(L, i, "ClassComponentMT"))->ptr;

    ecs_exclude_component(ecs, ID2SYST(s->id), ID2COMP(c->id));

    DEBUG_LOG("LG: " SYST_FL " EXCLUDES " COMP_FL, SYST_FL_ARGS(s),
              COMP_FL_ARGS(c));
  }

  lua_pushvalue(L, 1);
  return 1;
}
static int method_requires(lua_State* L) {
  lg_system* s = ((ptr2ptr*)luaL_checkudata(L, 1, "ClassSystemMT"))->ptr;
  int n = lua_gettop(L);

  for (int i = 2; i < n + 1; i++) {
    lg_component* c =
        ((ptr2ptr*)luaL_checkudata(L, i, "ClassComponentMT"))->ptr;

    ecs_require_component(ecs, ID2SYST(s->id), ID2COMP(c->id));

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

static int system_run_debug_system(lua_State* L) {
  if (debug_system_valid == 0) {
    lua_pushboolean(L, 0);
    return 1;
  }

  ecs_run_system(ecs, ID2SYST(debug_system), 0);

  lua_pushboolean(L, 1);
  return 1;
}

static int system_new(lua_State* L) {
  const char* cname = luaL_checkstring(L, 2);
  ptr2ptr* ud = lua_newuserdatauv(L, sizeof(*ud), 0);
  lg_system* s;

  // TODO: Add realloc
  s = &systems[systems_count++];
  ud->ptr = s;

  s->lua_ref = LUA_NOREF;

  s->id = ecs_define_system(ecs, 0, lua_runner_system, NULL, NULL, s).id;

  s->name = malloc(strlen(cname) + 1);
  strcpy(s->name, cname);

  luaL_getmetatable(L, "ClassSystemMT");
  lua_setmetatable(L, -2);

  DEBUG_LOG("LG: REGISTERED " SYST_FL, SYST_FL_ARGS(s));

  return 1;
}

static struct luaL_Reg system_class_methods[] = {
    {"new", system_new},
    {"run_debug_system", system_run_debug_system},
    {NULL, NULL}};

static int system_register_content(lua_State* L) {
  lua_newtable(L);

  luaL_setfuncs(L, system_class_methods, 0);

  return 1;
}

void lg_system_create(lua_State* L) {
  DEBUG_LOG("LG: SYSTEM CREATE");

  lua_getfield(L, LUA_REGISTRYINDEX, "ecs");
  ecs = lua_touserdata(L, -1);
  lua_pop(L, 1);

  lstate = L;

  systems_max = UINT8_MAX;
  systems_count = 0;
  systems = malloc(sizeof(*systems) * systems_max);

  system_init_metatable(L);

  system_register_content(L);

  debug_system_valid = 1;
  debug_system =
      ecs_define_system(ecs, 0, lg_component_debug_system, NULL, NULL, NULL).id;
}
void lg_system_destroy(void) {
  DEBUG_LOG("LG: SYSTEM DESTROY");

  for (ecs_id_t i = 0; i < systems_count; i++) {
    free(systems[i].name);
  }

  free(systems);
  systems_count = systems_max = 0;
}
