#include "component.h"

#include <lauxlib.h>
#include <lua.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../constants.h"
#include "../../functions.h"
#include "entity.h"

static ecs_t* ecs;
static lg_component* comps;
static ecs_id_t comps_count = 0;
static ecs_id_t comps_size = 0;

static int method_get_name(lua_State* L) {
  lg_component* c = ((ptr2ptr*)luaL_checkudata(L, 1, "ClassComponentMT"))->ptr;

  lua_pushstring(L, c->name);

  return 1;
}

static struct luaL_Reg component_methods[] = {{"get_name", method_get_name},
                                              {NULL, NULL}};

static void component_init_metatable(lua_State* L) {
  luaL_newmetatable(L, "ClassComponentMT");
  lua_pushvalue(L, -1);

  lua_setfield(L, -2, "__index");
  luaL_setfuncs(L, component_methods, 0);

  lua_pop(L, 1);
}

static void component_str_destructor(ecs_t* ecs, ecs_entity_t entity,
                                     void* comp_ptr) {
  free(((lg_component_str*)comp_ptr)->str);
}

static int component_new(lua_State* L) {
  const char* ctype = luaL_checkstring(L, 2);
  const char* cname = luaL_checkstring(L, 3);
  ptr2ptr* ud = lua_newuserdatauv(L, sizeof(*ud), 0);
  lg_component* c;

  // TODO: Add realloc
  c = &comps[comps_count++];
  ud->ptr = c;

  if (strlen(ctype) != 3) goto lua_err;

  if (strncmp("int", ctype, 3) == 0) {
    c->type = COMP_INT;
    c->id = ecs_define_component(ecs, sizeof(lua_Integer), NULL, NULL).id;
  } else if (strncmp("num", ctype, 3) == 0) {
    c->type = COMP_NUM;
    c->id = ecs_define_component(ecs, sizeof(lua_Number), NULL, NULL).id;
  } else if (strncmp("tag", ctype, 3) == 0) {
    c->type = COMP_TAG;
    c->id = ecs_define_component(ecs, sizeof(uint8_t), NULL, NULL).id;
  } else if (strncmp("str", ctype, 3) == 0) {
    c->type = COMP_STR;
    c->id = ecs_define_component(ecs, sizeof(lg_component_str), NULL,
                                 component_str_destructor)
                .id;
  } else {
  lua_err:
    return luaL_argerror(L, 2, "expected \"int\", \"num\", \"tag\" or \"str\"");
  }

  c->name = malloc(strlen(cname) + 1);
  strcpy(c->name, cname);

  luaL_getmetatable(L, "ClassComponentMT");
  lua_setmetatable(L, -2);

  DEBUG_LOG("LG: REGISTRED " COMP_FL, COMP_FL_ARGS(c));

  return 1;
}

static int component_register_content(lua_State* L) {
  lua_newtable(L);

  lua_pushcfunction(L, component_new);
  lua_setfield(L, -2, "new");

  return 1;
}

void lg_component_create(lua_State* L) {
  DEBUG_LOG("LG: COMPONENT CREATE");

  lua_getfield(L, LUA_REGISTRYINDEX, "ecs");
  ecs = lua_touserdata(L, -1);
  lua_pop(L, 1);

  comps_size = UINT8_MAX;
  comps_count = 0;
  comps = malloc(sizeof(*comps) * comps_size);

  component_init_metatable(L);

  component_register_content(L);
}
void lg_component_destroy(void) {
  DEBUG_LOG("LG: COMPONENT DESTROY");

  for (ecs_id_t i = 0; i < comps_count; i++) {
    free(comps[i].name);
  }

  free(comps);
  comps_count = comps_size = 0;
}

ecs_ret_t lg_component_debug_system(ecs_t* ecs, ecs_entity_t* entities,
                                    size_t entity_count, void* udata) {
  DEBUG_LOG("LG: DEBUG SYSTEM RUNNING");

  for (size_t i = 0; i < entity_count; i++) {
    printf("LG: DEBUG SYSTEM, ENTITY %lu {\n", entities[i].id);

    for (ecs_id_t ci = 0; ci < comps_count; ci++) {
      lg_component* c = &comps[ci];

      if (ecs_has(ecs, ID2ENTI(entities[i].id), ID2COMP(c->id))) {
        void* v = ecs_get(ecs, ID2ENTI(entities[i].id), ID2COMP(c->id));

        printf("  " COMP_FL " = ", COMP_FL_ARGS(c));

        switch (comps[ci].type) {
          case COMP_INT:
            printf("%lld", *(lua_Integer*)v);
            break;
          case COMP_NUM:
            printf("%f", *(lua_Number*)v);
            break;
          case COMP_TAG:
            printf("%d", *(uint8_t*)v);
            break;
          case COMP_STR:
            printf("%s", ((lg_component_str*)v)->str);
            break;
        }

        putchar('\n');
      }
    }

    printf("}\n");
  }

  return 0;
}
