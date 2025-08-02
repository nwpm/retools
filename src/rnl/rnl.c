#include "rnl.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int rnl_int_from_str(const char *n_str, int *res) {

  if (*n_str == '\0' || !n_str) {
    fprintf(stderr, "invalid argument: empty string\n");
    return -1;
  }

  char *endptr;

  long val = strtol(n_str, &endptr, 10);

  if (val > INT_MAX || val < INT_MIN) {
    fprintf(stderr, "invalid number (out of range): '%s'\n", n_str);
    return -1;
  }

  if (*endptr != '\0') {
    fprintf(stderr, "invalid number format: '%s'\n", n_str);
    return -1;
  }

  *res = (int)val;
  return 0;
}

int rnl_parse_options_from_argv(RNlOptions *options, int argc, char **argv) {

  int c;
  int exit_status = 0;

  while ((c = getopt(argc, argv, "v:i:s:bfhdnwl")) != -1) {
    switch (c) {
    case 'b':
      options->bflag = true;
      break;
    case 'f':
      options->fflag = true;
      break;
    case 'h':
      options->hflag = true;
      break;
    case 'd':
      options->dflag = true;
      break;
    case 'n':
      options->nflag = true;
      break;
    case 's':
      options->separator_str = optarg;
      break;
    case 'w':
      options->wflag = true;
      break;
    case 'v':
      exit_status = rnl_int_from_str(optarg, &options->start_line_position);
      break;
    case 'i':
      exit_status = rnl_int_from_str(optarg, &options->line_step);
      break;
    case 'l':
      options->lflag = true;
      break;
    case '?':
      fprintf(stderr, "Try rtw --help for more information\n");
      exit_status = -1;
    }
  }

  return exit_status;
}

static void rnl_read_and_print_stream(FILE *stream, RNlOptions *options,
                                      int *linecounter) {

  char *line = NULL;
  size_t init_size = 0;

  while ((getline(&line, &init_size, stream) != -1) && !feof(stream)) {

    printf("\t%d%s", (*linecounter), options->separator_str);
    (*linecounter) += options->line_step;

    for (size_t i = 0; line[i] != '\0'; ++i) {
      putchar(line[i]);
    }
  }

  free(line);
}

int rnl(int argc, char **argv, RNlOptions *options) {

  int linecounter = options->start_line_position;

  if (optind == argc) {
    rnl_read_and_print_stream(stdin, options, &linecounter);
    return 0;
  }

  for (int i = optind; i < argc; ++i) {
    if (strcmp(argv[i], "-") == 0) {
      rnl_read_and_print_stream(stdin, options, &linecounter);
    } else {

      FILE *file_stream = fopen(argv[i], "r");

      if (!file_stream) {
        perror("Error opening file");
        return -1;
      }

      rnl_read_and_print_stream(file_stream, options, &linecounter);

      fclose(file_stream);
    }
  }

  return 0;
}

int main(int argc, char **argv) {

  RNlOptions options = {false, false, false, false, false,
                        "\t",  false, 1,     1,     false};

  if (rnl_parse_options_from_argv(&options, argc, argv) != 0)
    return -1;

  if (rnl(argc, argv, &options) != 0)
    return -1;

  return 0;
}
