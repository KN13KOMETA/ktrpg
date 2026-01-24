#pragma once
#ifndef LUAGLUE_CORE_H
#define LUAGLUE_CORE_H

#include <lua.h>
#include <pico_ecs.h>

void lg_create(lua_State* L, ecs_t* ecs);
void lg_destroy(void);

#endif  // !LUAGLUE_CORE_H
