#pragma once
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <lua.h>
#include <stdio.h>
#include <stdlib.h>

#define ANY2STR(any) #any
#define EXPAND2STR(x) ANY2STR(x)

// TODO: Make debug show short filenames
#ifdef DEBUG
#define DEBUG_LOG(...)                \
  printf(__FILE__ ":%d: ", __LINE__); \
  printf(__VA_ARGS__);                \
  putchar('\n')
#else
#define DEBUG_LOG(...)
#endif  // DEBUG

#define RND_RANGE(max, min) rand() % (max + 1 - min) + min
#define RND_MAX(max) RND_RANGE(max, 0)

// Reads char from stdin and clears it's buffer.
// Returns last char from input.
int getchar_clear(char* c);

// Reads string from stdin and clears it's buffer.
// len is including '\0'.
// Returns last char from input.
int getchars_clear(char* str, size_t len);

// Checks if file exists on path.
// Returns 0 when file exists
int file_exists(char* path);

// Registers a lua module with content as code
void register_lua_text_module(lua_State* L, char* name, char* content);

#endif  // !FUNCTIONS_H
