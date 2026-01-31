#include "functions.h"

#include <lauxlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int getchar_clear(char* ch) {
  int c = 0;

  *ch = (char)getchar();

  if (*ch != '\n')
    while ((c = getchar()) != '\n' && c != EOF);

  return c;
}

int getchars_clear(char* str, size_t len) {
  size_t i = 0;
  int c = 0;

  while ((c = getchar()) != '\n' && c != EOF) {
    if (i != len - 1) {
      str[i++] = (char)c;
    }
  }

  for (; i < len; i++) str[i] = '\0';

  return c;
}

int file_exists(char* path) {
  FILE* f = fopen(path, "r");

  if (f == NULL) return 1;

  fclose(f);

  return 0;
}

void register_lua_text_module(lua_State* L, const char* name,
                              const char* content) {
  lua_getglobal(L, "package");
  lua_getfield(L, -1, "preload");

  if (luaL_loadbuffer(L, content, strlen(content), name) == LUA_OK) {
    lua_setfield(L, -2, name);
  } else {
    lua_pop(L, 1);
  }

  lua_pop(L, 2);
}

char* build_lua_package_search_path(const char* path) {
  char* p1 = "/?.lua;";
  char* p2 = "/?/init.lua";
  unsigned long len = strlen(path);
  char* np;

  FMALLOC(np, len + strlen(p1) + len + strlen(p2) + 1);

  if (np != NULL) {
    sprintf(np, "%s%s%s%s", path, p1, path, p2);
  }

  return np;
}

void add_lua_package_path(lua_State* L, const char* path) {
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
