#include "entity.h"

#include <lauxlib.h>
#include <limits.h>
#include <lua.h>
#include <pico_ecs.h>
#include <stdlib.h>
#include <string.h>

#include "../../constants.h"
#include "../../functions.h"
#include "component.h"

static ecs_id_t array_limit;

static ecs_t* ecs;
static lg_entity* entities = NULL;
static ecs_id_t entities_count = 0;
static ecs_id_t entities_max = 0;

static int method_kill(lua_State* L) {
  lg_entity* e = ((ptr2ptr*)luaL_checkudata(L, 1, "ClassEntityMT"))->ptr;

  if (ecs_is_invalid_entity(ID2ENTI(e->id)) ||
      !ecs_is_ready(ecs, ID2ENTI(e->id))) {
    lua_pushnil(L);
    return 1;
  }

  ecs_queue_destroy(ecs, ID2ENTI(e->id));

  return 0;
}

static int method_remove(lua_State* L) {
  lg_entity* e = ((ptr2ptr*)luaL_checkudata(L, 1, "ClassEntityMT"))->ptr;
  lg_component* c = ((ptr2ptr*)luaL_checkudata(L, 2, "ClassComponentMT"))->ptr;

  if (ecs_is_invalid_entity(ID2ENTI(e->id)) ||
      !ecs_is_ready(ecs, ID2ENTI(e->id))) {
    lua_pushnil(L);
    return 1;
  }

  if (!ecs_has(ecs, ID2ENTI(e->id), ID2COMP(c->id))) {
    lua_pushboolean(L, 0);
    return 1;
  }

  ecs_remove(ecs, ID2ENTI(e->id), ID2COMP(c->id));

  lua_pushboolean(L, 1);
  return 1;
}

static int method_get(lua_State* L) {
  lg_entity* e = ((ptr2ptr*)luaL_checkudata(L, 1, "ClassEntityMT"))->ptr;
  lg_component* c = ((ptr2ptr*)luaL_checkudata(L, 2, "ClassComponentMT"))->ptr;
  void* ec;

  if (ecs_is_invalid_entity(ID2ENTI(e->id)) ||
      !ecs_is_ready(ecs, ID2ENTI(e->id))) {
    lua_pushnil(L);
    return 1;
  }

  if (!ecs_has(ecs, ID2ENTI(e->id), ID2COMP(c->id))) {
    lua_pushnil(L);
    return 1;
  }

  ec = ecs_get(ecs, ID2ENTI(e->id), ID2COMP(c->id));

  switch (c->type) {
    case COMP_INT:
      lua_pushinteger(L, *(lua_Integer*)ec);
      break;
    case COMP_NUM:
      lua_pushnumber(L, *(lua_Number*)ec);
      break;
    case COMP_TAG:
      lua_pushinteger(L, *(uint8_t*)ec);
      break;
    case COMP_STR: {
      lua_pushstring(L, (*(lg_component_str*)ec).str);
      break;
    }
  }

  return 1;
}

static int method_set(lua_State* L) {
  lg_entity* e = ((ptr2ptr*)luaL_checkudata(L, 1, "ClassEntityMT"))->ptr;
  lg_component* c = ((ptr2ptr*)luaL_checkudata(L, 2, "ClassComponentMT"))->ptr;
  void* ec;

  if (ecs_is_invalid_entity(ID2ENTI(e->id)) ||
      !ecs_is_ready(ecs, ID2ENTI(e->id))) {
    lua_pushnil(L);
    return 1;
  }

  if (ecs_has(ecs, ID2ENTI(e->id), ID2COMP(c->id))) {
    ec = ecs_get(ecs, ID2ENTI(e->id), ID2COMP(c->id));
  } else {
    ec = ecs_add(ecs, ID2ENTI(e->id), ID2COMP(c->id), NULL);
  }

  switch (c->type) {
    case COMP_INT:
      *(lua_Integer*)ec = luaL_checkinteger(L, 3);
      lua_pushinteger(L, *(lua_Integer*)ec);
      break;
    case COMP_NUM:
      *(lua_Number*)ec = luaL_checknumber(L, 3);
      lua_pushnumber(L, *(lua_Number*)ec);
      break;
    case COMP_TAG:
      *(uint8_t*)ec = (uint8_t)luaL_checkinteger(L, 3);
      lua_pushinteger(L, *(uint8_t*)ec);
      break;
    case COMP_STR: {
      const char* s = luaL_checkstring(L, 3);

      (*(lg_component_str*)ec).str = malloc(strlen(s) + 1);
      strcpy((*(lg_component_str*)ec).str, s);

      lua_pushstring(L, (*(lg_component_str*)ec).str);
      break;
    }
  }

  return 1;
}

static int method_get_id(lua_State* L) {
  lg_entity* e = ((ptr2ptr*)luaL_checkudata(L, 1, "ClassEntityMT"))->ptr;

  lua_pushinteger(L, e->id);
  return 1;
}

