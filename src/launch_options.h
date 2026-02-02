#pragma once
#ifndef LAUNCH_OPTIONS_H
#define LAUNCH_OPTIONS_H

#include <argparse.h>
#include <stdint.h>

#define LO_EXIT_SUCCESS 0
#define LO_EXIT_ACTION 1
#define LO_EXIT_ERROR 2

typedef struct {
  uint8_t help;
  uint8_t version;
  char* script_path;
  char* scripts_export_path;
  char* types_export_path;
} launch_options;

int process_launch_options(launch_options* loptions, int argc, char* argv[]);

void parse_launch_options(launch_options* loptions, struct argparse* ap,
                          struct argparse_option* options, int argc,
                          const char* argv[]);
int validate_launch_options(launch_options* loptions);
int handle_launch_options_actions(launch_options* loptions,
                                  struct argparse* ap);

#endif  // !LAUNCH_OPTIONS_H
