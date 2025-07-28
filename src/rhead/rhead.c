#include "rhead.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// TODO: make -v -z --help flags functions

int rhead_parse_options_from_argv(RHeadOptions *flags, int argc, char **argv) {

  int c;

  while ((c = getopt(argc, argv, "c:n:qv")) != -1) {
    switch (c) {
    case 'c':
      flags->cflag = true;
      flags->cflag_param = optarg;
      break;
    case 'n':
      flags->nflag = true;
      flags->nflag_param = optarg;
      break;
    case 'q':
      flags->qflag = true;
      break;
    case 'v':
      flags->vflag = true;
      break;
    case '?':
      fprintf(stderr, "Try rtw --help for more information\n");
      return -1;
    }
  }

  return 0;
}

static int rhead_get_argument_param(const char *param) {

  size_t len = strlen(param);
  int multiply = 1;
  int res = 0;

  for (size_t i = len; i-- > 0;) {
    if (!isdigit(param[i])) {
      fprintf(stderr, "invalid number of lines '%s'\n", param);
      return -1;
    }

    res += (param[i] - '0') * multiply;
    multiply *= 10;
  }

  return res;
}

static void rhead_read_and_print_stream(FILE *stream, RHeadOptions *flags) {

  int print_number = 0;

  if (flags->cflag) {
    
    print_number = rhead_get_argument_param(flags->cflag_param);
    char c;
    
    for (int i = 0; i < print_number; ++i) {
      c = getc(stream);
      putchar(c);
    }

    return;

  } else {
    print_number =
        flags->nflag ? rhead_get_argument_param(flags->nflag_param) : 10;
  }

  if (print_number == -1)
    return;

  char *line = NULL;
  size_t init_size = 0;

  for (int i = 0; i < print_number; ++i) {
    
    if ((getline(&line, &init_size, stream) == -1) || feof(stream)){
      //TODO: goto?
      free(line);
      return;
    }

    // TODO: use fwrite
    printf("%s", line);
  }

  printf("\n");
  free(line);
}

int rhead(RHeadOptions *flags, int argc, char **argv) {

  // TODO: use last flag like GNU version
  if (flags->cflag && flags->nflag) {
    fprintf(stderr, "you can't use -c and -n flags together\n");
    return -1;
  }

  if (optind == argc) {
    // TODO: print stream header
    rhead_read_and_print_stream(stdin, flags);
    return 0;
  }

  for (int i = optind; i < argc; ++i) {
    if (strcmp(argv[i], "-") == 0) {

      if (!flags->qflag) {
        printf("===> standart input <===\n");
      }

      rhead_read_and_print_stream(stdin, flags);

    } else {

      FILE *file_stream = fopen(argv[i], "r");

      if (!file_stream) {
        perror("Error opening file");
        return -1;
      }

      if (!flags->qflag) {
        printf("===> %s <===\n", argv[i]);
      }

      rhead_read_and_print_stream(file_stream, flags);

      fclose(file_stream);
    }
  }

  return 0;
}

int main(int argc, char **argv) {

  RHeadOptions flags = {0};

  if (rhead_parse_options_from_argv(&flags, argc, argv) != 0)
    return -1;

  if (rhead(&flags, argc, argv) != 0)
    return -1;

  return 0;
}
