#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "functions.h"
#include "game/ecs/components.h"
#include "game/ecs/systems.h"
#include "tgg/tgg.h"

#define PICO_ECS_IMPLEMENTATION
#include <pico_ecs.h>

#include "lt.h"

int main(void) {
  ecs_t* tgg_ecs = tgg_init();

  tgg_load_content(tgg_ecs, true);

  for (size_t i = 0; i < 2; i++) {
    ecs_run_systems(tgg_ecs, 0);
  }

  tgg_free(tgg_ecs);

  ltest();
  return 0;
  ecs_t* ecs = ecs_new(64, NULL);

  register_components(ecs);
  register_systems(ecs);

  ecs_entity_t player = ecs_create(ecs);

  debug_comp* player_debug = ecs_add(ecs, player, DEBUG_COMP, NULL);
  *player_debug = 1;

  character_name_comp* player_name =
      ecs_add(ecs, player, CHARACTER_NAME_COMP, NULL);
  {
    *player_name = malloc(MAX_NAME_LENGTH);

    printf("Enter player name: ");
    if (getchars_clear(*player_name, MAX_NAME_LENGTH) == EOF) return 1;
  }

  character_health_comp* player_health =
      ecs_add(ecs, player, CHARACTER_HEALTH_COMP, NULL);
  player_health->max = 20;
  player_health->current = 10;

  weapon_name_comp* weapon_name = ecs_add(ecs, player, WEAPON_NAME_COMP, NULL);
  *weapon_name = "sex";

  weapon_damage_comp* weapon_damage =
      ecs_add(ecs, player, WEAPON_DAMAGE_COMP, NULL);
  *weapon_damage = 66;

  for (size_t i = 0; i < 16; i++) {
    ecs_run_systems(ecs, 0);
  }

  free(*player_name);
  ecs_free(ecs);

  return 0;
}
