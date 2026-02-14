#ifndef LUA_SANDBOX_H
#define LUA_SANDBOX_H

#include <lua.h>

#include "constants.h"

#define LSB_REG "krequire_loaded"

// Creates lua sandbox
// workdir_path == NULL then only virtual_libs will be used
// virtual_libs == NULL then only workdir_path will be used
// If both NULL, well no libs will be found
void lsb_create(lua_State* L, vfile* virtual_libs, char* workdir_path);
void lsb_destroy(void);

#endif  // !LUA_SANDBOX_H