static struct luaL_Reg entity_methods[] = {
    {"get_id", method_get_id}, {"set", method_set},   {"get", method_get},
    {"remove", method_remove}, {"kill", method_kill}, {NULL, NULL}};

static void entity_init_metatable(lua_State* L) {
  luaL_newmetatable(L, "ClassEntityMT");
  lua_pushvalue(L, -1);

  lua_setfield(L, -2, "__index");
  luaL_setfuncs(L, entity_methods, 0);

  lua_pop(L, 1);
}

static int entity_all(lua_State* L) {
  ptr2ptr* ud;

  lua_createtable(L, entities_count, 0);
  for (ecs_id_t i = 0; i < entities_count; i++) {
    ud = lua_newuserdatauv(L, sizeof(*ud), 0);
    ud->ptr = &entities[i];

    luaL_getmetatable(L, "ClassEntityMT");
    lua_setmetatable(L, -2);

    lua_rawseti(L, -2, i + 1);
  }

  lua_pushinteger(L, entities_count);

  return 2;
}

static int entity_by_id(lua_State* L) {
  ptr2ptr* ud;
  lua_Integer tid = luaL_checkinteger(L, 2);

  for (ecs_id_t i = 0; i < entities_count; i++) {
    if (entities[i].id == tid) {
      ud = lua_newuserdatauv(L, sizeof(*ud), 0);
      ud->ptr = &entities[i];

      luaL_getmetatable(L, "ClassEntityMT");
      lua_setmetatable(L, -2);

      return 1;
    }
  }

  lua_pushnil(L);
  lua_pushstring(L, "entity not found");
  return 2;
}

static int entity_new(lua_State* L) {
  ptr2ptr* ud = lua_newuserdatauv(L, sizeof(*ud), 0);
  lg_entity* e;

  if (entities_count + 1 > entities_max) {
    lua_pushnil(L);
    lua_pushstring(L, "entity limit reached");
    return 2;
  }

  e = &entities[entities_count++];
  ud->ptr = e;

  e->id = ecs_create(ecs).id;

  luaL_getmetatable(L, "ClassEntityMT");
  lua_setmetatable(L, -2);

  DEBUG_LOG("LG: REGISTRED " ENTI_FL, ENTI_FL_ARGS(e));

  return 1;
}

static int entity_set_limit(lua_State* L) {
  ecs_id_t limit = (ecs_id_t)luaL_checkinteger(L, 2);
  void* ptr;

  if (limit < 1) {
    lua_pushboolean(L, 0);
    lua_pushstring(L, "limit must be at least 1");
    return 2;
  }

  if (limit > array_limit) {
    lua_pushboolean(L, 0);
    lua_pushstring(L, "limit cannot exceed " EXPAND2STR(ENTI_MAX));
    return 2;
  }

  if (entities_count != 0) {
    lua_pushboolean(L, 0);
    lua_pushstring(L, "cannot set limit after entities have been created");
    return 2;
  }

  if (limit == entities_count) {
    lua_pushboolean(L, 1);
    return 1;
  }

  ptr = malloc(sizeof(*entities) * limit);

  if (ptr == NULL) {
    lua_pushboolean(L, 0);
    lua_pushstring(L, "memory allocation failed");
    return 2;
  }

  free(entities);
  entities = ptr;
  entities_max = limit;

  DEBUG_LOG("LG: ENTITY ARRAY SIZE = %lu", entities_max);

  lua_pushboolean(L, 1);
  return 1;
}

static luaL_Reg entity_class_methods[] = {{"all", entity_all},
                                          {"by_id", entity_by_id},
                                          {"new", entity_new},
                                          {"set_limit", entity_set_limit},
                                          {NULL, NULL}};

static int entity_register_content(lua_State* L) {
  lua_newtable(L);

  luaL_setfuncs(L, entity_class_methods, 0);

  return 1;
}

void lg_entity_create(lua_State* L) {
  DEBUG_LOG("LG: ENTITY CREATE");

  lua_getfield(L, LUA_REGISTRYINDEX, "ecs");
  ecs = lua_touserdata(L, -1);
  lua_pop(L, 1);

  array_limit = LUA_MAXINTEGER / sizeof(*entities);

  DEBUG_LOG("LG: ENTITY ARRAY LIMIT %lu", array_limit);

  entities_max = UINT16_MAX;
  entities_count = 0;
  entities = malloc(sizeof(*entities) * entities_max);

  DEBUG_LOG("LG: ENTITY ARRAY SIZE = %lu", entities_max);

  entity_init_metatable(L);

  entity_register_content(L);
}
void lg_entity_destroy(void) {
  DEBUG_LOG("LG: ENTITY DESTROY");

  free(entities);
  entities_count = entities_max = 0;
}
