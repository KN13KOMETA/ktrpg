#include "components.h"

ecs_comp_t CHARACTER_NAME_COMP;
ecs_comp_t CHARACTER_HEALTH_COMP;
ecs_comp_t WEAPON_NAME_COMP;
ecs_comp_t WEAPON_DAMAGE_COMP;

ecs_comp_t DEBUG_COMP;

void register_components(ecs_t* ecs) {
  CHARACTER_NAME_COMP =
      ecs_define_component(ecs, sizeof(character_name_comp), NULL, NULL);
  CHARACTER_HEALTH_COMP =
      ecs_define_component(ecs, sizeof(character_health_comp), NULL, NULL);
  WEAPON_NAME_COMP =
      ecs_define_component(ecs, sizeof(weapon_name_comp), NULL, NULL);
  WEAPON_DAMAGE_COMP =
      ecs_define_component(ecs, sizeof(weapon_damage_comp), NULL, NULL);
  DEBUG_COMP = ecs_define_component(ecs, sizeof(debug_comp), NULL, NULL);
}
