#include <ctype.h>
#include <getopt.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// DONE: use getopt_long instead getopt
// DONE: add --help --version

static const char *HELP_TEXT =
    "Usage: rhead [OPTION]... [FILE]...\n"
    "Print the first 10 lines of each FILE to standard output.\n"
    "With more than one FILE, precede each with a header giving the file "
    "name.\n"
    "\nWith no FILE, or when FILE is -, read standard input.\n"
    "Mandatory arguments to long options are mandatory for short options too.\n"
    "\nThe options below may be used to select which counts are printed.\n"
    "-c, --bytes=NUM         print the first NUM bytes of each file;\n"
    "-n, --lines=NUM         print the first NUM lines instead of the first "
    "10;\n"
    "-q, --quiet             never print headers giving file names\n"
    "-v, --verbose           always print headers giving file names\n"
    "-z, --zero-terminated   line delimiter is NUL, not newline\n"
    "    --help              display this help and exit\n"
    "    --version           output version information and exit";

static const char *VERSION_TEXT = "rhead (example implementation) 0.1.0\n"
                                  "Written by nwpm.";

typedef struct RHeadOptions {
  bool cflag;
  const char *cflag_param;

  bool nflag;
  const char *nflag_param;

  bool qflag;
  bool vflag;
} RHeadOptions;

int rhead_parse_options_from_argv(const struct option *long_options,
                                  RHeadOptions *flags, int argc, char **argv) {

  int c;

  while ((c = getopt_long(argc, argv, "c:n:qv", long_options, NULL)) != -1) {
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
    case 'h':
      puts(HELP_TEXT);
      return 1;
    case 'w':
      puts(VERSION_TEXT);
      return 1;
    case '?':
      fprintf(stderr, "Try rhead --help for more information\n");
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

    if ((getline(&line, &init_size, stream) == -1) || feof(stream)) {
      free(line);
      return;
    }

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
    if (flags->vflag) {
      printf("===> standart input <===\n");
    }
    rhead_read_and_print_stream(stdin, flags);
    return 0;
  }

  bool print_header = ((argc - optind) > 1) ? true : false;

  for (int i = optind; i < argc; ++i) {
    if (strcmp(argv[i], "-") == 0) {
      if ((flags->vflag || print_header) && !flags->qflag) {
        printf("===> standart input <===\n");
      }

      rhead_read_and_print_stream(stdin, flags);

    } else {

      FILE *file_stream = fopen(argv[i], "r");

      if (!file_stream) {
        perror("Error opening file");
        return -1;
      }

      if ((flags->vflag || print_header) && !flags->qflag) {
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

  struct option long_options[] = {{"bytes", required_argument, NULL, 'c'},
                                  {"lines", required_argument, NULL, 'n'},
                                  {"quiet", no_argument, NULL, 'q'},
                                  {"verbose", no_argument, NULL, 'v'},
                                  {"help", no_argument, NULL, 'h'},
                                  {"version", no_argument, NULL, 'w'}};

  int parse_options_res =
      rhead_parse_options_from_argv(long_options, &flags, argc, argv);

  if (parse_options_res == -1)
    return -1;
  else if (parse_options_res == 1)
    return 0;

  if (rhead(&flags, argc, argv) != 0)
    return -1;

  return 0;
}
