#include "luab_m.h"

#include <lauxlib.h>
#include <stdint.h>
#include <stdio.h>

#include "lua.h"
#include "luab_helper.h"

struct luaL_Reg luab_m_l[] = {
    {"register_integer_component", luab_m_register_integer_component},
    {"register_number_component", luab_m_register_number_component},
    {"register_string_component", luab_m_register_string_component},
    {"register_system", luab_m_register_system},
    {"ecs_entity_create", luab_m_ecs_entity_create},
    {"ecs_entity_destroy", luab_m_ecs_entity_destroy},
    {"ecs_comp_has", luab_m_ecs_comp_has},
    {"ecs_comp_add", luab_m_ecs_comp_add},
    {"ecs_comp_set", luab_m_ecs_comp_set},
    {"ecs_comp_get", luab_m_ecs_comp_get},
    {"ecs_comp_remove", luab_m_ecs_comp_remove},
    {"ecs_run_system", luab_m_ecs_run_system},
    {"ecs_run_systems", luab_m_ecs_run_systems},
    {NULL, NULL}};

// TODO: Add checks if too many componensts/systems added
int luab_m_register_integer_component(lua_State* L) {
  luab_state* lb = lua_touserdata(L, lua_upvalueindex(1));

  lua_pushinteger(L, luab_h_register_integer_component(lb));

  return 1;
}
int luab_m_register_number_component(lua_State* L) {
  luab_state* lb = lua_touserdata(L, lua_upvalueindex(1));

  lua_pushinteger(L, luab_h_register_number_component(lb));

  return 1;
}
int luab_m_register_string_component(lua_State* L) {
  luab_state* lb = lua_touserdata(L, lua_upvalueindex(1));

  lua_pushinteger(L, luab_h_register_string_component(lb));

  return 1;
}

// TODO: include exclude components
int luab_m_register_system(lua_State* L) {
  luab_state* lb = lua_touserdata(L, lua_upvalueindex(1));

  luaL_checktype(L, 1, LUA_TFUNCTION);
  lua_pushvalue(L, 1);

  lua_pushinteger(L,
                  luab_h_register_system(lb, luaL_ref(L, LUA_REGISTRYINDEX)));

  return 1;
}

// TODO: implementation
int luab_m_ecs_entity_create(lua_State* L) {
  luab_state* lb = lua_touserdata(L, lua_upvalueindex(1));

  lua_pushinteger(L, luab_h_ecs_entity_create(lb));

  return 1;
}
int luab_m_ecs_entity_destroy(lua_State* L) { return 0; }

int luab_m_ecs_comp_has(lua_State* L) { return 0; }
int luab_m_ecs_comp_add(lua_State* L) {
  luab_state* lb = lua_touserdata(L, lua_upvalueindex(1));
  lua_Integer entity_id = luaL_checkinteger(L, 1);
  lua_Integer comp_id = luaL_checkinteger(L, 2);

  lua_Integer comp =
      luab_h_ecs_comp_add(lb, (uint32_t)entity_id, (uint16_t)comp_id);

  lua_pushlightuserdata(L, (void*)comp);

  return 1;
}
int luab_m_ecs_comp_set(lua_State* L) {
  luab_state* lb = lua_touserdata(L, lua_upvalueindex(1));
  lua_Integer comp_id = luaL_checkinteger(L, 1);
  // TODO: Checks for 2 arg
  void* lud_p = lua_touserdata(L, 2);
  lua_Integer int_value;
  lua_Number num_value;
  char* str_value;

  switch (lb->comp_types[comp_id]) {
    case COMP_INTEGER:
      int_value = luaL_checkinteger(L, 3);
      *(lua_Integer*)lud_p = int_value;
      break;
    case COMP_NUMBER:
      num_value = luaL_checknumber(L, 3);
      *(lua_Number*)lud_p = num_value;
      break;
    case COMP_STRING:
      // TODO: Malloc string because string value gets cleared by lua
      str_value = luaL_checkstring(L, 3);
      *(char**)lud_p = str_value;
      break;
  }

  return 0;
}
int luab_m_ecs_comp_get(lua_State* L) {
  luab_state* lb = lua_touserdata(L, lua_upvalueindex(1));
  lua_Integer entity_id = luaL_checkinteger(L, 1);
  lua_Integer comp_id = luaL_checkinteger(L, 2);
  ecs_entity_t entity = {entity_id};

  if (!ecs_has(lb->ecs, entity, lb->comps[comp_id])) {
    lua_pushnil(L);
    return 1;
  }

  switch (lb->comp_types[comp_id]) {
    case COMP_INTEGER:
      lua_pushinteger(
          L, *(lua_Integer*)ecs_get(lb->ecs, entity, lb->comps[comp_id]));
      return 1;
    case COMP_NUMBER:
      lua_pushnumber(
          L, *(lua_Number*)ecs_get(lb->ecs, entity, lb->comps[comp_id]));
      return 1;
    case COMP_STRING:
      lua_pushstring(L, *(char**)ecs_get(lb->ecs, entity, lb->comps[comp_id]));
      return 1;
  }

  lua_pushnil(L);
  return 1;
}
// TODO: write these luab_m function]
int luab_m_ecs_comp_remove(lua_State* L) { return 0; }

int luab_m_ecs_run_system(lua_State* L) {
  luab_state* lb = lua_touserdata(L, lua_upvalueindex(1));
  return 0;
}
int luab_m_ecs_run_systems(lua_State* L) {
  luab_state* lb = lua_touserdata(L, lua_upvalueindex(1));

  ecs_run_systems(lb->ecs, 0);

  return 0;
}
