#pragma once
#ifndef LUAGLUE_CORE_H
#define LUAGLUE_CORE_H

#include <lua.h>
#include <pico_ecs.h>

typedef struct {
  ecs_t* ptr;
  uint8_t modified;
} ptr2ecs;

void lg_create(lua_State* L);
void lg_destroy(void);

#endif  // !LUAGLUE_CORE_H
