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
int user_script_warning(void);
int init_game(char* script_path);
int init_embedded_game(vfile* scripts);

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

  // Handle process_launch_options exit code
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

  if (loptions.scripts_export_path != NULL) {
    if (write_vfiles_to_dir(vfscripts, loptions.scripts_export_path)) {
      printf("Exporting scripts failed\n");
      return EXIT_FAILURE;
    }
  }

  if (loptions.types_export_path != NULL) {
    if (write_vfiles_to_dir(vftypes, loptions.types_export_path)) {
      printf("Exporting types failed\n");
      return EXIT_FAILURE;
    }
  }

  if (loptions.script_path != NULL)
    return init_game(loptions.script_path);
  else
    return init_embedded_game(vfscripts);

  return EXIT_SUCCESS;
}

void print_debug_mode(void) {
#ifdef DEBUG

  printf(TITLE("DEBUG MODE"));
  printf("Be Quiet, Wanderer\n");

#endif  // DEBUG
}

int user_script_warning(void) {
  printf(TITLE("SECURITY WARNING"));
  printf("You are about to run user-written Lua scripts\n");
  printf("They are NOT sandboxed and have full access to your SYSTEM\n");

  return ask_yn("Are you sure you want to continue?");
}

int init_game(char* script_path) {
  int code = EXIT_SUCCESS;
  char basedir[strlen(script_path)];
  char* module_path;
  lua_State* L;

  if (user_script_warning()) return EXIT_SUCCESS;

  L = luaL_newstate();

  luaL_openlibs(L);
  lg_create(L);

  printf(TITLE("GAME (user scripts)"));

  // We only need to set up package.path
  // if file not in the same workdir
  if (get_basedir(script_path, basedir) == 0) {
    module_path = build_lua_package_search_path(basedir);
    add_lua_package_path(L, module_path);
    free(module_path);
  }

  if (luaL_dofile(L, script_path) != LUA_OK) {
    const char* err = lua_tostring(L, -1);

    if (strcmp(LG_EXIT_USER, err) != 0 && strcmp(LG_EXIT_SYSTEM, err) != 0) {
      printf("Lua error: %s\n", err);
      code = EXIT_FAILURE;
    }
  }

  lg_destroy();
  lua_close(L);

  return code;
}

int init_embedded_game(vfile* scripts) {
  int code = EXIT_SUCCESS;
  int script_count = 0;
  lua_State* L;

  for (int i = 0; scripts[i].path != NULL; i++) script_count++;

  printf(TITLE("GAME (embedded scripts)"));

  if (script_count == 0) {
    printf("Fail to load embedded game, expected at least 1 scripts\n");
    return EXIT_FAILURE;
  }

  L = luaL_newstate();

  luaL_openlibs(L);
  lg_create(L);

  for (int i = 1; i < script_count; i++) {
    vfile f = scripts[i];
    size_t path_len = strlen(f.path);
    char* ext = strrchr(f.path, '.');
    char module_name[path_len - 4 + 1];

    if (ext == NULL || strcmp(".lua", ext) != 0) {
      printf("Fail to load embedded game, unknown file \"%s\"\n", f.path);
      code = EXIT_FAILURE;
    }

    strncpy(module_name, f.path, path_len - 4);
    module_name[path_len - 4] = '\0';

    register_lua_text_module(L, module_name, f.content);
  }

  if (code == EXIT_SUCCESS && luaL_dostring(L, scripts[0].content) != LUA_OK) {
    const char* err = lua_tostring(L, -1);

    if (strcmp(LG_EXIT_USER, err) != 0 && strcmp(LG_EXIT_SYSTEM, err) != 0) {
      printf("Lua error: %s\n", err);
      code = EXIT_FAILURE;
    }
  }

  lg_destroy();
  lua_close(L);

  return code;
}

// TODO: update readme
// write about used libraries
// write about scripting
// update spoiler alert
