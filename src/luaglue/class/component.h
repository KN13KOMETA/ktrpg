#pragma once
#ifndef LUAGLUE_CLASS_COMPONENT_H
#define LUAGLUE_CLASS_COMPONENT_H

#include <lua.h>
#include <pico_ecs.h>

// TODO: Check if ecs system works with 0 size values
// and then add tag comp
typedef enum { COMP_INT, COMP_NUM, COMP_TAG, COMP_STR } LG_COMPONENT_TYPE;
typedef struct {
  char* name;
  LG_COMPONENT_TYPE type;
  ecs_id_t id;
} lg_component;

void lg_component_create(lua_State* L);
void lg_component_destroy(void);

#endif  // !LUAGLUE_CLASS_COMPONENT_H
