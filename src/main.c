#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "argparse.h"
#include "functions.h"
#include "game/ecs/components.h"
#include "game/ecs/systems.h"
#include "prompt_options.h"
#include "tgg/tgg.h"

#define PICO_ECS_IMPLEMENTATION
#include <pico_ecs.h>

int main(int argc, char* argv[]) {
  prompt_options(argc, argv);
  printf("AMONGUS. START OF GAME.\n\n");

  ecs_t* tgg_ecs = tgg_init();

  tgg_load_content(tgg_ecs, false);

  for (size_t i = 0; i < 2; i++) {
    ecs_run_systems(tgg_ecs, 0);
  }

  tgg_free(tgg_ecs);

  return EXIT_SUCCESS;
}
