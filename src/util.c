#include "util.h"

#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

void progressbar(size_t written, size_t total)
{
  char pbstr[PBWIDTH];
  memset(pbstr, PBCHAR, PBWIDTH);
  int percent = 100 * (double) ((double) written / (double) total);
  fprintf(stderr, "\r[%-" S(PBWIDTH) ".*s] %u%% %zu/%zu",
          percent * PBWIDTH / 100, pbstr, percent, written, total);
}

uint64_t read_hex_or_decimal(const char *str)
{
  char *end;
  uint64_t result;
  errno = 0;
  result = strtoull(str, &end, 16);

  if (result == 0 && end == str) {
    fprintf(stderr, "error: %s was not a number\n", str);
    exit(EXIT_FAILURE);
  } else if (result == ULLONG_MAX && errno) {
    fprintf(stderr, "error: %s does not fit in a uint64_t\n", str);
    exit(EXIT_FAILURE);
  } else if (*end) {
    fprintf(stderr, "error: %s has junk data after the number\n", str);
    exit(EXIT_FAILURE);
  }

  return result;
}
