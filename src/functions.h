#pragma once
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <lua.h>
#include <stdio.h>
#include <stdlib.h>

#include "constants.h"

#define ANY2STR(any) #any
#define EXPAND2STR(x) ANY2STR(x)

#define FMALLOC(ptr, size)                             \
  {                                                    \
    size_t _s = size;                                  \
    ptr = malloc(_s);                                  \
    if (ptr == NULL) {                                 \
      printf(__FILE__ ":%d: FATAL ERROR\n", __LINE__); \
      printf("Failed to allocate memory\n");           \
      printf("PTR = " #ptr "\n");                      \
      printf("SIZE (%zu) = " #size "\n", _s);          \
      exit(0);                                         \
    }                                                  \
  }

#define SELFMALLOC(ptr) FMALLOC(ptr, sizeof(*ptr))
#define SELFMALLOCARR(ptr, arrsize) FMALLOC(ptr, sizeof(*ptr) * arrsize)

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

// Asks for y/n
// Returns 0 if y
int ask_yn(char* text);

// Turns bytes into human readable form
// Returns char from "BKMGTPE"
char human_bytes(size_t bytes, double* result);

// Checks if file exists on path.
// Returns 0 when file exists
int file_exists(char* path);

// Checks if directory exists on path.
// Returns 0 when directory exists
int directory_exists(char* path);

// Checks if directory has files/directories.
// Returns 0 when directory is empty
int directory_empty(char* path);

// Sets basedir
// from /path/file.txt to /path
// Returns 0 when succesful
int get_basedir(char* path, char* basedir);

void create_dir_recursive(char* path);

// Returns count of failed writes
int write_vfiles_to_dir(vfile* vfiles, char* dir);

// Registers a lua module with content as code
void register_lua_text_module(lua_State* L, const char* name,
                              const char* content);

// Allocates memory
// Returns path/?.lua;path/?/init.lua
char* build_lua_package_search_path(const char* path);

// Adds path to lua package.path
void add_lua_package_path(lua_State* L, const char* path);

#endif  // !FUNCTIONS_H
