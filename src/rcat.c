#include <getopt.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// TODO: 'w' in options
// TODO: 'v' flag

static const char *HELP_TEXT =
    "Usage: rcat [OPTION]... [FILE]...\n"
    "Concatenate FILE(s) to standard output.\n"
    "\nWith no FILE, or when FILE is -, read standard input.\n"
    "-A, --show-all           equivalent to -vET\n"
    "-b, --number-nonblank    number nonempty output lines, overrides -n\n"
    "-e                       equivalent to -vE\n"
    "-E, --show-ends          display $ at end of each line\n"
    "-n, --number             number all output lines\n"
    "-s, --squeeze-blank      suppress repeated empty output lines\n"
    "-t                       equivalent to -vT\n"
    "-T, --show-tabs          display TAB characters as ^I\n"
    "-v, --show-nonprinting   use ^ and M- notation, except for LFD and TAB\n"
    "    --help        display this help and exit\n"
    "    --version     output version information and exit";

static const char *VERSION_TEXT = "rcat (example implementation) 0.1.0\n"
                                  "Written by nwpm.";

typedef struct RCatFlags {
  bool Eflag;
  bool nflag;
  bool bflag;
  bool vflag;
  bool Tflag;
  bool sflag;
} RCatFlags;

static int rcat_parse_flags_from_argv(const struct option *long_options,
                                      RCatFlags *flags, int argc, char **argv) {

  int c;

  while ((c = getopt_long(argc, argv, "nbesvtTEA", long_options, NULL)) != -1) {
    switch (c) {
    case 'A':
      flags->vflag = true;
      flags->Eflag = true;
      flags->Tflag = true;
      break;
    case 'e':
      flags->vflag = true;
      flags->Eflag = true;
      break;
    case 't':
      flags->vflag = true;
      flags->Tflag = true;
      break;
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
    case 'h':
      puts(HELP_TEXT);
      return 1;
    case 'w':
      puts(VERSION_TEXT);
      return 1;
    case '?':
      fprintf(stderr, "Try rcat --help for more information\n");
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

    if ((flags->bflag && !is_empty_line) || (!flags->bflag && flags->nflag)) {
      printf("\t%d  ", (*linecounter)++);
    }

    for (size_t i = 0; line[i] != '\0'; ++i) {
      if (flags->Eflag && line[i] == '\n') {
        putchar('$');
        putchar('\n');
        continue;
      }
      if (flags->Tflag && line[i] == '\t') {
        fputs("^I", stdout);
        continue;
      }
      if (flags->vflag) {
        if (line[i] == 0x7F) {
          fputs("^?", stdout);
          continue;
        } else if (line[i] < 0x1F && line[i] != '\n' && line[i] != '\t') {
          putchar('^');
          putchar(line[i] + 0x40);
          continue;
        }
      }

      putchar(line[i]);
    }
  }

  free(line);
}

static int rcat(int argc, char **argv, RCatFlags *flags) {

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

  struct option long_options[] = {
      {"show-all", no_argument, NULL, 'A'},
      {"number-nonblank", no_argument, NULL, 'b'},
      {"show-ends", no_argument, NULL, 'E'},
      {"number", no_argument, NULL, 'n'},
      {"squeeze-blank", no_argument, NULL, 's'},
      {"show-tabs", no_argument, NULL, 'T'},
      {"show-nonprinting", no_argument, NULL, 'v'},
      {"help", no_argument, NULL, 'h'},
      {"version", no_argument, NULL, 'w'},
  };

  int parse_res = rcat_parse_flags_from_argv(long_options, &flags, argc, argv);

  if (parse_res == -1) {
    return -1;
  } else if (parse_res == 1) {
    return 0;
  }

  if (rcat(argc, argv, &flags) != 0)
    return -1;

  return 0;
}
