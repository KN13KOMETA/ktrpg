#include "components.h"

ecs_comp_t GEN_MIN_HEALTH_COMP;
ecs_comp_t GEN_MAX_HEALTH_COMP;
ecs_comp_t GEN_NAME_COMP;

void register_components(ecs_t* ecs) {
  GEN_MIN_HEALTH_COMP =
      ecs_define_component(ecs, sizeof(gen_min_health_comp), NULL, NULL);
  GEN_MAX_HEALTH_COMP =
      ecs_define_component(ecs, sizeof(gen_max_health_comp), NULL, NULL);
  GEN_NAME_COMP = ecs_define_component(ecs, sizeof(gen_name_comp), NULL, NULL);
}
