#pragma once
#ifndef LUAGLUE_CORE_H
#define LUAGLUE_CORE_H

#include <lua.h>
#include <pico_ecs.h>
#include <stdint.h>

#define ECS_IS_NULL "ecs is nil"
#define ECS_MODIFIED "ecs already modified"

#define ENTI_DEFAULT_MAX UINT16_MAX
#define COMP_DEFAULT_MAX UINT8_MAX
#define SYST_DEFAULT_MAX UINT8_MAX

typedef struct {
  ecs_t* ptr;
  uint8_t modified;
} ptr2ecs;

void lg_create(lua_State* L);
void lg_destroy(void);

#endif  // !LUAGLUE_CORE_H
