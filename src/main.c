#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "project_options.h"
#include "tgg/tgg.h"

#define PICO_ECS_IMPLEMENTATION
#include <pico_ecs.h>

int main(int argc, char* argv[]) {
  project_options* poptions = prompt_project_options(argc, (const char**)argv);

  printf("For help run with --help\n\n");

  ecs_t* tgg_ecs = tgg_init();

  tgg_load_content(tgg_ecs, poptions->script_path);

  free_project_options(poptions);

#ifdef DEBUG
  ecs_run_systems(tgg_ecs, 0);
#endif /* ifdef DEBUG */

  tgg_free(tgg_ecs);

  return EXIT_SUCCESS;
}
