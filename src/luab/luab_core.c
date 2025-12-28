#include "luab_core.h"

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <pico_ecs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../functions.h"
#include "../luah/init.h"
#include "luab_m.h"

#define LUAB_MAX_COMP_COUNT 64
#define LUAB_MAX_SYSTEM_COUNT 64
#define LUAB_MAX_ENTITY_COUNT 64

luab_state luab_init(void) {
  luab_state lb;
  lua_State* L = luaL_newstate();

  lb.L = L;
  lb.comps = malloc(sizeof(ecs_comp_t) * LUAB_MAX_COMP_COUNT);
  lb.comp_types = malloc(sizeof(COMP_TYPE) * LUAB_MAX_COMP_COUNT);
  lb.systems = malloc(sizeof(ecs_system_t) * LUAB_MAX_SYSTEM_COUNT);
  lb.system_lua_refs = malloc(sizeof(int) * LUAB_MAX_SYSTEM_COUNT);
  lb.entities = malloc(sizeof(ecs_entity_t) * LUAB_MAX_ENTITY_COUNT);

  luaL_openlibs(L);

  // Register luab module
  // NOTE: For 5.1 use luaL_register
  lua_newtable(L);
  lua_pushlightuserdata(L, &lb);
  luaL_setfuncs(L, luab_m_l, 1);
  lua_setglobal(L, "game");

  // NOTE: Clean this part after testing
  {
    // NOTE: Register systems after components, but before entities
    ecs_t* ecs = ecs_new(64, NULL);

    lb.ecs = ecs;

    DEBUG_LOG("Running lua scripts");
    if (luaL_dostring(L, init_lua) != LUA_OK) {
      printf("Error at internal scripts: %s\n", lua_tostring(L, -1));
    }

    DEBUG_LOG("Comps registered %d", lb.comp_count);

    ecs_define_system(lb.ecs, 0, luab_debug_system, NULL, NULL, &lb);

    ecs_entity_t entity_a = ecs_create(ecs);
    ecs_entity_t entity_b = ecs_create(ecs);

    char** s = ecs_add(lb.ecs, entity_b, lb.comps[0], NULL);
    *s = "yo string";

    lua_Integer* d = ecs_add(lb.ecs, entity_a, lb.comps[1], NULL);
    *d = 67;

    lua_Number* f = ecs_add(lb.ecs, entity_b, lb.comps[2], NULL);
    *f = 6.9;

    printf("asd %lu\n", entity_b.id);

    for (int i = 0; i < 1; i++) {
      ecs_run_systems(ecs, 0);
    }

    ecs_free(ecs);
  }

  lua_close(L);
  return lb;
}

void luab_free(luab_state* lb) {
  free(lb->comps);
  free(lb->systems);
  free(lb->system_lua_refs);
}

ecs_ret_t luab_system_wrapper(ecs_t* ecs, ecs_entity_t* entities,
                              size_t entity_count, void* udata) {
  luab_system* lbs = udata;
  luab_state* lb = lbs->lb;

  (void)ecs;

  DEBUG_LOG("Running wrapper");

  {
    int lua_func_ref = lb->system_lua_refs[lbs->index];

    DEBUG_LOG("Wrapper running lua function with reference %d", lua_func_ref);

    lua_rawgeti(lb->L, LUA_REGISTRYINDEX, lua_func_ref);

    // Create lua table with entities id
    lua_createtable(lb->L, entity_count, 0);
    for (size_t ei = 0; ei < entity_count; ei++) {
      lua_pushinteger(lb->L, entities[ei].id);
      lua_rawseti(lb->L, -2, ei + 1);
    }

    lua_pushinteger(lb->L, entity_count);

    lua_pcall(lb->L, 2, 0, 1);
  }

  return 0;
}

ecs_ret_t luab_debug_system(ecs_t* ecs, ecs_entity_t* entities,
                            size_t entity_count, void* udata) {
  luab_state* lb = udata;

  (void)ecs;

  DEBUG_LOG("Running debug system");

  for (size_t i = 0; i < entity_count; i++) {
    ecs_entity_t entity = entities[i];
    printf("Entity %lu {\n", entity.id);

    for (ecs_id_t ci = 0; ci < lb->comp_count; ci++) {
      ecs_comp_t comp = lb->comps[ci];
      COMP_TYPE comp_type = lb->comp_types[ci];

      // TODO: Make output more pretty
      if (ecs_has(lb->ecs, entity, comp)) {
        printf("  ");
        printf("%lu ", ci);

        switch (comp_type) {
          case COMP_INTEGER:
            printf("%lld\n", *(lua_Integer*)ecs_get(ecs, entity, comp));
            break;
          case COMP_NUMBER:
            printf("%f\n", *(lua_Number*)ecs_get(ecs, entity, comp));
            break;
          case COMP_STRING:
            printf("%s\n", *(char**)ecs_get(ecs, entity, comp));
            break;
        }
      }
    }

    printf("}\n");
  }

  return 0;
}
