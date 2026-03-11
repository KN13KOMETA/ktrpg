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
#include "lua_sandbox.h"
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
int init_game_universal(char* script_path, vfile* modules);

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
    return init_game_universal(loptions.script_path, (vfile[]){{NULL, NULL}});
  else
    return init_game_universal(NULL, vfscripts);

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

static luaL_Reg glibs[] = {{LUA_GNAME, lsbopen_base},
                           {LUA_TABLIBNAME, luaopen_table},
                           {LUA_STRLIBNAME, luaopen_string},
                           {LUA_MATHLIBNAME, luaopen_math},
                           {"ktrpg", lg_openlib},
                           {NULL, NULL}};

int init_game_universal(char* script_path, vfile* modules) {
  int code = EXIT_SUCCESS;
  char* basedir = NULL;
  lua_State* L;

  if (script_path != NULL) {
    if (user_script_warning()) return EXIT_SUCCESS;

    printf(TITLE("GAME (user scripts)"));
    FMALLOC(basedir, strlen(script_path) + 1);

    if (get_basedir(script_path, basedir) != 0) {
      strcpy(basedir, ".");
    }
  } else {
    if (modules[0].path == NULL) {
      printf("Fail to load embedded game, no scripts found\n");
      return EXIT_FAILURE;
    }

    printf(TITLE("GAME (embedded scripts)"));
  }

  // Create Lua state
  L = luaL_newstate();
  // Create sandbox
  lsb_create(L, modules, basedir);
  // Create luaglue
  lg_create(L);

  // Create global libs/modules
  for (luaL_Reg* glib = glibs; glib->name != NULL; glib++)
    lsb_krequiref(L, glib->name, glib->func, 1);

  if ((basedir == NULL) ? luaL_dostring(L, modules[0].content)
                        : luaL_dofile(L, script_path) != LUA_OK) {
    const char* err = lua_tostring(L, -1);

    if (strcmp(LG_EXIT_USER, err) != 0 && strcmp(LG_EXIT_SYSTEM, err) != 0) {
      printf("Lua error: %s\n", err);
      code = EXIT_FAILURE;
    }
  }

  lsb_destroy();
  lg_destroy();
  lua_close(L);
  if (basedir != NULL) free(basedir);

  return code;
}
