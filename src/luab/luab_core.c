#include "luab_core.h"

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <pico_ecs.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../constants.h"
#include "../functions.h"
#include "../luah/init.h"
#include "../luah/t.h"
#include "luab_m.h"

// TODO: Make arrays sized with lua script
#define LUAB_MAX_ENTITY_COUNT 64

luab_state luab_init(project_options* poptions) {
  luab_state lb;
  clock_t start_time = clock();

  printf(TITLE("LOADING CONTENT"));

  lb.L = luaL_newstate();
  lb.ecs = ecs_new(LUAB_MAX_ENTITY_COUNT, NULL);

  // lb.comps_size = MAX_COMP_COUNT;
  lb.comps_size = DEFAULT_COMP_COUNT;
  lb.comps = malloc(sizeof(ecs_comp_t) * lb.comps_size);
  lb.comp_types = malloc(sizeof(COMP_TYPE) * lb.comps_size);

  // lb.systems_size = MAX_SYSTEM_COUNT;
  lb.systems_size = DEFAULT_SYSTEM_COUNT;
  lb.systems = malloc(sizeof(ecs_system_t) * lb.systems_size);
  lb.system_lua_refs = malloc(sizeof(int) * lb.systems_size);

  luaL_openlibs(lb.L);

  // Register luab module
  // NOTE: For 5.1 use luaL_register
  lua_newtable(lb.L);
  lua_pushlightuserdata(lb.L, &lb);
  luaL_setfuncs(lb.L, luab_m_l, 1);
  lua_setglobal(lb.L, "game");

  // Register debug system before running lua scripts
  // NOTE: Register systems after components, but before entities
  ecs_define_system(lb.ecs, 0, luab_debug_system, NULL, NULL, &lb);

  // Load lua scripts or use internal ones
  if (poptions->script_path != NULL &&
      file_exists(poptions->script_path) == 0) {
    DEBUG_LOG("Running user lua scripts");
    if (luaL_dofile(lb.L, poptions->script_path) != LUA_OK)
      printf("Error at internal user scripts: %s\n", lua_tostring(lb.L, -1));
  } else {
    DEBUG_LOG("Running internal lua scripts");

    luab_register_text_module(lb.L, "t", t_lua);

    if (luaL_dostring(lb.L, init_lua) != LUA_OK)
      printf("Error at internal scripts: %s\n", lua_tostring(lb.L, -1));
  }

  putchar('\n');

  // TODO: fix message (it's called not after load but after lua script finish)
  printf(TITLE("CONTENT LOADED"));

  printf("Registered %d components\n", lb.comp_count);
  printf("Registered %d systems\n", lb.system_count);

  printf("Loaded in %f ms\n\n",
         (double)(clock() - start_time) / CLOCKS_PER_SEC * 1000);

  return lb;
}

void luab_free(luab_state* lb) {
  lua_close(lb->L);
  ecs_free(lb->ecs);

  free(lb->comps);
  free(lb->comp_types);

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

void luab_register_text_module(lua_State* L, char* name, char* content) {
  lua_getglobal(L, "package");
  lua_getfield(L, -1, "preload");

  if (luaL_loadbuffer(L, content, strlen(content), name) == LUA_OK) {
    lua_setfield(L, -2, name);
  } else {
    lua_pop(L, 1);
  }

  lua_pop(L, 2);
}
