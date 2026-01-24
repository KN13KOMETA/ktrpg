#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <pico_ecs.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "constants.h"
#include "functions.h"
#include "luab/luab_core.h"
#include "luaglue/class/component.h"
#include "luaglue/core.h"
#include "project_options.h"

#define PICO_ECS_IMPLEMENTATION
#include <pico_ecs.h>

int main(int argc, char* argv[]) {
  const char** temp_argv = malloc(((size_t)argc + 1) * sizeof(*temp_argv));
  project_options* poptions;

  for (int i = 0; i < argc; i++) temp_argv[i] = argv[i];
  temp_argv[argc] = NULL;

  poptions = prompt_project_options(argc, temp_argv);

  DEBUG_LOG(TITLE("DEBUG MODE"));

  printf(TITLE("HELP"));
  printf("Run with --help for more info\n\n");

  {
    lua_State* L = luaL_newstate();
    ecs_t* ecs = ecs_new(128, NULL);

    luaL_openlibs(L);

    lg_create(L, ecs);

    lg_destroy();
    ecs_free(ecs);
    lua_close(L);

    return EXIT_SUCCESS;
  }

  luab_init(poptions);

  free_project_options(poptions);
  free(temp_argv);

  return EXIT_SUCCESS;
}
