#include "tgg.h"

#include "components.h"
#include "systems.h"

ecs_t* tgg_init(void) {
  ecs_t* tgg_ecs = ecs_new(64, NULL);

  tgg_register_components(tgg_ecs);
  tgg_register_systems(tgg_ecs);

  return tgg_ecs;
}

void tgg_free(ecs_t* tggecs) { ecs_free(tggecs); }
