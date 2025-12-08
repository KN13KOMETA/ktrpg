#include "ltest.h"

#include <pico_ecs.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>

#include "../luah/init.h"
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"

#define LT_ENTITY_COUNT 64
#define LT_COMP_COUNT 32

// TODO: Rewrite this mega duper super ultra ugly code

int m_register_integer_component(lua_State* L);
int m_register_number_component(lua_State* L);
int m_register_system(lua_State* L);

typedef struct {
  ecs_t* ecs;
  ecs_comp_t* comps;
  size_t comp_count;
} udata;

ecs_ret_t tgg_debug_system(ecs_t* ecs, ecs_entity_t* entities,
                           size_t entity_count, void* udata);

jmp_buf env;

int lpanic(lua_State* L) {
  char* err = lua_tostring(L, -1);

  printf("PANIC ERROR AT LUA: %s\n", err);

  longjmp(env, 1);

  return 0;
}

int lal(udata ud) {
  lua_State* L = luaL_newstate();

  if (L == NULL) return 1;

  if (setjmp(env) != 0) {
    printf("error happened while running lua\n");
    lua_close(L);
    return 1;
  }

  lua_atpanic(L, lpanic);

  luaL_openlibs(L);

  if (luaL_dostring(L, "return 69")) return 1;

  lua_close(L);

  return 0;
}

void ltest(void) {
  udata ud;

  printf("AMONGUS\n\n");

  ud.ecs = ecs_new(LT_ENTITY_COUNT, NULL);
  ud.comps = malloc(sizeof(ecs_comp_t) * LT_COMP_COUNT);
  ud.comp_count = 0;

  lal(ud);

  {
    // TODO: Maybe somehow try to make it sandbox safe
    // NOTE: luaopen_package should be called later
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    // Open some libs
    // luaopen_base(L);
    // luaopen_math(L);
    // luaopen_string(L);
    // luaopen_table(L);

    struct luaL_Reg lua_module[] = {
        {"register_integer_component", m_register_integer_component},
        {"register_number_component", m_register_number_component},
        {"register_system", m_register_system},
        {NULL, NULL}};

    lua_newtable(L);
    lua_pushlightuserdata(L, &ud);
    luaL_setfuncs(L, lua_module, 1);
    lua_setglobal(L, "lt");

    // luaopen_package(L);
    // TODO: custom scripts

    printf("Loading content using internal scripts\n");

    if (luaL_dostring(L, init_lua) != LUA_OK) {
      printf("Error at internal scripts: %s\n", lua_tostring(L, -1));

      return;
    }

    for (size_t i = 0; i < ud.comp_count; i++) {
      ecs_comp_t comp = ud.comps[i];
      printf("deb id: %lu\n", comp.id);
    }

    lua_close(L);
  }

  free(ud.comps);
}

// REGISTER COMPONENTS FROM LUA
lua_Integer register_integer_component(udata* ud) {
  ecs_comp_t comp = ecs_define_component(ud->ecs, 1, NULL, NULL);

  ud->comps[ud->comp_count] = comp;

  return (lua_Integer)ud->comp_count++;
}

int m_register_integer_component(lua_State* L) {
  udata* ud = lua_touserdata(L, lua_upvalueindex(1));
  lua_Integer comp_id = register_integer_component(ud);

  lua_pushinteger(L, comp_id);
  return 1;
}

lua_Number register_number_component(udata* ud) {
  ecs_comp_t comp = ecs_define_component(ud->ecs, 1, NULL, NULL);

  ud->comps[ud->comp_count] = comp;

  return (lua_Number)ud->comp_count++;
}

int m_register_number_component(lua_State* L) {
  udata* ud = lua_touserdata(L, lua_upvalueindex(1));
  lua_Number comp_id = register_number_component(ud);

  lua_pushnumber(L, comp_id);
  return 1;
}

void register_system(void) { printf("TODO: REGISTER SYSTEM"); }

int m_register_system(lua_State* L) {
  udata* ud = lua_touserdata(L, lua_upvalueindex(1));

  luaL_checktype(L, 1, LUA_TFUNCTION);

  lua_pcall(L, 0, 0, 1);

  return 0;
}
