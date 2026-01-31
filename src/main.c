#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <pico_ecs.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "constants.h"
#include "functions.h"
#include "luaglue/core.h"
#include "luah/init.h"
#include "luah/module.h"
#include "project_options.h"

#define PICO_ECS_IMPLEMENTATION
#include <pico_ecs.h>

void user_script_warning(void);

int main(int argc, char* argv[]) {
  const char** temp_argv = malloc(((size_t)argc + 1) * sizeof(*temp_argv));
  project_options* poptions;

  for (int i = 0; i < argc; i++) temp_argv[i] = argv[i];
  temp_argv[argc] = NULL;

  poptions = prompt_project_options(argc, temp_argv);

  DEBUG_LOG(TITLE("DEBUG MODE"));

  printf(TITLE("HELP"));
  printf("Run with --help for more info\n");

  {
    lua_State* L = luaL_newstate();
    ecs_t* ecs = ecs_new(128, NULL);

    luaL_openlibs(L);

    lg_create(L, ecs);

    // Load lua scripts or use internal ones
    if (poptions->script_path != NULL) {
      user_script_warning();

      printf(TITLE("GAME (user scripts)"));

      // TODO: Test it on windows
      {
        char* module_path;
        char* target_path;

        // Init target_path
        {
          char* last_slash = strrchr(poptions->script_path, '/');
          unsigned long length =
              (unsigned long)(last_slash - poptions->script_path);

          target_path = malloc(length + 1);

          strncpy(target_path, poptions->script_path, length);

          target_path[length] = '\0';
        }

        module_path = build_lua_package_search_path(target_path);

        add_lua_package_path(L, module_path);

        free(target_path);
        free(module_path);
      }

      if (luaL_dofile(L, poptions->script_path) != LUA_OK)
        printf("Error at internal user scripts: %s\n", lua_tostring(L, -1));
    } else {
      printf(TITLE("GAME (internal scripts)"));

      register_lua_text_module(L, "module", module_lua);

      if (luaL_dostring(L, init_lua) != LUA_OK)
        printf("Error at internal scripts: %s\n", lua_tostring(L, -1));
    }

    lg_destroy();
    ecs_free(ecs);
    lua_close(L);
  }

  free_project_options(poptions);
  free(temp_argv);

  return EXIT_SUCCESS;
}

void user_script_warning(void) {
  char c = '\0';

  printf(TITLE("SECURITY WARNING"));
  printf("You are about to run user-written Lua scripts\n");
  printf("They are NOT sandboxed and have full access to your SYSTEM\n");
  printf("Are you sure you want to continue? (y/n): ");

  getchar_clear(&c);

  switch (c) {
    case 'y':
    case 'Y':
      break;
    default:
      exit(EXIT_SUCCESS);
  }
}
