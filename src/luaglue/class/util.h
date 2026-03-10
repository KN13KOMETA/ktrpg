#pragma once
#ifndef LUAGLUE_CLASS_UTIL_H
#define LUAGLUE_CLASS_UTIL_H

#include <lua.h>
#include <pico_ecs.h>

void lg_util_create(lua_State* L);
void lg_util_destroy(void);

#endif  // !LUAGLUE_CLASS_UTIL_H
