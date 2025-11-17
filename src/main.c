#include <getopt.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "functions.h"
#include "game/ecs/components.h"
#include "game/ecs/systems.h"
#include "tgg/tgg.h"

#define PICO_ECS_IMPLEMENTATION
#include <pico_ecs.h>

int main(int argc, char* argv[]) {
  {
    // Do not print error
    opterr = 0;

    int opt = '?';
    while ((opt = getopt(argc, argv, "VH")) != -1) {
      switch (opt) {
        case 'V':
          goto process_option;
        case 'L':
          goto process_option;
        case 'H':
          goto process_option;
        default:
          goto process_option;
      }
    }

  process_option:
    switch (opt) {
      case 'V':
        printf("%s %s\n\n", PROJECT_NAME, PROJECT_FULL_VERSION);
        break;
      case 'H':
        printf("%s\n\n", PROJECT_DESCRIPTION);

        printf("Usage: %s [OPTION]\n\n", argv[0]);

        printf(
            "Options:\n"
            "  -V, --version               "
            "prints program version\n"
            "  -I, --internal              "
            "load game with internal lua scripts\n"
            "  -X [PATH], --extract [PATH] "
            "extracts internal lua scripts to given path\n"
            "  -L PATH, --lua PATH         "
            "path to lua script for custom content\n"
            "  -H, --help                  "
            "prints this text\n"
            "\n");

        printf("Project homepage: %s\n", PROJECT_HOMEPAGE_URL);
        break;
      default:
        printf("SWW: Unknown option.");
    }
  }

  ecs_t* tgg_ecs = tgg_init();

  tgg_load_content(tgg_ecs, false);

  for (size_t i = 0; i < 2; i++) {
    ecs_run_systems(tgg_ecs, 0);
  }

  tgg_free(tgg_ecs);

  return EXIT_SUCCESS;
}
