#include "core.h"

#include <lauxlib.h>
#include <lua.h>

#include "../functions.h"
#include "../lua_sandbox.h"
#include "class/component.h"
#include "class/entity.h"
#include "class/system.h"
#include "class/util.h"

static ptr2ecs ecs = {NULL, 0};

static int openclib(lua_State* L) {
  DEBUG_LOG("LG: OPENING C LIBRARY");

  lua_newtable(L);

  lg_entity_create(L);
  lua_setfield(L, -2, "Entity");
  lg_component_create(L);
  lua_setfield(L, -2, "Component");
  lg_system_create(L);
  lua_setfield(L, -2, "System");
  lg_util_create(L);
  lua_setfield(L, -2, "Util");

  return 1;
}

void lg_create(lua_State* L) {
  DEBUG_LOG("LG: CREATE");

  lua_pushlightuserdata(L, &ecs);
  lua_setfield(L, LUA_REGISTRYINDEX, "ecs");

  lsb_krequiref(L, "ktrpg", openclib, 0);
}
void lg_destroy(void) {
  DEBUG_LOG("LG: DESTROY");

  lg_entity_destroy();
  lg_component_destroy();
  lg_system_destroy();
  lg_util_destroy();

  if (ecs.ptr != NULL) ecs_free(ecs.ptr);
}
