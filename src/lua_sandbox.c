#include "lua_sandbox.h"

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "functions.h"

#define LSB_NOT_INIT "LSB not initialized call lsb_create first\n"

static int loaded = 0;
static char* wd;
static vfile* vlibs;

int lsbopen_base(lua_State* L) {
  char* allowed_base[] = {
      "assert", "error",  "getmetatable", "ipairs",   "next",
      "pairs",  "pcall",  "rawequal",     "rawlen",   "rawget",
      "rawset", "select", "setmetatable", "tonumber", "tostring",
      "type",   "xpcall", LUA_GNAME,      "_VERSION", NULL};

  luaopen_base(L);

  lua_pushglobaltable(L);

  for (char** p = allowed_base; *p != NULL; p++) {
    char* name = *p;

    lua_getfield(L, -2, name);
    if (lua_isnil(L, -1)) {
      lua_pushfstring(L, "%s not found in base", name);
      return lua_error(L);
    }
    lua_setfield(L, -2, name);
  }

  lua_remove(L, -2);

  return 1;
}

int lsb_krequiref(lua_State* L, const char* name, lua_CFunction openf,
                  int glb) {
  if (loaded == 0) {
    printf(LSB_NOT_INIT);
    return EXIT_FAILURE;
  }

  lua_pushcfunction(L, openf);
  if (lua_pcall(L, 0, 1, 0) != LUA_OK) return lua_error(L);

  lua_getfield(L, LUA_REGISTRYINDEX, LSB_REG);

  lua_pushvalue(L, -2);
  lua_setfield(L, -2, name);
  lua_pop(L, 1);

  if (glb) {
    lua_pushvalue(L, -1);
    lua_setglobal(L, name);
  }

  return EXIT_SUCCESS;
}

static int ll_krequire(lua_State* L) {
  const char* name = luaL_checkstring(L, 1);

  lua_getfield(L, LUA_REGISTRYINDEX, LSB_REG);

  // Returning cache if there's any
  lua_getfield(L, -1, name);
  if (!lua_isnil(L, -1)) {
    lua_remove(L, -2);
    return 1;
  }
  lua_pop(L, 1);

  {
    const char* vcontent = NULL;
    int status;

    // Searching module in vlibs first
    if (vlibs != NULL) {
      vfile* f = vlibs;
      char fname[strlen(name) + 4 + 1];

      sprintf(fname, "%s.lua", name);

      while (f->path != NULL) {
        if (strcmp(fname, f->path) == 0) {
          vcontent = f->content;
          break;
        }

        f++;
      }
    }

    if (vcontent != NULL) {
      status = luaL_loadbuffer(L, vcontent, strlen(vcontent), name);
    } else if (wd != NULL) {
      // Searching module in wd
      char fname[strlen(wd) + 1 + strlen(name) + 4 + 1];

      sprintf(fname, "%s/%s.lua", wd, name);

      status = luaL_loadfile(L, fname);
    } else {
      lua_pushfstring(L, "module '%s' not found", name);
      return lua_error(L);
    }

    if (status != LUA_OK) return lua_error(L);

    // Run module
    if (lua_pcall(L, 0, LUA_MULTRET, 0) != LUA_OK) return lua_error(L);

    // Handle return value
    {
      int nret = lua_gettop(L) - 2;
      if (nret == 0) {
        lua_pushboolean(L, 1);
        nret = 1;
      }

      if (nret > 1) {
        lua_pushvalue(L, 2);
        lua_replace(L, 2);
        lua_settop(L, 2);
      }
    }
  }

  // Save result in cache
  lua_pushvalue(L, -1);
  lua_setfield(L, -3, name);
  lua_remove(L, -2);

  return 1;
}

void lsb_create(lua_State* L, vfile* virtual_libs, char* workdir_path) {
  DEBUG_LOG("LSB: CREATE");

  loaded = 1;

  vlibs = virtual_libs;

  if (workdir_path != NULL) {
    FMALLOC(wd, strlen(workdir_path) + 1);
    strcpy(wd, workdir_path);
  } else {
    wd = NULL;
  }

  lua_newtable(L);
  lua_pushvalue(L, -1);
  lua_setfield(L, LUA_REGISTRYINDEX, LSB_REG);
  lua_pop(L, 1);

  lua_register(L, "krequire", ll_krequire);
}
void lsb_destroy(void) {
  DEBUG_LOG("LSB: DESTROY");

  loaded = 0;
  vlibs = NULL;
  if (wd != NULL) free(wd);
}
