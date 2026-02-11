#include "component.h"

#include <lauxlib.h>
#include <lua.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../constants.h"
#include "../../functions.h"
#include "../core.h"
#include "entity.h"

static ecs_id_t array_limit;

static ptr2ecs* ecs;
static lg_component* comps;
static ecs_id_t comps_count = 0;
static ecs_id_t comps_max = 0;

static int comp_estimated_size =
    COMP_ESTIMATED_SIZE + sizeof(*comps) + sizeof(uint8_t);

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
  (void)ecs;
  (void)entity;

  free(((lg_component_str*)comp_ptr)->str);
}

static int component_new(lua_State* L) {
  const char* ctype;
  const char* cname;
  ptr2ptr* ud;
  lg_component* c;

  if (ecs->ptr == NULL) return luaL_error(L, ECS_IS_NULL);
  ecs->modified = 1;

  if (comps_count + 1 > comps_max) {
    lua_pushnil(L);
    lua_pushstring(L, "component limit reached");
    return 2;
  }

  ctype = luaL_checkstring(L, 2);
  cname = luaL_checkstring(L, 3);

  ud = lua_newuserdatauv(L, sizeof(*ud), 0);

  c = &comps[comps_count++];
  ud->ptr = c;

  if (strlen(ctype) != 3) goto lua_err;

  if (strncmp("int", ctype, 3) == 0) {
    c->type = COMP_INT;
    c->id = ecs_define_component(ecs->ptr, sizeof(lua_Integer), NULL, NULL).id;
  } else if (strncmp("num", ctype, 3) == 0) {
    c->type = COMP_NUM;
    c->id = ecs_define_component(ecs->ptr, sizeof(lua_Number), NULL, NULL).id;
  } else if (strncmp("tag", ctype, 3) == 0) {
    c->type = COMP_TAG;
    c->id = ecs_define_component(ecs->ptr, sizeof(uint8_t), NULL, NULL).id;
  } else if (strncmp("str", ctype, 3) == 0) {
    c->type = COMP_STR;
    c->id = ecs_define_component(ecs->ptr, sizeof(lg_component_str), NULL,
                                 component_str_destructor)
                .id;
  } else {
  lua_err:
    return luaL_argerror(L, 2, "expected \"int\", \"num\", \"tag\" or \"str\"");
  }

  FMALLOC(c->name, strlen(cname) + 1);
  strcpy(c->name, cname);

  luaL_getmetatable(L, "ClassComponentMT");
  lua_setmetatable(L, -2);

  DEBUG_LOG("LG: REGISTRED " COMP_FL, COMP_FL_ARGS(c));

  return 1;
}

static int component_set_limit(lua_State* L) {
  ecs_id_t limit = (ecs_id_t)luaL_checkinteger(L, 2);
  void* ptr;

  if (limit < 1) {
    lua_pushnil(L);
    lua_pushstring(L, "limit must be at least 1");
    return 2;
  }

  if (limit > array_limit) {
    char str[256];

    sprintf(str, "limit cannot exceed %lu", limit);

    lua_pushnil(L);
    lua_pushstring(L, str);

    return 2;
  }

  if (comps_count != 0) {
    lua_pushnil(L);
    lua_pushstring(L, "cannot set limit after entities have been created");
    return 2;
  }

  if (limit != comps_count) {
    ptr = malloc(sizeof(*comps) * limit);

    if (ptr == NULL) {
      lua_pushnil(L);
      lua_pushstring(L, "memory allocation failed");
      return 2;
    }

    free(comps);
    comps = ptr;
    comps_max = limit;

    DEBUG_LOG("LG: COMPONENT ARRAY SIZE = %lu", comps_max);
  }

  lua_pushinteger(L, (lua_Integer)limit);
  return 1;
}

static int component_get_limit(lua_State* L) {
  lua_pushinteger(L, (lua_Integer)comps_max);
  return 1;
}

static luaL_Reg component_class_methods[] = {{"new", component_new},
                                             {"set_limit", component_set_limit},
                                             {"get_limit", component_get_limit},
                                             {NULL, NULL}};

static int component_register_content(lua_State* L) {
  lua_newtable(L);

  luaL_setfuncs(L, component_class_methods, 0);

  return 1;
}

void lg_component_create(lua_State* L) {
  DEBUG_LOG("LG: COMPONENT CREATE");

  DEBUG_LOG("LG: COMPONENT ESTIMATED SIZE = %d", comp_estimated_size);

  lua_getfield(L, LUA_REGISTRYINDEX, "ecs");
  ecs = lua_touserdata(L, -1);
  lua_pop(L, 1);

  array_limit = LUA_MAXINTEGER / sizeof(*comps);

  DEBUG_LOG("LG: COMPONENT ARRAY LIMIT %lu", array_limit);

  comps_max = COMP_DEFAULT_MAX;
  comps_count = 0;
  SELFMALLOCARR(comps, comps_max);

  DEBUG_LOG("LG: COMPONENT ARRAY SIZE = %lu", comps_max);

  component_init_metatable(L);

  component_register_content(L);
}
void lg_component_destroy(void) {
  DEBUG_LOG("LG: COMPONENT DESTROY");

  for (ecs_id_t i = 0; i < comps_count; i++) {
    free(comps[i].name);
  }

  free(comps);
  comps_count = comps_max = 0;
}

ecs_ret_t lg_component_debug_system(ecs_t* ecs, ecs_entity_t* entities,
                                    size_t entity_count, void* udata) {
  (void)udata;

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
