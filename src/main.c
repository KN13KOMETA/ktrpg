#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "character/character.h"
#include "functions.h"
#include "location/location.h"
#include "story/story.h"

#if RAND_MAX < INT16_MAX
#error RAND_MAX IS LOWER THAN INT16_MAX
#endif

#ifndef PROJECT_NAME
#error PROJECT_NAME IS NOT DEFINED
#endif /* ifndef PROJECT_NAME */

#ifndef PROJECT_VERSION
#error PROJECT_VERSION IS NOT DEFINED
#endif /* ifndef PROJECT_VERSION */

void print_info(void);
void run_game(void);
void sigint_handler(int code) {
  printf("\n");
  print_info();
  exit(code);
}

// int main(int argc, char *argv[]) {
int main(void) {
  signal(SIGINT, sigint_handler);

  run_game();

  print_info();

  return 0;
}

void print_info(void) {
  printf("\n-----< %s %s >-----\n", PROJECT_NAME, PROJECT_VERSION);
  printf(
      "Repository: https://github.com/KN13KOMETA/c_text_rpg\n"
      "Discord: https://discord.gg/ADGTqPrYr8\n"
      "Personal Discord: https://discord.gg/d4rKqZs\n");
}

void run_game(void) {
  char select = 0;
  struct Character player;
  struct Story story = get_story();

#ifdef DEBUG
  printf("\n-----< DEBUG MODE >-----\n");
#endif /* ifdef DEBUG */

  printf(STORY_DISCLAIMER);

  if (getchar_clear(&select) == EOF) return;

  switch (select) {
    case 'y':
      break;
    default: {
      printf("Consent not received, exiting the game.\n");
      return;
    }
  }

  printf(
      "\n-----< ADVENTURE START >-----\n"
      "Enter your name(%u): ",
      CHARACTER_NAME_LENGTH - 1);

  if (getchars_clear(player.name, CHARACTER_NAME_LENGTH) == EOF) return;

  // Init seed using nick
  {
    unsigned int new_seed = 0;
    size_t len = strlen(player.name);

    for (size_t i = 0; i < len; i++) new_seed += player.name[i];

    srand(new_seed);
  }

  player = generate_player(player.name);
  print_player(&player, &story);
  location_loop(&player, &story);

  printf("\n-----< ADVENTURE END >-----\n");

  print_player(&player, &story);
}
