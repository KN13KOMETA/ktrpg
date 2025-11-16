#include "tgg.h"

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../luah/init.h"
#include "components.h"
#include "systems.h"

int tgg_register_character(lua_State* L);

ecs_t* tgg_init(void) {
  ecs_t* tgg_ecs = ecs_new(64, NULL);

  tgg_register_components(tgg_ecs);
  tgg_register_systems(tgg_ecs);

  return tgg_ecs;
}

int tgg_load_content(ecs_t* tgg_ecs, bool force_internal) {
  // TODO: Maybe somehow try to make it sandbox safe
  // NOTE: luaopen_package should be called later
  lua_State* L = luaL_newstate();
  // Open some libs
  luaopen_base(L);
  luaopen_math(L);
  luaopen_string(L);
  luaopen_table(L);
  // luaopen_io(L);
  // luaopen_os(L);
  // luaopen_package(L);
  // luaopen_debug(L);
  luaopen_bit(L);
  // luaopen_jit(L);
  // luaopen_ffi(L);
  // luaopen_string_buffer(L);

  {
    ecs_entity_t player = ecs_create(tgg_ecs);

    tgg_character_name_comp* player_name =
        ecs_add(tgg_ecs, player, TGG_CHARACTER_NAME_COMP, NULL);

    *player_name = "Not";
  }

  lua_pushlightuserdata(L, tgg_ecs);

  lua_pushcclosure(L, tgg_register_character, 1);
  // lua_pushcfunction(L, tgg_register_character);
  lua_setglobal(L, "tgg_register_character");

  // Load internal scripts in case if forced internal or init.lua does'n exists
  if (force_internal || access("init.lua", F_OK) != 0) {
    printf("Loading content using internal scripts\n");

    if (luaL_dostring(L, init_lua) != LUA_OK) {
      printf("Error at internal scripts: %s\n", lua_tostring(L, -1));

      return 1;
    }
  } else {
    // TODO: user scripts load
  }

  lua_close(L);

  return 0;
}

void tgg_free(ecs_t* tggecs) { ecs_free(tggecs); }

// Registers
int tgg_register_character(lua_State* L) {
  ecs_t* tgg_ecs = lua_touserdata(L, lua_upvalueindex(1));
  ecs_entity_t character = ecs_create(tgg_ecs);

  // If arg is not table return an error
  if (!lua_istable(L, -1)) luaL_typerror(L, -1, "table");

  {
    lua_getfield(L, -1, "name");
    const char* field = lua_tostring(L, -1);

    if (field != NULL) {
      tgg_character_name_comp* name =
          ecs_add(tgg_ecs, character, TGG_CHARACTER_NAME_COMP, NULL);

      *name = malloc(strlen(field));

      strcpy(*name, field);
    }
  }

  {
    lua_getfield(L, -2, "health");
    lua_Number health = lua_tonumber(L, -1);

    printf("health: %f\n", health);
  }

  // We don't return anything
  return 0;
}
