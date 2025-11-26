#include "tgg_lua_module.h"

#include <stdlib.h>
#include <string.h>

#include "components.h"
#include "lauxlib.h"

int luagg_tgg_module_register_character(lua_State* L);

struct luaL_Reg tgg_lua_module[] = {
    {"register_character", luagg_tgg_module_register_character}, {NULL, NULL}};

int luagg_tgg_module_register_character(lua_State* L) {
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

      *name = malloc(strlen(field) + 1);

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

  // NOTE: we are converting big type to a lower one
  // This will probably be fine since tgg doesnt need absurd amount of entities
  lua_pushinteger(L, (lua_Integer)character.id);
  return 1;
}
