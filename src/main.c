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
      {
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
            return 0;
        }
      }

      printf(TITLE("GAME (user scripts)"));

      // TODO: Test it on windows
      {
        char* module_path;
        char* path_type1 = "?.lua";
        char* path_type2 = "?/init.lua";
        char* target_path;
        size_t target_path_length;
        int i = 0;

        // Init target_path
        {
          // TODO: Add check incase path doesnt contain /
          char* last_slash = strrchr(poptions->script_path, '/');
          unsigned long length =
              (unsigned long)(last_slash - poptions->script_path + 1);

          target_path = malloc(length + 1);

          strncpy(target_path, poptions->script_path, length);

          target_path[length] = '\0';
        }

        target_path_length = strlen(target_path);

        module_path = malloc(target_path_length + strlen(path_type1) + 1 +
                             target_path_length + strlen(path_type2) + 1);

        strcpy(module_path, target_path);
        i += target_path_length;

        strcpy(module_path + i, path_type1);
        i += strlen(path_type1);

        module_path[i] = ';';
        i++;

        strcpy(module_path + i, target_path);
        i += target_path_length;

        strcpy(module_path + i, path_type2);

        // Set new package.path
        {
          char* package_path;
          const char* current_path;
          int i = 0;

          lua_getglobal(L, "package");
          lua_getfield(L, -1, "path");

          current_path = lua_tostring(L, -1);

          package_path =
              malloc(strlen(current_path) + 1 + strlen(module_path) + 1);

          strcpy(package_path, current_path);
          i += strlen(current_path);

          package_path[i] = ';';
          i++;

          strcpy(package_path + i, module_path);

          lua_pop(L, 1);
          lua_pushstring(L, package_path);
          lua_setfield(L, -2, "path");
          lua_pop(L, 1);

          free(target_path);
          free(module_path);
          free(package_path);
        }
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

    // return EXIT_SUCCESS;
  }

  // luab_init(poptions);

  free_project_options(poptions);
  free(temp_argv);

  return EXIT_SUCCESS;
}
