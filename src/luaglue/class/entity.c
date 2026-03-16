#include "entity.h"

#include <lauxlib.h>
#include <limits.h>
#include <lua.h>
#include <luaconf.h>
#include <pico_ecs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../constants.h"
#include "../../functions.h"
#include "../core.h"
#include "component.h"

static ecs_id_t array_limit;

static ptr2ecs* ecs;
static lg_entity* entities = NULL;
static ecs_id_t entities_count = 0;
static ecs_id_t entities_max = 0;

static int entities_estimated_size = ENTI_ESTIMATED_SIZE + sizeof(*entities);

static int method_kill(lua_State* L) {
  lg_entity* e = ((ptr2ptr*)luaL_checkudata(L, 1, "ClassEntityMT"))->ptr;

  if (ecs_is_invalid_entity(ID2ENTI(e->id)) ||
      !ecs_is_ready(ecs->ptr, ID2ENTI(e->id))) {
    lua_pushboolean(L, 0);
    lua_pushstring(L, "entity not ready or invalid");
    return 2;
  }

  ecs_queue_destroy(ecs->ptr, ID2ENTI(e->id));

  lua_pushboolean(L, 1);
  return 1;
}

static int method_remove(lua_State* L) {
  lg_entity* e = ((ptr2ptr*)luaL_checkudata(L, 1, "ClassEntityMT"))->ptr;
  lg_component* c = ((ptr2ptr*)luaL_checkudata(L, 2, "ClassComponentMT"))->ptr;

  if (ecs_is_invalid_entity(ID2ENTI(e->id)) ||
      !ecs_is_ready(ecs->ptr, ID2ENTI(e->id))) {
    lua_pushboolean(L, 0);
    lua_pushstring(L, "entity not ready or invalid");
    return 2;
  }

  if (!ecs_has(ecs->ptr, ID2ENTI(e->id), ID2COMP(c->id))) {
    lua_pushboolean(L, 0);
    lua_pushstring(L, "entity doesn't have component");
    return 2;
  }

  ecs_remove(ecs->ptr, ID2ENTI(e->id), ID2COMP(c->id));

  lua_pushboolean(L, 1);
  return 1;
}

static int method_get(lua_State* L) {
  lg_entity* e = ((ptr2ptr*)luaL_checkudata(L, 1, "ClassEntityMT"))->ptr;
  lg_component* c = ((ptr2ptr*)luaL_checkudata(L, 2, "ClassComponentMT"))->ptr;
  void* ec;

  if (ecs_is_invalid_entity(ID2ENTI(e->id)) ||
      !ecs_is_ready(ecs->ptr, ID2ENTI(e->id))) {
    lua_pushnil(L);
    lua_pushstring(L, "entity not ready or invalid");
    return 2;
  }

  if (!ecs_has(ecs->ptr, ID2ENTI(e->id), ID2COMP(c->id))) {
    lua_pushnil(L);
    lua_pushstring(L, "entity doesn't have component");
    return 2;
  }

  ec = ecs_get(ecs->ptr, ID2ENTI(e->id), ID2COMP(c->id));

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
      !ecs_is_ready(ecs->ptr, ID2ENTI(e->id))) {
    lua_pushnil(L);
    lua_pushstring(L, "entity not ready or invalid");
    return 2;
  }

  if (ecs_has(ecs->ptr, ID2ENTI(e->id), ID2COMP(c->id))) {
    ec = ecs_get(ecs->ptr, ID2ENTI(e->id), ID2COMP(c->id));
  } else {
    ec = ecs_add(ecs->ptr, ID2ENTI(e->id), ID2COMP(c->id), NULL);
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

      FMALLOC((*(lg_component_str*)ec).str, strlen(s) + 1);
      strcpy((*(lg_component_str*)ec).str, s);

      lua_pushstring(L, (*(lg_component_str*)ec).str);
      break;
    }
  }

  return 1;
}

static int method_get_id(lua_State* L) {
  lg_entity* e = ((ptr2ptr*)luaL_checkudata(L, 1, "ClassEntityMT"))->ptr;

  lua_pushinteger(L, (int)e->id);
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
  int count = (int)entities_count;

  lua_createtable(L, count, 0);
  for (int i = 0; i < count; i++) {
    ud = lua_newuserdatauv(L, sizeof(*ud), 0);
    ud->ptr = &entities[i];

    luaL_getmetatable(L, "ClassEntityMT");
    lua_setmetatable(L, -2);

    lua_rawseti(L, -2, i + 1);
  }

  lua_pushinteger(L, count);

  return 2;
}

