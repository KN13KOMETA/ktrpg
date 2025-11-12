#include "enemy.h"

#include "components.h"

ecs_t *gen_enemy_generator(void) {
  ecs_t *ecs = ecs_new(16, NULL);
  register_components(ecs);

  {
    ecs_entity_t enemy = ecs_create(ecs);

    gen_name_comp *name = ecs_add(ecs, enemy, GEN_NAME_COMP, NULL);
    gen_min_health_comp *min_health =
        ecs_add(ecs, enemy, GEN_MIN_HEALTH_COMP, NULL);
    gen_max_health_comp *max_health =
        ecs_add(ecs, enemy, GEN_MAX_HEALTH_COMP, NULL);

    *name = "Amogus";
    *min_health = 10;
    *max_health = 20;
  }

  return ecs;
}

ecs_entity_t gen_enemy(ecs_t *ecs) {}
