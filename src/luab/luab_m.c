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

int luab_m_register_system(lua_State* L) {
  luab_state* lb = lua_touserdata(L, lua_upvalueindex(1));

  luaL_checktype(L, 1, LUA_TFUNCTION);
  lua_pushvalue(L, 1);

  lua_pushinteger(L,
                  luab_h_register_system(lb, luaL_ref(L, LUA_REGISTRYINDEX)));

  return 1;
}
