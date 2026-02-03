#pragma once
#ifndef CONSTANTS_H

#define TITLE_L "\n::> "
#define TITLE_R " <::"
#define TITLE(s) TITLE_L s TITLE_R "\n"

#define LIST_L " ) "
#define LIST(n, s) n LIST_L s "\n"

#define ACT_L "::) "

typedef struct {
  void* ptr;
} ptr2ptr;

typedef struct {
  const char* path;
  const char* content;
} vfile;

#endif  // !CONSTANTS_H
