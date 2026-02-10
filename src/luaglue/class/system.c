#include "system.h"

#include <lauxlib.h>
#include <lua.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../constants.h"
#include "../../functions.h"
#include "../core.h"
#include "component.h"
#include "entity.h"

#define DEBUG_SYSTEM_NAME "__KTRPG_DEBUG"

static ecs_id_t array_limit;

static ptr2ecs* ecs;
static lua_State* lstate;
static lg_system* systems;
static ecs_id_t systems_count = 0;
static ecs_id_t systems_max = 0;

static ecs_ret_t lua_runner_system(ecs_t* ecs, ecs_entity_t* raw_entities,
                                   size_t entity_count, void* udata) {
  lua_State* L = lstate;
  lg_system* s = udata;
  int ecount = (int)entity_count;

  (void)ecs;

  if (s->lua_ref == LUA_NOREF) {
    DEBUG_LOG("LG: LRS " SYST_FL " DOES NOT HAVE LUA FUNCTION",
              SYST_FL_ARGS(s));
  }

  DEBUG_LOG("LG: LRS RUNNING " SYST_FL, SYST_FL_ARGS(s));

  {
    lg_entity* entities;
    SELFMALLOCARR(entities, entity_count);

    lua_rawgeti(L, LUA_REGISTRYINDEX, s->lua_ref);

    // Create lua table with entities id
    lua_createtable(L, ecount, 0);
    for (int i = 0; i < ecount; i++) {
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

    lua_pushinteger(L, ecount);

    lua_pcall(L, 2, 0, 1);

    free(entities);
  }

  return 0;
}

static int method_get_entity_count(lua_State* L) {
  lg_system* s = ((ptr2ptr*)luaL_checkudata(L, 1, "ClassSystemMT"))->ptr;

  lua_pushinteger(
      L, (lua_Integer)ecs_get_system_entity_count(ecs->ptr, ID2SYST(s->id)));

  return 1;
}

static int method_run(lua_State* L) {
  lg_system* s = ((ptr2ptr*)luaL_checkudata(L, 1, "ClassSystemMT"))->ptr;

  ecs_run_system(ecs->ptr, ID2SYST(s->id),
                 ecs_get_system_mask(ecs->ptr, ID2SYST(s->id)));

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

  ecs_disable_system(ecs->ptr, ID2SYST(s->id));

  lua_pushvalue(L, 1);
  return 1;
}
static int method_enable(lua_State* L) {
  lg_system* s = ((ptr2ptr*)luaL_checkudata(L, 1, "ClassSystemMT"))->ptr;

  ecs_enable_system(ecs->ptr, ID2SYST(s->id));

  lua_pushvalue(L, 1);
  return 1;
}

static int method_get_mask(lua_State* L) {
  lg_system* s = ((ptr2ptr*)luaL_checkudata(L, 1, "ClassSystemMT"))->ptr;

  // NOTE: This conversation may lose data
  // but since we are only setting system mask
  // from lua_Integer (signed) this should be fine
  lua_pushinteger(L,
                  (lua_Integer)ecs_get_system_mask(ecs->ptr, ID2SYST(s->id)));

  return 1;
}
static int method_set_mask(lua_State* L) {
  lg_system* s = ((ptr2ptr*)luaL_checkudata(L, 1, "ClassSystemMT"))->ptr;
  lua_Integer mask = luaL_checkinteger(L, 2);

  if (mask < 0) return luaL_argerror(L, 2, "expected positive integer");

  ecs_set_system_mask(ecs->ptr, ID2SYST(s->id), (ecs_mask_t)mask);

  lua_pushvalue(L, 1);
  return 1;
}

static int method_excludes(lua_State* L) {
  lg_system* s = ((ptr2ptr*)luaL_checkudata(L, 1, "ClassSystemMT"))->ptr;
  int n = lua_gettop(L);

  for (int i = 2; i < n + 1; i++) {
    lg_component* c =
        ((ptr2ptr*)luaL_checkudata(L, i, "ClassComponentMT"))->ptr;

    ecs_exclude_component(ecs->ptr, ID2SYST(s->id), ID2COMP(c->id));

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

    ecs_require_component(ecs->ptr, ID2SYST(s->id), ID2COMP(c->id));

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

static void system_init_metatable(lua_State* L) {
  luaL_newmetatable(L, "ClassSystemMT");
  lua_pushvalue(L, -1);

  lua_setfield(L, -2, "__index");
  luaL_setfuncs(L, system_methods, 0);

  lua_pop(L, 1);
}

static int system_new(lua_State* L) {
  const char* cname;
  ptr2ptr* ud;
  lg_system* s;

  if (ecs->ptr == NULL) return luaL_error(L, ECS_IS_NULL);
  ecs->modified = 1;

  if (systems_count + 1 > systems_max) {
    lua_pushnil(L);
    lua_pushstring(L, "system limit reached");
    return 2;
  }

  cname = luaL_checkstring(L, 2);

  ud = lua_newuserdatauv(L, sizeof(*ud), 0);
  s = &systems[systems_count++];
  ud->ptr = s;

  s->lua_ref = LUA_NOREF;

  if (strcmp(DEBUG_SYSTEM_NAME, cname) == 0) {
    s->id = ecs_define_system(ecs->ptr, 0, lg_component_debug_system, NULL,
                              NULL, NULL)
                .id;
  } else {
    s->id = ecs_define_system(ecs->ptr, 0, lua_runner_system, NULL, NULL, s).id;
  }

  FMALLOC(s->name, strlen(cname) + 1);
  strcpy(s->name, cname);

  luaL_getmetatable(L, "ClassSystemMT");
  lua_setmetatable(L, -2);

  DEBUG_LOG("LG: REGISTERED " SYST_FL, SYST_FL_ARGS(s));

  return 1;
}

static int system_set_limit(lua_State* L) {
  ecs_id_t limit = (ecs_id_t)luaL_checkinteger(L, 2);
  void* ptr;

  if (limit < 1) {
    lua_pushnil(L);
    lua_pushstring(L, "limit must be at least 1");
    return 2;
  }

  if (limit > array_limit) {
    char str[256];

    sprintf(str, "limit cannot exceed %lu", limit);

    lua_pushnil(L);
    lua_pushstring(L, str);

    return 2;
  }

  if (systems_count != 0) {
    lua_pushnil(L);
    lua_pushstring(L, "cannot set limit after entities have been created");
    return 2;
  }

  if (limit != systems_count) {
    ptr = malloc(sizeof(*systems) * limit);

    if (ptr == NULL) {
      lua_pushnil(L);
      lua_pushstring(L, "memory allocation failed");
      return 2;
    }

    free(systems);
    systems = ptr;
    systems_max = limit;

    DEBUG_LOG("LG: SYSTEM ARRAY SIZE = %lu", systems_max);
  }

  lua_pushinteger(L, (lua_Integer)limit);
  return 1;
}

static int system_get_limit(lua_State* L) {
  lua_pushinteger(L, (lua_Integer)systems_max);
  return 1;
}

static struct luaL_Reg system_class_methods[] = {
    {"new", system_new},
    {"set_limit", system_set_limit},
    {"get_limit", system_get_limit},
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

  array_limit = LUA_MAXINTEGER / sizeof(*systems);

  DEBUG_LOG("LG: SYSTEM ARRAY LIMIT %lu", array_limit);

  systems_max = SYST_DEFAULT_MAX;
  systems_count = 0;
  SELFMALLOCARR(systems, systems_max);

  DEBUG_LOG("LG: SYSTEM ARRAY SIZE = %lu", systems_max);

  system_init_metatable(L);

  system_register_content(L);
}
void lg_system_destroy(void) {
  DEBUG_LOG("LG: SYSTEM DESTROY");

  for (ecs_id_t i = 0; i < systems_count; i++) {
    free(systems[i].name);
  }

  free(systems);
  systems_count = systems_max = 0;
}
