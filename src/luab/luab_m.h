#pragma once
#ifndef LUAB_LUAB_M_H
#define LUAB_LUAB_M_H

#include <lauxlib.h>
#include <lua.h>

extern struct luaL_Reg luab_m_l[];

// TODO: Change component functions
int luab_m_register_integer_component(lua_State* L);
int luab_m_register_number_component(lua_State* L);
int luab_m_register_string_component(lua_State* L);
int luab_m_register_system(lua_State* L);

#endif  // !LUAB_LUAB_M_H
