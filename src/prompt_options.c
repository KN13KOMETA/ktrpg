#include "prompt_options.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "argparse.h"

static const char* const usages[] = {
    PROJECT_NAME,
    PROJECT_NAME " [options]",
    NULL,
};

void print_version(void);
void export_scripts(char* path);
void use_user_script(char* path);

int version_opt_cb(struct argparse* self, const struct argparse_option* option);
int export_opt_cb(struct argparse* self, const struct argparse_option* option);
int user_opt_cb(struct argparse* self, const struct argparse_option* option);

project_options* prompt_options(int argc, const char* argv[]) {
  void* __user = NULL;
  struct argparse_option options[] = {
      OPT_HELP(),
      OPT_GROUP("Basic options"),
      OPT_BOOLEAN('v', "version", NULL, "shows version", version_opt_cb, 0,
                  OPT_NONEG),
      OPT_GROUP("Lua scripts options"),
      OPT_BOOLEAN('x', "export", NULL, "exports internal lua script",
                  export_opt_cb, 0, OPT_NONEG),
      OPT_STRING('u', "user", &__user,
                 "use user script for content load by path", user_opt_cb, 0,
                 OPT_NONEG),
      OPT_END(),
  };

  struct argparse argparse;
  argparse_init(&argparse, options, usages, 0);
  argparse_describe(&argparse, "\n" PROJECT_DESCRIPTION,
                    "\nHomepage: " PROJECT_HOMEPAGE_URL);
  argc = argparse_parse(&argparse, argc, argv);
}

void print_version(void) {
  printf("%s %s\n", PROJECT_NAME, PROJECT_FULL_VERSION);
  exit(EXIT_SUCCESS);
}
void export_scripts(char* path);
void use_user_script(char* path) {}

int version_opt_cb(struct argparse* self,
                   const struct argparse_option* option) {
  (void)self;
  (void)option;
  print_version();
  return EXIT_SUCCESS;
}
int export_opt_cb(struct argparse* self, const struct argparse_option* option) {
  return EXIT_SUCCESS;
}
int user_opt_cb(struct argparse* self, const struct argparse_option* option) {
  (void)self;
  printf("sex : %d\n", option->data) use_user_script(*(char**)option->value);
  return EXIT_SUCCESS;
}

void free_project_options(project_options* options) {}
