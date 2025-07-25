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
      flags->Eflag = 1;
      break;
    case 'n':
      flags->nflag = 1;
      break;
    case 'b':
      flags->bflag = 1;
      break;
    case 'v':
      flags->vflag = 1;
      break;
    case 'T':
      flags->Tflag = 1;
      break;
    case 's':
      flags->sflag = 1;
      break;
    case '?':
      fprintf(stderr, "Try rtw --help for more information\n");
      return -1;
    }
  }

  return 0;
}

static int rcat_read_and_print_stream(FILE *stream, RCatFlags *flags,
                                      int *linecounter) {

  char *line = NULL;
  size_t init_size = 0;
  int prev_was_empty = 0;
  int is_empty_line = 0;

  // TODO: feof error check
  while (getline(&line, &init_size, stream) != -1) {

    is_empty_line = (line[0] == '\n' && line[1] == '\0'); 

    if (flags->sflag) {
      if (prev_was_empty && is_empty_line)
        continue;

      prev_was_empty = is_empty_line;
    }

    // TODO: remove printf and use fwrite
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

  return 0;
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

      //TODO: remove fprintf and use perror
      if (!file_stream) {
        fprintf(stderr, "%s : no such file or directory\n", argv[i]);
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
