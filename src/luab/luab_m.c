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

  luab_h_ecs_comp_add(lb, (uint32_t)entity_id, (uint16_t)comp_id);

  return 0;
}
int luab_m_ecs_comp_get(lua_State* L) { return 0; }
int luab_m_ecs_comp_remove(lua_State* L) { return 0; }

int luab_m_ecs_run_system(lua_State* L) { return 0; }
int luab_m_ecs_run_systems(lua_State* L) { return 0; }
