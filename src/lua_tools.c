#include "lua_tools.h"

#include <lauxlib.h>
#include <string.h>

#include "functions.h"

char* luat_build_package_search_path(const char* path) {
  char* p1 = "/?.lua;";
  char* p2 = "/?/init.lua";
  size_t len = strlen(path);
  char* np;

  FMALLOC(np, len + strlen(p1) + len + strlen(p2) + 1);

  if (np != NULL) {
    sprintf(np, "%s%s%s%s", path, p1, path, p2);
  }

  return np;
}

void luat_add_package_path(lua_State* L, const char* path) {
  const char* current_path;
  char* package_path;
  int i = 0;

  lua_getglobal(L, "package");
  lua_getfield(L, -1, "path");

  current_path = lua_tostring(L, -1);

  FMALLOC(package_path, strlen(current_path) + 1 + strlen(path) + 1)

  strcpy(package_path, current_path);
  i += strlen(current_path);

  package_path[i] = ';';
  i++;

  strcpy(package_path + i, path);
  i += strlen(path);

  package_path[i] = '\0';

  lua_pop(L, 1);
  lua_pushstring(L, package_path);
  lua_setfield(L, -2, "path");
  lua_pop(L, 1);

  free(package_path);
}

void luat_requiref_text(lua_State* L, const char* name, const char* content) {
  lua_getglobal(L, "package");
  lua_getfield(L, -1, "preload");

  if (luaL_loadbuffer(L, content, strlen(content), name) == LUA_OK) {
    lua_setfield(L, -2, name);
  } else {
    lua_pop(L, 1);
  }

  lua_pop(L, 2);
}
