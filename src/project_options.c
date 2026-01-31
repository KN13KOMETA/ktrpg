#include "project_options.h"

#include <argparse.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "functions.h"

static const char* const usages[] = {
    PROJECT_NAME,
    PROJECT_NAME " [options]",
    NULL,
};

void print_version(void);
void export_scripts(char* path, project_options* poptions);
void use_user_script(char* path, project_options* poptions);

int version_opt_cb(struct argparse* self, const struct argparse_option* option);
int export_opt_cb(struct argparse* self, const struct argparse_option* option);
int user_opt_cb(struct argparse* self, const struct argparse_option* option);

project_options* prompt_project_options(int argc, const char* argv[]) {
  project_options* poptions;
  SELFMALLOC(poptions);

  poptions->export_path = NULL;
  poptions->script_path = NULL;

  // NOTE: These two unused, but required for value in callback
  void* export_value = NULL;
  void* user_value = NULL;

  struct argparse_option options[] = {
      OPT_HELP(),
      OPT_GROUP("Basic options"),
      OPT_BOOLEAN('v', "version", NULL, "shows version and exit",
                  version_opt_cb, 0, OPT_NONEG),
      OPT_GROUP("Lua scripts options"),
      OPT_STRING('x', "export", &export_value,
                 "export internal scripts to specified path", export_opt_cb,
                 (intptr_t)poptions, OPT_NONEG),
      OPT_STRING('u', "user", &user_value,
                 "loads game content using script by specified path",
                 user_opt_cb, (intptr_t)poptions, OPT_NONEG),
      OPT_END(),
  };

  struct argparse argparse;
  argparse_init(&argparse, options, usages, 0);
  argparse_describe(&argparse, "\n" PROJECT_DESCRIPTION,
                    "\nHomepage: " PROJECT_HOMEPAGE_URL);
  argc = argparse_parse(&argparse, argc, argv);

  return poptions;
}

void print_version(void) {
  printf("%s %s\n", PROJECT_NAME, PROJECT_FULL_VERSION);
  exit(EXIT_SUCCESS);
}
void export_scripts(char* path, project_options* poptions) {
  FMALLOC(poptions->export_path, strlen(path) + 1);
  strcpy(poptions->export_path, path);
}
void use_user_script(char* path, project_options* poptions) {
  FMALLOC(poptions->script_path, strlen(path) + 1);
  strcpy(poptions->script_path, path);
}

int version_opt_cb(struct argparse* self,
                   const struct argparse_option* option) {
  (void)self;
  (void)option;
  print_version();
  return EXIT_SUCCESS;
}
int export_opt_cb(struct argparse* self, const struct argparse_option* option) {
  (void)self;
  export_scripts(*(char**)option->value, (project_options*)option->data);
  return EXIT_SUCCESS;
}
int user_opt_cb(struct argparse* self, const struct argparse_option* option) {
  char* path = *(char**)option->value;
  char* ext = strrchr(path, '.');

  (void)self;

  if (ext == NULL || strcmp(".lua", ext) != 0) {
    printf("Path \"%s\" is not path to lua script\n", path);
    exit(EXIT_SUCCESS);
  }

  if (file_exists(path) != 0) {
    printf("Path \"%s\" does not exists\n", path);
    exit(EXIT_SUCCESS);
  }

  use_user_script(path, (project_options*)option->data);
  return EXIT_SUCCESS;
}

void free_project_options(project_options* options) {
  if (options->script_path != NULL) free(options->script_path);
  if (options->export_path != NULL) free(options->export_path);

  free(options);
}
