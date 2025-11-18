#pragma once
#ifndef PROMPT_OPTIONS
#define PROMPT_OPTIONS

typedef struct {
  char* script_path;
  char* save_path;
} project_options;

project_options* prompt_project_options(int argc, const char* argv[]);

void free_project_options(project_options* poptions);

#endif  // !PROMPT_OPTIONS
