#include "components.h"

#include <stdio.h>

ecs_comp_t TGG_CHARACTER_NAME_COMP;
ecs_comp_t TGG_CHARACTER_MIN_HEALTH_COMP;
ecs_comp_t TGG_CHARACTER_MAX_HEALTH_COMP;

void tgg_register_components(ecs_t* ecs) {
  TGG_CHARACTER_NAME_COMP =
      ecs_define_component(ecs, sizeof(tgg_character_name_comp), NULL, NULL);
  TGG_CHARACTER_MIN_HEALTH_COMP = ecs_define_component(
      ecs, sizeof(tgg_character_min_health_comp), NULL, NULL);
  TGG_CHARACTER_MAX_HEALTH_COMP = ecs_define_component(
      ecs, sizeof(tgg_character_max_health_comp), NULL, NULL);
}

void tgg_character_name_comp_debug(tgg_character_name_comp* comp) {
  printf("tgg_character_name_comp: %s\n", *comp);
}
void tgg_character_min_health_comp_debug(tgg_character_min_health_comp* comp) {
  printf("tgg_character_min_health_comp: %f\n", *comp);
}
void tgg_character_max_health_comp_debug(tgg_character_max_health_comp* comp) {
  printf("tgg_character_max_health_comp: %f\n", *comp);
}
