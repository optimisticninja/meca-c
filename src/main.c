#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <unistd.h>
// #include <omp.h>

#include "args.h"
#include "meca.h"
#include "util.h"

/* TODO:
 *  - Fix stdin
 *  - Fix pipe progress/reading size
 *  - Threading
 *  - Fix uint64_t
 */

#define DEFAULT_CHUNK_SIZE 1024

int main(int argc, char *argv[])
{
  cli_args args = {.input_file = NULL,
                   .output_file = NULL,
                   .chunk_size = DEFAULT_CHUNK_SIZE,
                   .config = {.word_size = sizeof(uint32_t),
                              .rule = 0xDEADBEEFCAFEBABE,
                              .epochs = 120,
                              .neighborhood_size = 5}};

  void *buffer = malloc(args.config.word_size * args.chunk_size);
  ssize_t read;
  size_t written = 0;
  FILE *input_file, *output_file;

  parse_args(argc, argv, &args);
  validate_args(argv, &args);

  input_file = args.input_file ? fopen(args.input_file, "rb") : stdin;
  output_file = args.output_file ? fopen(args.output_file, "wb+") : stdout;

  if (output_file == NULL)
    perror("error: failed to open output file");

  while ((read = fread(buffer, args.config.word_size, args.chunk_size,
                       input_file)) ||
         !feof(input_file)) {
    // FIXME: Need to be able to write at offsets if threads finish early
    // #pragma omp parallel
    // read = fread(buffer, args.config.word_size, args.chunk_size, input_file);
    // Adjust for uneven state (state must be divided into two for current and
    // previous timestep)
    size_t adjustment = read;
    if (read % 2 != 0)
      adjustment++;

    // Process the chunk here
    switch (args.config.word_size) {
      case 1:
        CALL_MECA_CRYPT((uint8_t *) buffer, adjustment, &args.config);
        break;
      case 2:
        CALL_MECA_CRYPT((uint16_t *) buffer, adjustment, &args.config);
        break;
      case 4:
        CALL_MECA_CRYPT((uint32_t *) buffer, adjustment, &args.config);
        break;
      case 8:
        CALL_MECA_CRYPT((uint64_t *) buffer, adjustment, &args.config);
        break;
      default:
        fprintf(stderr, "error: invalid word size for encryption\n");
        exit(EXIT_FAILURE);
    }

    // FIXME: See other OMP fixme above
    // #pragma omp critical
    //     {
    // FIXME: for omp write_at(output_file, written, buffer,
    // args.config.word_size, adjustment);
    fwrite(buffer, args.config.word_size, adjustment, output_file);
    written += adjustment * args.config.word_size;
    memset(buffer, 0, adjustment * args.config.word_size);
    progressbar(written, args.config.size);
    //    }
  }
  puts("");

  if (input_file)
    fclose(input_file);

  if (output_file)
    fclose(output_file);

  free(buffer);
  buffer = NULL;

  return 0;
}
