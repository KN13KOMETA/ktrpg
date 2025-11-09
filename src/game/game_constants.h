#pragma once
#ifndef GAME_CONSTANTS_H
#define GAME_CONSTANTS_H

#include <stdint.h>

#define MAX_NAME_LENGTH 32

// UInt type for HP for easy switch
typedef uint16_t hpint_t;
typedef uint8_t bool_t;

typedef enum { FIRE, WATER, EARTH, AIR } element_t;
typedef char* name_t;
typedef struct {
  hpint_t max;
  hpint_t current;
} health_t;

#endif  // !GAME_CONSTANTS_H
