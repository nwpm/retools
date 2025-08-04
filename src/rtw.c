#include <ctype.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// TODO: build from Make
// TODO: suppert wchar
// TODO: version and help in .po

static const char *HELP_TEXT =
    "Usage: rtw [OPTION]... [FILE]...\n"
    "Print newline, word, and byte counts for each FILE, and a total line if\n"
    "use flag '-t' or '--total'.  A word is a nonempty sequence of non "
    "white\n"
    "space delimited by white space characters or by start or end of input.\n"
    "\nWith no FILE, or when FILE is -, read standard input.\n"
    "\nThe options below may be used to select which counts are printed.\n"
    "-c, --bytes          print the byte counts\n"
    "-m, --chars          print the character counts\n"
    "-l, --lines          print the newline counts\n"
    "-w, --words          print the word counts\n"
    "-t, --total          print total info\n"
    "    --help           display this help and exit\n"
    "    --version        output version information and exit";

static const char *VERSION_TEXT = "rtw (example implementation) 0.1.0\n"
                                  "Written by nwpm.";

typedef struct RtwOptions {
  bool cflag;
  bool mflag;
  bool lflag;
  bool wflag;
  bool tflag;
} RtwOptions;

struct RtwResult {
  size_t bytes;
  size_t newlines;
  size_t words;
} result;

int rtw_parse_flags_from_argv(RtwOptions *flags, struct option *long_options,
                              int argc, char **argv) {

  int c;

  while ((c = getopt_long(argc, argv, "cmlwt", long_options, NULL)) != -1) {
    switch (c) {
    case 'c':
      flags->cflag = true;
      break;
    case 'm':
      flags->mflag = true;
      break;
    case 'l':
      flags->lflag = true;
      break;
    case 'w':
      flags->wflag = true;
      break;
    case 't':
      flags->tflag = true;
      break;
    case 'v':
      fputs(VERSION_TEXT, stdout);
      return 1;
    case 'h':
      fputs(HELP_TEXT, stdout);
      return 1;
    case '?':
      fprintf(stderr, "Try rtw --help for more information\n");
      return -1;
    }
  }

  if (!(flags->cflag || flags->lflag || flags->mflag || flags->wflag)) {
    flags->cflag = true;
    flags->lflag = true;
    flags->mflag = true;
    flags->wflag = true;
  }

  return 0;
}

static void rtw_read_and_print_stream(FILE *stream, RtwOptions *flags) {

  int c;

  size_t newlines = 0;
  size_t words = 0;
  size_t bytes = 0;

  bool in_word = false;

  while ((c = fgetc(stream)) != EOF) {

    bool is_delim = isspace(c);

    if (is_delim && in_word) {
      in_word = false;
    } else if (!is_delim && !in_word) {
      in_word = true;
      words++;
    }

    if (c == '\n')
      newlines++;

    bytes++;
  }

  putchar('\n');
  if (flags->cflag) {
    printf("bytes:%zu  ", bytes);
  }
  if (flags->mflag) {
    printf("chars:%zu  ", bytes);
  }
  if (flags->lflag) {
    printf("newlines:%zu  ", newlines);
  }
  if (flags->wflag) {
    printf("words:%zu  ", words);
  }

  result.newlines += newlines;
  result.words += words;
  result.bytes += bytes;
}

int rtw(int argc, char **argv, RtwOptions *flags) {

  if (optind == argc) {
    rtw_read_and_print_stream(stdin, flags);
    putchar('\n');
    return 0;
  }

  for (int i = optind; i < argc; ++i) {
    if (strcmp(argv[i], "-") == 0) {
      rtw_read_and_print_stream(stdin, flags);
      printf("\t:%s\n", "stdin");
    } else {

      FILE *file_stream = fopen(argv[i], "r");

      if (!file_stream) {
        perror("Error opening file");
        return -1;
      }

      rtw_read_and_print_stream(file_stream, flags);
      printf("  :%s\n", argv[i]);

      fclose(file_stream);
    }
  }

  if (flags->tflag) {
    putchar('\n');
    if (flags->cflag) {
      printf("bytes:%zu  ", result.bytes);
    }
    if (flags->mflag) {
      printf("chars:%zu  ", result.bytes);
    }
    if (flags->lflag) {
      printf("newlines:%zu  ", result.newlines);
    }
    if (flags->wflag) {
      printf("words:%zu  ", result.words);
    }
    printf(":total\n");
  }

  return 0;
}

int main(int argc, char **argv) {

  RtwOptions flags = {0};

  struct option long_options[] = {{"bytes", no_argument, NULL, 'c'},
                                  {"chars", no_argument, NULL, 'm'},
                                  {"lines", no_argument, NULL, 'l'},
                                  {"words", no_argument, NULL, 'w'},
                                  {"help", no_argument, NULL, 'h'},
                                  {"total", required_argument, NULL, 't'},
                                  {"version", no_argument, NULL, 'v'}};

  int rtw_parse_res =
      rtw_parse_flags_from_argv(&flags, long_options, argc, argv);

  if (rtw_parse_res == -1)
    return -1;
  else if (rtw_parse_res == 1)
    return 0;

  if (rtw(argc, argv, &flags) != 0)
    return -1;

  return 0;
}
