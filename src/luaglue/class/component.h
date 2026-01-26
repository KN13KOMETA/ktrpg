#pragma once
#ifndef LUAGLUE_CLASS_COMPONENT_H
#define LUAGLUE_CLASS_COMPONENT_H

#include <lua.h>
#include <pico_ecs.h>

typedef enum { COMP_INT, COMP_NUM, COMP_TAG, COMP_STR } LG_COMPONENT_TYPE;
typedef struct {
  char* name;
  LG_COMPONENT_TYPE type;
  ecs_id_t id;
} lg_component;

#define ID2COMP(id) \
  (ecs_comp_t) { id }

#define COMP_FL "Component #%lu \"%s\" (%s)"
// TODO: WTF is clangd doing
#define COMP_FL_ARGS(c)                    \
  c->id, c->name,                          \
      ((c->type == COMP_INT)               \
           ? "INT"                         \
           : ((c->type == COMP_NUM)        \
                  ? "NUM"                  \
                  : ((c->type == COMP_TAG) \
                         ? "TAG"           \
                         : ((c->type == COMP_STR) ? "STR" : "ERR"))))

void lg_component_create(lua_State* L);
void lg_component_destroy(void);

#endif  // !LUAGLUE_CLASS_COMPONENT_H
