#include "functions.h"

#include <lauxlib.h>
#include <stdio.h>
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
