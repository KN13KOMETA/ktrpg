#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "constants.h"
#include "functions.h"
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

  DEBUG_LOG(TITLE("DEBUG MODE") "\n")

  printf(TITLE("HELP"));
  printf("Run with --help\n\n");

  ecs_t* tgg_ecs = tgg_init();

  tgg_load_content(tgg_ecs, poptions->script_path);

  free_project_options(poptions);
  free(temp_argv);

#ifdef DEBUG
  ecs_run_systems(tgg_ecs, TGG_DEBUG_FLAG);
#endif /* ifdef DEBUG */

  tgg_free(tgg_ecs);

  return EXIT_SUCCESS;
}
