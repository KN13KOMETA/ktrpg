#pragma once
#ifndef COMPONENT_H
#define COMPONENT_H

#include <pico_ecs.h>
#include <stdint.h>

#include "../game_constants.h"

/* PRINCIPLES FOR COMPONENTS
 * Every component must have a typedef (name_comp)
 * Every_component must have extern ecs_comp_t (NAME_COMP)
 * Every component must have debug function
 */

typedef name_t character_name_comp;
typedef health_t character_health_comp;
typedef name_t weapon_name_comp;
typedef hpint_t weapon_damage_comp;
typedef bool_t debug_comp;

extern ecs_comp_t CHARACTER_NAME_COMP;
extern ecs_comp_t CHARACTER_HEALTH_COMP;
extern ecs_comp_t WEAPON_NAME_COMP;
extern ecs_comp_t WEAPON_DAMAGE_COMP;
extern ecs_comp_t DEBUG_COMP;

void character_name_comp_debug(character_name_comp* comp);
void character_health_comp_debug(character_health_comp* comp);
void weapon_name_comp_debug(weapon_name_comp* comp);
void weapon_damage_comp_debug(weapon_damage_comp* comp);
void debug_comp_debug(debug_comp* comp);

void register_components(ecs_t* ecs);

#endif  // !COMPONENT_H

// Component debug functionds implementation
#ifdef COMPONENTS_DEBUG_IMPLEMENTATION

#include <stdio.h>

void character_name_comp_debug(character_name_comp* comp) {
  printf("character_name_comp: %s\n", *comp);
}
void character_health_comp_debug(character_health_comp* comp) {
  printf("character_health_comp: [max] %u | [current] %u\n", comp->max,
         comp->current);
}
void weapon_name_comp_debug(weapon_name_comp* comp) {
  printf("weapon_name_comp: %s\n", *comp);
}
void weapon_damage_comp_debug(weapon_damage_comp* comp) {
  printf("weapon_damage_comp: %u\n", *comp);
}
void debug_comp_debug(debug_comp* comp) { printf("debug_comp: %u\n", *comp); }

#endif  // COMPONENTS_DEBUG_IMPLEMENTATION
