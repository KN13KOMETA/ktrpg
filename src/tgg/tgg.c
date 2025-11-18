#include "tgg.h"

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../functions.h"
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

int tgg_load_content(ecs_t* tgg_ecs, char* script_path) {
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
  // luaopen_bit(L);
  // luaopen_jit(L);
  // luaopen_ffi(L);
  // luaopen_string_buffer(L);

  lua_pushlightuserdata(L, tgg_ecs);

  lua_pushcclosure(L, tgg_register_character, 1);
  lua_setglobal(L, "tgg_register_character");

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

// Registers
int tgg_register_character(lua_State* L) {
  ecs_t* tgg_ecs = lua_touserdata(L, lua_upvalueindex(1));
  ecs_entity_t character = ecs_create(tgg_ecs);

  // If arg is not table return an error
  if (!lua_istable(L, -1)) luaL_typeerror(L, -1, "table");

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
    lua_getfield(L, -2, "min_health");
    lua_Number field = lua_tonumber(L, -1);

    if (!lua_isnil(L, -1)) {
      tgg_character_min_health_comp* min_health =
          ecs_add(tgg_ecs, character, TGG_CHARACTER_MIN_HEALTH_COMP, NULL);

      *min_health = field;
    }
  }

  {
    lua_getfield(L, -3, "max_health");
    lua_Number field = lua_tonumber(L, -1);

    if (!lua_isnil(L, -1)) {
      tgg_character_max_health_comp* max_health =
          ecs_add(tgg_ecs, character, TGG_CHARACTER_MAX_HEALTH_COMP, NULL);

      *max_health = field;
    }
  }

  // We don't return anything
  return 0;
}
