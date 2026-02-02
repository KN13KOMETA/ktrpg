#pragma once
#ifndef PROJECT_OPTIONS_H
#define PROJECT_OPTIONS_H

typedef struct {
  char* script_path;
  char* scripts_export_path;
} project_options;

project_options* prompt_project_options(int argc, const char* argv[]);

void free_project_options(project_options* poptions);

#endif  // !PROJECT_OPTIONS_H
