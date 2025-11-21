#include "tgg.h"

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdio.h>

#include "../functions.h"
#include "../luah/init.h"
#include "components.h"
#include "systems.h"
#include "tgg_lua.h"

ecs_t* tgg_init(void) {
  ecs_t* tgg_ecs = ecs_new(64, NULL);

  tgg_register_components(tgg_ecs);
  tgg_register_systems(tgg_ecs);

  return tgg_ecs;
}

int tgg_load_content(ecs_t* tgg_ecs, char* script_path) {
  // TODO: Maybe somehow try to make it sandbox safe
  // NOTE: luaopen_package should be called later
  lua_State* L = luaL_newstate();
  // Open some libs
  luaopen_base(L);
  luaopen_math(L);
  luaopen_string(L);
  luaopen_table(L);

  lua_newtable(L);
  lua_pushlightuserdata(L, tgg_ecs);
  luaL_setfuncs(L, luagg_tgg_module, 1);
  lua_setglobal(L, "tgg");

  luaopen_package(L);
  // Load internal scripts in case if script_path doesn't exists
  if (file_exists(script_path)) {
    printf("Loading content using internal scripts\n");

    if (luaL_dostring(L, init_lua) != LUA_OK) {
      printf("Error at internal scripts: %s\n", lua_tostring(L, -1));

      return 1;
    }
  } else {
    printf("Loading content using user scripts\n");

    if (luaL_dofile(L, script_path) != LUA_OK) {
      printf("Error at user scripts: %s\n", lua_tostring(L, -1));

      return 1;
    }
  }

  lua_close(L);

  return 0;
}

void tgg_free(ecs_t* tggecs) { ecs_free(tggecs); }
