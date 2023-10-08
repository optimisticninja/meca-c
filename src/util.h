#pragma once

#include <stdint.h>
#include <stdio.h>

#define PERRORF(FMT, ...)                                                      \
  fprintf(stderr, FMT ": %s\n", ##__VA_ARGS__, strerror(errno))

// String methods
#define S_(x) #x
#define S(x) S_(x)

// Progress bar
#define PBWIDTH 64
#define PBCHAR '#'

void progressbar(size_t written, size_t total);

uint64_t read_hex_or_decimal(const char *str);
