#include "systems.h"

#include <stdio.h>

#define TGG_COMPONENTS_DEBUG_IMPLEMENTATION
#include "components.h"

ecs_system_t TGG_DEBUG_SYSTEM;

void tgg_register_systems(ecs_t* ecs) {
  TGG_DEBUG_SYSTEM =
      ecs_define_system(ecs, 0, tgg_debug_system, NULL, NULL, NULL);

  // NOTE: in this case i decided currently not to add debug comp
  // ecs_require_component(ecs, DEBUG_SYSTEM, DEBUG_COMP);
}

#define debug_comp(comp, f)         \
  if (ecs_has(ecs, entity, comp)) { \
    printf("  ");                   \
    f(ecs_get(ecs, entity, comp));  \
  }

ecs_ret_t tgg_debug_system(ecs_t* ecs, ecs_entity_t* entities,
                           size_t entity_count, void* udata) {
  (void)udata;

  printf("TGG DEBUG SYSTEM\n");

  for (size_t i = 0; i < entity_count; i++) {
    ecs_entity_t entity = entities[i];

    printf("DEBUG ENTITY [%lu] {\n", entity.id);

    debug_comp(TGG_CHARACTER_NAME_COMP, tgg_character_name_comp_debug);
    debug_comp(TGG_CHARACTER_MIN_HEALTH_COMP,
               tgg_character_min_health_comp_debug);
    debug_comp(TGG_CHARACTER_MAX_HEALTH_COMP,
               tgg_character_max_health_comp_debug);

    printf("}\n");
  }

  return 0;
}

#undef debug_comp
