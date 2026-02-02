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
#include "launch_options.h"
#include "luaglue/core.h"
#include "luah/init.h"
#include "luah/module.h"

#define PICO_ECS_IMPLEMENTATION
#include <pico_ecs.h>

void print_debug_mode(void);
void user_script_warning(void);

int main(int argc, char* argv[]) {
  launch_options loptions = {0, 0, NULL, NULL, NULL};
  int code = process_launch_options(&loptions, argc, argv);

  switch (code) {
    case LO_EXIT_ACTION:
      return EXIT_SUCCESS;
    case LO_EXIT_ERROR:
      return EXIT_FAILURE;
    case EXIT_SUCCESS:
      break;
    default:
      printf("Unknown exit code (%d)\n", code);
      return EXIT_FAILURE;
  }

  print_debug_mode();

  printf(TITLE("HELP"));
  printf("Run with --help for more info\n");

  {
    lua_State* L = luaL_newstate();
    ecs_t* ecs = ecs_new(128, NULL);

    luaL_openlibs(L);

    lg_create(L, ecs);

    // Load lua scripts or use internal ones
    if (loptions.script_path != NULL) {
      char* target_path;
      char* module_path;

      user_script_warning();

      printf(TITLE("GAME (user scripts)"));

      // TODO: Test it on windows

      // Init target_path
      {
        char* last_slash = strrchr(loptions.script_path, '/');
        unsigned long length =
            (unsigned long)(last_slash - loptions.script_path);

        FMALLOC(target_path, length + 1);

        strncpy(target_path, loptions.script_path, length);

        target_path[length] = '\0';
      }

      module_path = build_lua_package_search_path(target_path);

      add_lua_package_path(L, module_path);

      free(target_path);
      free(module_path);

      if (luaL_dofile(L, loptions.script_path) != LUA_OK)
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

  return EXIT_SUCCESS;
}

void print_debug_mode(void) {
#ifdef DEBUG

  printf(TITLE("DEBUG MODE"));
  printf("Be Quiet, Wanderer\n");

#endif  // DEBUG
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