static int entity_by_id(lua_State* L) {
  ptr2ptr* ud;
  lua_Integer tid = luaL_checkinteger(L, 2);

  for (ecs_id_t i = 0; i < entities_count; i++) {
    if (entities[i].id == (ecs_id_t)tid) {
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
  ptr2ptr* ud;
  lg_entity* e;

  if (ecs->ptr == NULL) return luaL_error(L, ECS_IS_NULL);
  ecs->modified = 1;

  if (entities_count + 1 > entities_max) {
    lua_pushnil(L);
    lua_pushstring(L, "entity limit reached");
    return 2;
  }

  ud = lua_newuserdatauv(L, sizeof(*ud), 0);

  e = &entities[entities_count++];
  ud->ptr = e;

  e->id = ecs_create(ecs->ptr).id;

  luaL_getmetatable(L, "ClassEntityMT");
  lua_setmetatable(L, -2);

  DEBUG_LOG("LG: REGISTRED " ENTI_FL, ENTI_FL_ARGS(e));

  return 1;
}

static int entity_set_limit(lua_State* L) {
  ecs_id_t limit = (ecs_id_t)luaL_checkinteger(L, 2);
  void* ptr;

  if (limit < 1) {
    lua_pushnil(L);
    lua_pushstring(L, "limit must be at least 1");
    return 2;
  }

  if (limit > array_limit) {
    char str[256];

    sprintf(str, "limit cannot exceed %" PRIu64, limit);

    lua_pushnil(L);
    lua_pushstring(L, str);

    return 2;
  }

  if (entities_count != 0) {
    lua_pushnil(L);
    lua_pushstring(L, "cannot set limit after entities have been created");
    return 2;
  }

  if (ecs->modified) return luaL_error(L, ECS_MODIFIED);

  if (limit != entities_count) {
    if (limit > ENTI_SOFT_LIMIT) {
      double result;
      char unit = human_bytes(
          (size_t)(limit * (ecs_id_t)entities_estimated_size), &result);

      printf(TITLE("WARNING"));
      printf("Script requested entities limit: %" PRIu64 "\n", limit);
      printf("This exceeds the soft limit of %d.\n", ENTI_SOFT_LIMIT);
      printf("Estimated memory usage: %.2f%c\n", result, unit);

      if (ask_yn("Are you sure you want to continue?")) {
        lua_pushliteral(L, LG_EXIT_USER);
        return lua_error(L);
      }
    }

    ptr = malloc(sizeof(*entities) * (size_t)limit);

    if (ptr == NULL) {
      lua_pushnil(L);
      lua_pushstring(L, "memory allocation failed");
      return 2;
    }

    free(entities);
    entities = ptr;
    entities_max = limit;

    if (ecs->ptr != NULL) ecs_free(ecs->ptr);
    ecs->ptr = ecs_new((size_t)limit, NULL);

    DEBUG_LOG("LG: ENTITY ARRAY SIZE = %" PRIu64, entities_max);
  }

  lua_pushinteger(L, (lua_Integer)limit);
  return 1;
}

static int entity_get_limit(lua_State* L) {
  lua_pushinteger(L, (lua_Integer)entities_max);
  return 1;
}

static luaL_Reg entity_class_methods[] = {{"all", entity_all},
                                          {"by_id", entity_by_id},
                                          {"new", entity_new},
                                          {"set_limit", entity_set_limit},
                                          {"get_limit", entity_get_limit},
                                          {NULL, NULL}};

static int entity_register_content(lua_State* L) {
  lua_newtable(L);

  luaL_setfuncs(L, entity_class_methods, 0);

  return 1;
}

void lg_entity_create(lua_State* L) {
  DEBUG_LOG("LG: ENTITY CREATE");

  DEBUG_LOG("LG: ENTITY ESTIMATED SIZE = %d", entities_estimated_size);

  lua_getfield(L, LUA_REGISTRYINDEX, "ecs");
  ecs = lua_touserdata(L, -1);
  lua_pop(L, 1);

  // Limit is so low because max table length is int
  array_limit = ((LUA_MAXINTEGER < INT_MAX) ? LUA_MAXINTEGER : INT_MAX) /
                sizeof(*entities);

  DEBUG_LOG("LG: ENTITY ARRAY LIMIT %" PRIu64, array_limit);

  ecs->ptr = ecs_new(ENTI_DEFAULT_MAX, NULL);

  entities_max = ENTI_DEFAULT_MAX;
  entities_count = 0;
  SELFMALLOCARR(entities, (size_t)entities_max);

  DEBUG_LOG("LG: ENTITY ARRAY SIZE = %" PRIu64, entities_max);

  entity_init_metatable(L);

  entity_register_content(L);
}
void lg_entity_destroy(void) {
  DEBUG_LOG("LG: ENTITY DESTROY");

  free(entities);
  entities_count = entities_max = 0;
}
