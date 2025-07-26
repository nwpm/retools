#include "rcat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int rcat_parse_flags_from_argv(RCatFlags *flags, int argc, char **argv) {

  int c;

  while ((c = getopt(argc, argv, "nbsvTE")) != -1) {
    switch (c) {
    case 'E':
      flags->Eflag = true;
      break;
    case 'n':
      flags->nflag = true;
      break;
    case 'b':
      flags->bflag = true;
      break;
    case 'v':
      flags->vflag = true;
      break;
    case 'T':
      flags->Tflag = true;
      break;
    case 's':
      flags->sflag = true;
      break;
    case '?':
      fprintf(stderr, "Try rtw --help for more information\n");
      return -1;
    }
  }

  return 0;
}

static void rcat_read_and_print_stream(FILE *stream, RCatFlags *flags,
                                      int *linecounter) {

  char *line = NULL;
  size_t init_size = 0;
  bool prev_was_empty = false;
  bool is_empty_line = false;

  while ((getline(&line, &init_size, stream) != -1) && !feof(stream)) {

    is_empty_line = (line[0] == '\n' && line[1] == '\0'); 

    if (flags->sflag) {
      if (prev_was_empty && is_empty_line)
        continue;

      prev_was_empty = is_empty_line;
    }

    if ((flags->bflag && !is_empty_line) ||
        (!flags->bflag && flags->nflag)) {
      printf("\t%d  ", (*linecounter)++);
    }

    for (size_t i = 0; line[i] != '\0'; ++i) {
      if (flags->Eflag && line[i] == '\n')
        putchar('$');
      if(flags->Tflag && line[i] == '\t'){
        fputs("^I", stdout);
        continue;
      }

      putchar(line[i]);
    }
  }

  free(line);
}

int rcat(int argc, char **argv, RCatFlags *flags) {

  int linecounter = 1;

  if (optind == argc) {
    rcat_read_and_print_stream(stdin, flags, &linecounter);
    return 0;
  }

  for (int i = optind; i < argc; ++i) {
    if (strcmp(argv[i], "-") == 0) {
      rcat_read_and_print_stream(stdin, flags, &linecounter);
    } else {

      FILE *file_stream = fopen(argv[i], "r");

      if (!file_stream) {
        perror("Error opening file");
        return -1;
      }

      rcat_read_and_print_stream(file_stream, flags, &linecounter);

      fclose(file_stream);
    }
  }

  return 0;
}

int main(int argc, char **argv) {

  RCatFlags flags = {0};

  if (rcat_parse_flags_from_argv(&flags, argc, argv) != 0)
    return -1;

  if (rcat(argc, argv, &flags) != 0)
    return -1;

  return 0;
}
