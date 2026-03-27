#include "functions.h"

#include <lauxlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32

#include <direct.h>
#include <fileapi.h>
#include <sys/stat.h>
#include <windows.h>

#define stat _stat

#else

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#endif  // ifdef _WIN32

void sleep_s(unsigned seconds) {
  // TODO: Check on windows
#ifdef _WIN32
  Sleep(seconds * 1000);
#else
  sleep((unsigned)seconds);
#endif  // ifdef _WIN32
}

// SplitMix64
// Written by Sebastiano Vigna (vigna@acm.org)
// For license check:
// https://prng.di.unimi.it/splitmix64.c
uint64_t rnd64_next(uint64_t* state) {
  uint64_t z = (*state += 0x9e3779b97f4a7c15);
  z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
  z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
  return z ^ (z >> 31);
}

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

int ask_yn(const char* text) {
  char c;

  printf("%s (y/N): ", text);

  getchar_clear(&c);

  switch (c) {
    case 'y':
    case 'Y':
      return 0;
      break;
    default:
      return 1;
  }
}

char human_bytes(size_t bytes, double* result) {
  char* units = "BKMGTPE";
  *result = (double)bytes;

  while (*result >= 1024 && *(units + 1) != '\0') {
    *result /= 1024;
    units++;
  }

  return *units;
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

#ifdef _WIN32
  _mkdir(path);
#else
  mkdir(path, DIRECTORY_DEFAULT_MODE);
#endif  // ifdef _WIN32
}

int write_vfiles_to_dir(vfile* vfiles, char* dir) {
  int code = 0;
  size_t dir_len = strlen(dir);

  for (int i = 0; vfiles[i].path != NULL; i++) {
    vfile f = vfiles[i];
    char filepath[dir_len + 1 + strlen(f.path) + 1];
    char basedir[dir_len + 1 + strlen(f.path) + 1];
    FILE* file;

    sprintf(filepath, "%s/%s", dir, f.path);

    get_basedir(filepath, basedir);

    create_dir_recursive(basedir);

    file = fopen(filepath, "w");

    if (file == NULL) {
      code++;
      continue;
    }

    fwrite(f.content, strlen(f.content), 1, file);

    fclose(file);
  }

  return code;
}
