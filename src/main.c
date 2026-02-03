#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <pico_ecs.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "functions.h"
#include "launch_options.h"
#include "luaglue/core.h"
#include "luah/init.h"
#include "luah/module.h"
#include "luah/types/ktrpg/class/component_d.h"
#include "luah/types/ktrpg/class/entity_d.h"
#include "luah/types/ktrpg/class/system_d.h"
#include "luah/types/ktrpg/ktrpg_d.h"

#define PICO_ECS_IMPLEMENTATION
#include <pico_ecs.h>

void print_debug_mode(void);
void user_script_warning(void);
void write_vfiles_to_dir(vfile* vfiles, char* dir);

int main(int argc, char* argv[]) {
  vfile vfscripts[] = {
      {"init.lua", init_lua}, {"module.lua", module_lua}, {NULL, NULL}};
  vfile vftypes[] = {
      {"types/ktrpg/class/entity.d.lua", types_ktrpg_class_entity_d_lua},
      {"types/ktrpg/class/component.d.lua", types_ktrpg_class_component_d_lua},
      {"types/ktrpg/class/system.d.lua", types_ktrpg_class_system_d_lua},
      {"types/ktrpg/ktrpg.d.lua", types_ktrpg_ktrpg_d_lua},
      {NULL, NULL}};
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

  if (loptions.scripts_export_path != NULL)
    write_vfiles_to_dir(vfscripts, loptions.scripts_export_path);

  if (loptions.types_export_path != NULL)
    write_vfiles_to_dir(vftypes, loptions.types_export_path);

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

void write_vfiles_to_dir(vfile* vfiles, char* dir) {
  size_t dir_len = strlen(dir);

  for (int i = 0; vfiles[i].path != NULL; i++) {
    vfile f = vfiles[i];
    char filepath[dir_len + 1 + strlen(f.path) + 1];
    char basedir[dir_len + 1 + strlen(f.path) + 1];
    FILE* file;

    sprintf(filepath, "%s/%s", dir, f.path);

    get_basedir(filepath, basedir);

    create_dir_recursive(basedir);

    file = fopen(filepath, "w");

    fwrite(f.content, strlen(f.content), 1, file);

    fclose(file);
  }
}
