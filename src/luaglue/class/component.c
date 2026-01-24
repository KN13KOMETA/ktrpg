#include "component.h"

#include <lauxlib.h>
#include <lua.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../../functions.h"

static ecs_t* ecs;
static lg_component* comps;
static ecs_id_t comps_count = 0;
static ecs_id_t comps_size = 0;

static int method_get_name(lua_State* L) {
  lg_component* c = luaL_checkudata(L, 1, "ClassComponentMT");

  lua_pushstring(L, c->name);

  return 1;
}

static struct luaL_Reg component_methods[] = {{"get_name", method_get_name},
                                              {NULL, NULL}};

static int component_gc(lua_State* L) {
  lg_component* c = luaL_checkudata(L, 1, "ClassComponentMT");

  free(c->name);

  return 0;
}

static void component_init_metatable(lua_State* L) {
  luaL_newmetatable(L, "ClassComponentMT");
  lua_pushvalue(L, -1);

  lua_setfield(L, -2, "__index");
  luaL_setfuncs(L, component_methods, 0);

  lua_pushcfunction(L, component_gc);
  lua_setfield(L, -2, "__gc");

  lua_pop(L, 1);
}

static void component_str_destructor(ecs_t* ecs, ecs_entity_t entity,
                                     void* comp_ptr) {
  free(((char*)comp_ptr));
}

static int component_new(lua_State* L) {
  const char* ctype = luaL_checkstring(L, 2);
  const char* cname = luaL_checkstring(L, 3);
  lg_component* c = lua_newuserdatauv(L, sizeof(*c), 0);

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
    c->id =
        ecs_define_component(ecs, sizeof(char*), NULL, component_str_destructor)
            .id;
  } else {
  lua_err:
    return luaL_argerror(L, 2, "expected \"int\", \"num\", \"tag\" or \"str\"");
  }

  c->name = malloc(strlen(cname) + 1);
  strcpy(c->name, cname);

  // TODO: Add realloc
  comps[comps_count++] = *c;

  luaL_getmetatable(L, "ClassComponentMT");
  lua_setmetatable(L, -2);

  return 1;
}

static int component_register_content(lua_State* L) {
  lua_newtable(L);
  lua_pushcfunction(L, component_new);
  lua_setfield(L, -2, "new");

  return 1;
}

void lg_component_create(lua_State* L) {
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
  free(comps);
  comps_count = comps_size = 0;
}
