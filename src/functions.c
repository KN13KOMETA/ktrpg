#include "functions.h"

#include <lauxlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32

#include <fileapi.h>
#include <sys/stat.h>
#include <windows.h>

#define stat _stat

#else

#include <dirent.h>
#include <sys/stat.h>

#endif  // ifdef _WIN32

#ifdef _WIN32
#else
#endif  // ifdef _WIN32

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

int directory_exists(char* path) {
  struct stat path_stat;

  stat(path, &path_stat);

#ifdef _WIN32
  return (path_stat.st_mode & _S_IFDIR) == 0;
#else
  return S_ISDIR(path_stat.st_mode) == 0;
#endif  // ifdef _WIN32
}

int directory_empty(char* path) {
#ifdef _WIN32
  WIN32_FIND_DATA find_data;
  HANDLE h = NULL;

  char* template = "%s\\*";
  char search_mask[strlen(template) + strlen(path) + 1];

  sprintf(search_mask, template, path);

  if ((h = FindFirstFile(search_mask, &find_data)) == INVALID_HANDLE_VALUE)
    return -1;

  // If we found file other than "." and ".." means directory not empty
  do {
    // TODO: Maybe try count files like unix implementation
    if (strcmp(find_data.cFileName, ".") != 0 &&
        strcmp(find_data.cFileName, "..") != 0) {
      FindClose(h);
      return 1;
    }
  } while (FindNextFile(h, &find_data));

  FindClose(h);
  return 0;
#else
  DIR* dir = opendir(path);
  int i = 0;

  if (dir == NULL) return -1;

  // If more than 2 files then directory is not empty
  while (readdir(dir) != NULL)
    if (++i > 2) {
      closedir(dir);
      return 1;
    }

  closedir(dir);
  return 0;
#endif  // ifdef _WIN32
}

int get_basedir(char* path, char* basedir) {
  char* last_slash;

  strcpy(basedir, path);

  last_slash = strrchr(basedir, '/');

  if (last_slash == NULL) return 1;

  *last_slash = '\0';

  return 0;
}

void create_dir_recursive(char* path) {
  char basedir[strlen(path)];

  if (directory_exists(path) == 0) return;

  get_basedir(path, basedir);

  if (directory_exists(basedir) != 0) create_dir_recursive(basedir);

  mkdir(path, 0777);
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
