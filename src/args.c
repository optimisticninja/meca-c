#include "args.h"

#include <getopt.h>
#include <stdio.h>

#include "util.h"

enum {
  OPTS_HELP = 'h',
  OPTS_INPUT_FILE = 'i',
  OPTS_OUTPUT_FILE = 'o',
  OPTS_CHUNK_SIZE = 'c',
  OPTS_WORD_SIZE = 'w',
  OPTS_RULE = 'r',
  OPTS_EPOCHS = 'e'
};

static void print_usage(const char *prog)
{
  printf(
      "MECA Encryptor (c) John Holly 2023 - Encrypt files/data using "
      "second-order cellular automata\n"
      "usage:\n%s -i <input-file> -o <output-file> \\\n"
      "\t[-c <chunk-size> | --chunk-size <chunk-size>] [-w <1|2|4|8> | "
      "--word-size <1|2|4|8>] \\\n\t[--rule <hex|decimal> | -r <hex|decimal>] "
      "[--epochs <decimal|hex> | -e <decimal|hex>]\n",
      prog);
}

/**
 * @breif parse shorthand CLI switches using getopt_long
 * @param argv argv string array from main
 * @return pointer to correct string
 */
char *parse_arg(char **argv)
{
  char *arg;
  if (optarg == NULL && argv[optind] != NULL && argv[optind][0] != '-') {
    arg = argv[optind++];
  } else if (optarg != NULL) {
    // handle case of argument immediately after option
    arg = optarg;
  } else {
    fprintf(stderr, "error: failed to parse arg '%s'", argv[optind]);
    exit(EXIT_FAILURE);
  }

  return arg;
}

void parse_args(int argc, char *argv[], cli_args *args)
{
  int opt;

  static struct option long_options[] = {
      {"help", no_argument, NULL, OPTS_HELP},
      {"input-file", required_argument, NULL, OPTS_INPUT_FILE},
      {"output-file", required_argument, NULL, OPTS_OUTPUT_FILE},
      {"rule", required_argument, NULL, OPTS_RULE},
      {"epochs", required_argument, NULL, OPTS_EPOCHS},
      {"chunk-size", required_argument, NULL,
       OPTS_CHUNK_SIZE}, // TODO: Default to 1024, otherwise specify
      {"word-size", required_argument, NULL,
       OPTS_WORD_SIZE}, // TODO: Default to 4 (32-bit), otherwise specify
      {0, 0, 0, 0}};

  while ((opt = getopt_long(argc, argv, "hiocwer::", long_options, NULL)) !=
         -1) {
    switch (opt) {
      case OPTS_HELP:
        print_usage(argv[0]);
        exit(EXIT_SUCCESS);
      case OPTS_INPUT_FILE:
        args->input_file = parse_arg(argv);
        break;
      case OPTS_OUTPUT_FILE:
        args->output_file = parse_arg(argv);
        break;
      case OPTS_CHUNK_SIZE:
        args->chunk_size = read_hex_or_decimal(parse_arg(argv));
        break;
      case OPTS_WORD_SIZE:
        args->config.word_size = read_hex_or_decimal(parse_arg(argv));
        break;
      case OPTS_RULE:
        args->config.rule = read_hex_or_decimal(parse_arg(argv));
        break;
      case OPTS_EPOCHS:
        args->config.epochs = read_hex_or_decimal(parse_arg(argv));
        break;
      default:
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }
  }
}

void validate_args(__attribute__((unused)) char **argv, cli_args *args)
{
  // FIXME: Set size for pipe
  size_t size = -1;

  // TODO: Warn when both piped data and an input file are provided
  // --input-file AND piped data
  if (args->input_file != NULL) { // Piped data found
    FILE *fp = fopen(args->input_file, "rb");
    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    fclose(fp);
    args->config.size = size;
  }

  // --output-file
  // The output file can be null and will be redirected to stdout instead in
  // main()

  // --word-size
  switch (args->config.word_size) {
    case 1:
    case 2:
    case 4:
    case 8:
      break;
    default:
      fprintf(stderr, "error: invalid word size; must be one of [1|2|4|8] "
                      "equating to 8-64-bit words\n");
      exit(EXIT_FAILURE);
  }

  // --chunk-size
  size_t max_chunk_size = (args->chunk_size / args->config.word_size) +
                          (args->chunk_size % args->config.word_size ? 1 : 0);
  if (max_chunk_size >
      size / args->config.word_size + (size % args->config.word_size ? 1 : 0)) {
    fprintf(
        stderr,
        "warn: chunk size greater than file size; defaulting to file size\n");
    args->chunk_size = max_chunk_size;
  }
}
