#include "entity.h"

#include <lauxlib.h>
#include <lua.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../constants.h"
#include "../../functions.h"
#include "component.h"

static ecs_t* ecs;
static lg_entity* entities;
static ecs_id_t entities_count = 0;
static ecs_id_t entities_size = 0;

static int method_remove(lua_State* L) {
  lg_entity* e = ((ptr2ptr*)luaL_checkudata(L, 1, "ClassEntityMT"))->ptr;
  lg_component* c = ((ptr2ptr*)luaL_checkudata(L, 2, "ClassComponentMT"))->ptr;

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
  void* ec = ecs_add(ecs, ID2ENTI(e->id), ID2COMP(c->id), NULL);

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

static struct luaL_Reg entity_methods[] = {{"get_id", method_get_id},
                                           {"set", method_set},
                                           {"get", method_get},
                                           {"remove", method_remove},
                                           {NULL, NULL}};

static void entity_init_metatable(lua_State* L) {
  luaL_newmetatable(L, "ClassEntityMT");
  lua_pushvalue(L, -1);

  lua_setfield(L, -2, "__index");
  luaL_setfuncs(L, entity_methods, 0);

  lua_pop(L, 1);
}

static int entity_by_id(lua_State* L) {
  ptr2ptr* ud;
  lua_Integer tid = luaL_checkinteger(L, 2);
  lg_entity* e;

  for (ecs_id_t i = 0; i < entities_count; i++) {
    if (entities[i].id == tid) {
      ud = lua_newuserdatauv(L, sizeof(*ud), 0);
      ud->ptr;
      return 1;
    }
  }

  lua_pushnil(L);
  return 1;
}

static int entity_new(lua_State* L) {
  ptr2ptr* ud = lua_newuserdatauv(L, sizeof(*ud), 0);
  lg_entity* e;

  // TODO: Add realloc
  e = &entities[entities_count++];
  ud->ptr = e;

  e->id = ecs_create(ecs).id;

  luaL_getmetatable(L, "ClassEntityMT");
  lua_setmetatable(L, -2);

  DEBUG_LOG("LG: REGISTRED " ENTI_FL, ENTI_FL_ARGS(e));

  return 1;
}

static luaL_Reg entity_class_methods[] = {
    {"by_id", entity_by_id}, {"new", entity_new}, {NULL, NULL}};

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

  entities_size = UINT16_MAX;
  entities_count = 0;
  entities = malloc(sizeof(*entities) * entities_size);

  entity_init_metatable(L);

  entity_register_content(L);
}
void lg_entity_destroy(void) {
  DEBUG_LOG("LG: ENTITY DESTROY");

  free(entities);
  entities_count = entities_size = 0;
}
