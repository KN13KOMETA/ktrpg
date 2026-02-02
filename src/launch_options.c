#include "launch_options.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "argparse.h"
#include "functions.h"

static const char* const usages[] = {
    PROJECT_NAME,
    PROJECT_NAME " [options]",
    NULL,
};

int process_launch_options(launch_options* loptions, int argc, char* argv[]) {
  int code = LO_EXIT_SUCCESS;
  const char* temp_argv[argc];
  struct argparse ap;
  struct argparse_option options[] = {
      OPT_BOOLEAN('h', "help", &loptions->help,
                  "show this help message and exit", NULL, 0, OPT_NONEG),

      OPT_GROUP("Basic options"),
      OPT_BOOLEAN('v', "version", &loptions->version,
                  "show version information and exit", NULL, 0, OPT_NONEG),

      OPT_GROUP("Lua scripting options"),
      OPT_STRING('l', "load", &loptions->script_path,
                 "load game content using Lua script from specified path", NULL,
                 0, 0),

      OPT_GROUP("Embedded content management"),
      OPT_STRING('\0', "export-scripts", &loptions->scripts_export_path,
                 "export embedded Lua scripts to specified directory", NULL, 0,
                 0),
      OPT_STRING('\0', "export-types", &loptions->types_export_path,
                 "export Lua type annotations to specified directory", NULL, 0,
                 0),

      OPT_END(),
  };

  for (int i = 0; i < argc; i++) temp_argv[i] = argv[i];
  temp_argv[argc] = NULL;

  parse_launch_options(loptions, &ap, options, argc, temp_argv);

  code = validate_launch_options(loptions);

  if (code == LO_EXIT_SUCCESS)
    code = handle_launch_options_actions(loptions, &ap);

  return code;
}

void parse_launch_options(launch_options* loptions, struct argparse* ap,
                          struct argparse_option* options, int argc,
                          const char* argv[]) {
  (void)loptions;

  argparse_init(ap, options, usages, 0);

  argparse_describe(ap, "\n" PROJECT_DESCRIPTION,
                    "\nHomepage: " PROJECT_HOMEPAGE_URL);

  argparse_parse(ap, argc, argv);
}
int validate_launch_options(launch_options* loptions) {
  if (loptions->script_path != NULL) {
    char* path = loptions->script_path;
    char* ext = strrchr(path, '.');

    if (ext == NULL || strcmp(".lua", ext) != 0) {
      printf("Path \"%s\" is not path to lua script\n", path);
      return LO_EXIT_ERROR;
    }

    if (file_exists(path) != 0) {
      printf("Path \"%s\" does not exists\n", path);
      return LO_EXIT_ERROR;
    }
  }

  // TODO: export paths validation

  return LO_EXIT_SUCCESS;
}
int handle_launch_options_actions(launch_options* loptions,
                                  struct argparse* ap) {
  if (loptions->help) {
    argparse_usage(ap);
    return LO_EXIT_ACTION;
  }
  if (loptions->version) {
    printf(PROJECT_NAME " " PROJECT_FULL_VERSION "\n");
    return LO_EXIT_ACTION;
  }

  return LO_EXIT_SUCCESS;
}
