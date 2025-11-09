#include "systems.h"

#include <stdio.h>

#define COMPONENTS_DEBUG_IMPLEMENTATION
#include "components.h"

ecs_system_t RENDER_SYSTEM;

void register_systems(ecs_t* ecs) {
  RENDER_SYSTEM = ecs_define_system(ecs, 0, debug_system, NULL, NULL, NULL);

  ecs_require_component(ecs, RENDER_SYSTEM, DEBUG_COMP);
}

#define debug_comp(comp, f)         \
  if (ecs_has(ecs, entity, comp)) { \
    printf("  ");                   \
    f(ecs_get(ecs, entity, comp));  \
  }

ecs_ret_t debug_system(ecs_t* ecs, ecs_entity_t* entities, size_t entity_count,
                       void* udata) {
  (void)udata;

  printf("DEBUG SYSTEM\n");

  for (size_t i = 0; i < entity_count; i++) {
    ecs_entity_t entity = entities[i];

    printf("DEBUG ENTITY [%lu] {\n", entity.id);

    debug_comp(CHARACTER_NAME_COMP, character_name_comp_debug);
    debug_comp(CHARACTER_HEALTH_COMP, character_health_comp_debug);
    debug_comp(WEAPON_NAME_COMP, weapon_name_comp_debug);
    debug_comp(WEAPON_DAMAGE_COMP, weapon_damage_comp_debug);
    debug_comp(DEBUG_COMP, debug_comp_debug);

    printf("}\n");
  }

  return 0;
}

#undef debug_comp
