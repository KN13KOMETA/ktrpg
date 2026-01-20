#pragma once
#ifndef LUAB_LUAB_M_H
#define LUAB_LUAB_M_H

#include <lauxlib.h>
#include <lua.h>

extern struct luaL_Reg luab_m_l[];

int luab_m_register_integer_component(lua_State* L);
int luab_m_register_number_component(lua_State* L);
int luab_m_register_string_component(lua_State* L);

int luab_m_register_system(lua_State* L);

int luab_m_content_load_end(lua_State* L);

// Ecs functions
int luab_m_ecs_entity_create(lua_State* L);
int luab_m_ecs_entity_destroy(lua_State* L);

int luab_m_ecs_comp_has(lua_State* L);
int luab_m_ecs_comp_add(lua_State* L);
int luab_m_ecs_comp_set(lua_State* L);
int luab_m_ecs_comp_get(lua_State* L);
int luab_m_ecs_comp_remove(lua_State* L);

int luab_m_ecs_run_system(lua_State* L);
int luab_m_ecs_run_systems(lua_State* L);

// TODO: save/load system

#endif  // !LUAB_LUAB_M_H
