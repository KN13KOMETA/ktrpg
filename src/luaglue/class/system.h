#pragma once
#ifndef LUAGLUE_CLASS_SYSTEM_H
#define LUAGLUE_CLASS_SYSTEM_H

#include <lua.h>
#include <pico_ecs.h>

typedef struct {
  char* name;
  ecs_id_t id;
  int lua_ref;
} lg_system;

#define ID2SYST(s) \
  (ecs_system_t) { s }

#define SYST_FL "System \"%s\""
#define SYST_FL_ARGS(s) s->name

void lg_system_create(lua_State* L);
void lg_system_destroy(void);

#endif  // !LUAGLUE_CLASS_SYSTEM_H
