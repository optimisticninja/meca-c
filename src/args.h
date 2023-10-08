#pragma once

#include "meca.h"

typedef struct {
  char *input_file;
  char *output_file;
  size_t chunk_size;
  meca_config config;
} cli_args;

char *parse_arg(char **argv);
void parse_args(int argc, char *argv[], cli_args *args);
void validate_args(__attribute__((unused)) char **argv, cli_args *args);
