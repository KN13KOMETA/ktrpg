#pragma once
#ifndef TGG_COMPONENTS_H
#define TGG_COMPONENTS_H

#include <pico_ecs.h>
#include <stdint.h>

typedef char* string_t;
typedef double health_t;

/* PRINCIPLES FOR COMPONENTS
 * Every component must have a typedef (name_comp)
 * Every component must have extern ecs_comp_t (NAME_COMP)
 * Every component must have debug function
 */

typedef string_t tgg_character_name_comp;
typedef health_t tgg_character_min_health_comp;
typedef health_t tgg_character_max_health_comp;

extern ecs_comp_t TGG_CHARACTER_NAME_COMP;
extern ecs_comp_t TGG_CHARACTER_MIN_HEALTH_COMP;
extern ecs_comp_t TGG_CHARACTER_MAX_HEALTH_COMP;

void tgg_character_name_comp_debug(tgg_character_name_comp* comp);
void tgg_character_min_health_comp_debug(tgg_character_min_health_comp* comp);
void tgg_character_max_health_comp_debug(tgg_character_max_health_comp* comp);

void tgg_register_components(ecs_t* ecs);

#endif  // !TGG_COMPONENTS_H

#ifdef TGG_COMPONENTS_DEBUG_IMPLEMENTATION

#include <stdio.h>

void tgg_character_name_comp_debug(tgg_character_name_comp* comp) {
  printf("tgg_character_name_comp: %s\n", *comp);
}
void tgg_character_min_health_comp_debug(tgg_character_min_health_comp* comp) {
  printf("tgg_character_min_health_comp: %f\n", *comp);
}
void tgg_character_max_health_comp_debug(tgg_character_max_health_comp* comp) {
  printf("tgg_character_max_health_comp: %f\n", *comp);
}

#endif  // TGG_COMPONENTS_DEBUG_IMPLEMENTATION
