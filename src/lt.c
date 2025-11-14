#include "lt.h"

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "luah/example.h"

// WARN: TEST SHIT, I WILL REMOVE THIS A BIT LATER
// TODO: understand what the fuck is going on here

int load_script(lua_State* L, const char* filename) {
  const char* possible_paths[] = {filename,
                                  // "./scripts/user_data.lua",
                                  // "~/.config/c_text_rpg/user_data.lua",
                                  NULL};

  for (int i = 0; possible_paths[i] != NULL; i++) {
    if (access(possible_paths[i], F_OK) == 0) {
      printf("Loading user script: %s\n", possible_paths[i]);
      return luaL_dofile(L, possible_paths[i]);
    }
  }

  // If not found script give example
  printf("User script not found, using built-in script\n");
  return luaL_loadstring(L, example_lua) || lua_pcall(L, 0, 0, 0);
}

int* load_int_array_from_lua(lua_State* L, const char* function_name,
                             int* out_size) {
  // Get func from lua
  lua_getglobal(L, function_name);
  if (!lua_isfunction(L, -1)) {
    printf("Function %s not found in Lua\n", function_name);
    return NULL;
  }

  // Call lua func
  if (lua_pcall(L, 0, 1, 0) != 0) {
    printf("Error calling %s: %s\n", function_name, lua_tostring(L, -1));
    lua_pop(L, 1);
    return NULL;
  }

  // Check that we got table
  if (!lua_istable(L, -1)) {
    printf("%s did not return a table\n", function_name);
    lua_pop(L, 1);
    return NULL;
  }

  // Get table len
  int size = lua_objlen(L, -1);
  int* arr = malloc(size * sizeof(int));

  // Read data from table
  for (int i = 0; i < size; i++) {
    // NOTE: do not forgor lua arrays indexed from 1
    lua_rawgeti(L, -1, i + 1);
    arr[i] = lua_tointeger(L, -1);
    // Remove value from stack
    // TODO: ^^^ currently i am too dumb to understand tf is stack in lua
    lua_pop(L, 1);
  }

  // Remove table from stack
  lua_pop(L, 1);

  if (out_size) *out_size = size;
  return arr;
}

typedef struct {
  char name[50];
  int damage;
  int price;
} Weapon;

Weapon* load_weapons_from_lua(lua_State* L, const char* function_name,
                              int* out_count) {
  lua_getglobal(L, function_name);
  if (!lua_isfunction(L, -1)) {
    printf("Function %s not found\n", function_name);
    return NULL;
  }

  if (lua_pcall(L, 0, 1, 0) != 0) {
    printf("Error: %s\n", lua_tostring(L, -1));
    lua_pop(L, 1);
    return NULL;
  }

  if (!lua_istable(L, -1)) {
    printf("Expected table from %s\n", function_name);
    lua_pop(L, 1);
    return NULL;
  }

  int count = lua_objlen(L, -1);
  Weapon* weapons = malloc(count * sizeof(Weapon));

  for (int i = 0; i < count; i++) {
    lua_rawgeti(L, -1, i + 1);

    // Read table fields
    if (lua_istable(L, -1)) {
      lua_getfield(L, -1, "name");
      const char* name = lua_tostring(L, -1);
      if (name) {
        strncpy(weapons[i].name, name, sizeof(weapons[i].name) - 1);
        weapons[i].name[sizeof(weapons[i].name) - 1] = '\0';
      } else {
        weapons[i].name[0] = '\0';
      }
      lua_pop(L, 1);

      lua_getfield(L, -1, "damage");
      weapons[i].damage = lua_tointeger(L, -1);
      lua_pop(L, 1);

      lua_getfield(L, -1, "price");
      weapons[i].price = lua_tointeger(L, -1);
      lua_pop(L, 1);
    }

    lua_pop(L, 1);
  }

  lua_pop(L, 1);

  if (out_count) *out_count = count;
  return weapons;
}

void ltest(void) {
  lua_State* L = luaL_newstate();
  luaL_openlibs(L);

  // Load and run lua script
  if (load_script(L, "user_data.lua") != 0) {
    printf("Error loading script: %s\n", lua_tostring(L, -1));
    lua_pop(L, 1);
    lua_close(L);
    return;
  }

  // Get number array from lua script
  int size;
  int* numbers = load_int_array_from_lua(L, "init_arr", &size);

  if (numbers) {
    printf("Loaded array with %d elements:\n", size);
    for (int i = 0; i < size; i++) {
      printf("  [%d] = %d\n", i, numbers[i]);
    }
    free(numbers);
  }

  // Load more complex shit
  int weapon_count;
  Weapon* weapons = load_weapons_from_lua(L, "get_weapon_data", &weapon_count);

  if (weapons) {
    printf("\nLoaded %d weapons:\n", weapon_count);
    for (int i = 0; i < weapon_count; i++) {
      printf("  %s: damage=%d, price=%d\n", weapons[i].name, weapons[i].damage,
             weapons[i].price);
    }
    free(weapons);
  }

  lua_close(L);
}
