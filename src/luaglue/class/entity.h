#pragma once
#ifndef LUAGLUE_CLASS_ENTITY_H
#define LUAGLUE_CLASS_ENTITY_H

#include <lua.h>
#include <pico_ecs.h>

typedef struct {
  ecs_id_t id;
} lg_entity;

#define ID2ENTI(id) \
  (ecs_entity_t) { id }

#define ENTI_FL "Entity #%lu"
#define ENTI_FL_ARGS(e) e->id

void lg_entity_create(lua_State* L);
void lg_entity_destroy(void);

#endif  // !LUAGLUE_CLASS_ENTITY_H
