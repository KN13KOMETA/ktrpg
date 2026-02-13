#ifndef LUA_TOOLS_H
#define LUA_TOOLS_H

#include <lua.h>

// Allocates memory
// Returns path/?.lua;path/?/init.lua
char* luat_build_package_search_path(const char* path);

// Adds path to lua package.path
void luat_add_package_path(lua_State* L, const char* path);

// Add a require with name and code as content
void luat_requiref_text(lua_State* L, const char* name, const char* content);

#endif  // !LUA_TOOLS_H
