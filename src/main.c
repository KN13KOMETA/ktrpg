#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "constants.h"
#include "functions.h"
#include "luab/luab_core.h"
#include "project_options.h"
#include "tgg/systems.h"
#include "tgg/tgg.h"

#define PICO_ECS_IMPLEMENTATION
#include <pico_ecs.h>

int main(int argc, char* argv[]) {
  const char** temp_argv =
      (const char**)malloc(((size_t)argc + 1) * sizeof(const char*));
  project_options* poptions;

  for (int i = 0; i < argc; i++) temp_argv[i] = argv[i];
  temp_argv[argc] = NULL;

  poptions = prompt_project_options(argc, temp_argv);

  DEBUG_LOG(TITLE("DEBUG MODE") "\n");

  printf(TITLE("HELP"));
  printf("Run with --help for more info\n\n");

  printf(TITLE("RUNNING LUAB"));
  luab_init();
  printf(TITLE("END OF LUAB"));

  free_project_options(poptions);
  free(temp_argv);

  return EXIT_SUCCESS;
}
