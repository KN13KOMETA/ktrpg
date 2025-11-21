#pragma once
#ifndef TGG_TGG_LUA_H

#include "lauxlib.h"
#include "lua.h"
#include "tgg.h"

extern struct luaL_Reg luagg_tgg_module[];

void luagg_register_module(lua_State* L, ecs_t* tgg_ecs, char* name,
                           struct luaL_Reg* methods);

#endif  // !TGG_TGG_LUA_H
