#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>

// TODO: flags support
// TODO: build from Make

typedef struct RtwFlags {
  bool cflag;
  bool mflag;
  bool lflag;
  bool wflag;
} RtwFlags;

struct RtwResult {
  size_t bytes;
  size_t newlines;
  size_t words;
} result;

int rtw_parse_flags_from_argv(RtwFlags *flags, int argc, char **argv) {

  int c;

  while ((c = getopt(argc, argv, "cmlw")) != -1) {
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
    case '?':
      fprintf(stderr, "Try rtw --help for more information\n");
      return -1;
    }
  }

  return 0;
}

static void rtw_read_and_print_stream(FILE *stream, RtwFlags *flags) {

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

    if(c == '\n')
      newlines++;

    bytes++;
  }

  printf("\t%zu\t%zu\t%zu  ", newlines, words, bytes);
  
  result.newlines += newlines;
  result.words += words;
  result.bytes += bytes;
}

int rtw(int argc, char **argv, RtwFlags *flags) {

  if (optind == argc) {
    rtw_read_and_print_stream(stdin, flags);
    return 0;
  }

  for (int i = optind; i < argc; ++i) {
    if (strcmp(argv[i], "-") == 0) {
      rtw_read_and_print_stream(stdin, flags);
      printf(":%s\n", "stdin");
    } else {

      FILE *file_stream = fopen(argv[i], "r");

      if (!file_stream) {
        perror("Error opening file");
        return -1;
      }

      rtw_read_and_print_stream(file_stream, flags);
      printf(":%s\n", argv[i]);

      fclose(file_stream);
    }
  }

  printf("\t%zu\t%zu\t%zu  :total\n", result.newlines, result.words, result.bytes);

  return 0;
}

int main(int argc, char **argv) {

  RtwFlags flags = {0};

  if (rtw_parse_flags_from_argv(&flags, argc, argv) != 0)
    return -1;

  if (rtw(argc, argv, &flags) != 0)
    return -1;

  return 0;
}

