#include <ctype.h>
#include <getopt.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const char *HELP_TEXT =
    "Usage: rtail [OPTION]... [FILE]...\n"
    "Print the last 10 lines of each FILE to standard output.\n"
    "With more than one FILE, precede each with a header giving the file "
    "name.\n"
    "\nWith no FILE, or when FILE is -, read standard input.\n"
    "Mandatory arguments to long options are mandatory for short options too.\n"
    "\nThe options below may be used to select which counts are printed.\n"
    "-c, --bytes=NUM         print the last NUM bytes of each file;\n"
    "-n, --lines=NUM         print the last NUM lines instead of the last "
    "10;\n"
    "-q, --quiet             never print headers giving file names\n"
    "-v, --verbose           always print headers giving file names\n"
    "    --help              display this help and exit\n"
    "    --version           output version information and exit";

static const char *VERSION_TEXT = "rtail (example implementation) 0.1.0\n"
                                  "Written by nwpm.";

typedef struct RTailOptions {
  bool cflag;
  const char *cflag_param;

  bool nflag;
  const char *nflag_param;

  bool qflag;
  bool vflag;
} RTailOptions;

typedef struct RBuffer {
  size_t size;
  size_t capacity;
  char *data;
} RBuffer;

#define RTAIL_DEFAULT_BUF_CAPACITY 1024

int rtail_parse_options_from_argv(const struct option *long_options,
                                  RTailOptions *flags, int argc, char **argv) {

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
      fprintf(stderr, "Try rtail --help for more information\n");
      return -1;
    }
  }

  return 0;
}

static int rtail_get_argument_param(const char *param) {

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

static int rtail_ensure_buf_capacity(RBuffer *input_buf) {

  char *new_raw_buffer = realloc(input_buf->data, input_buf->capacity * 2);

  if (!new_raw_buffer)
    return -1;

  input_buf->capacity *= 2;

  return 0;
}

RBuffer *rtail_create_input_buf() {

  RBuffer *input_buf = malloc(sizeof(RBuffer));

  if (!input_buf)
    return NULL;

  input_buf->size = 0;
  input_buf->capacity = RTAIL_DEFAULT_BUF_CAPACITY;

  char *raw_input = malloc(input_buf->capacity);

  if (!raw_input)
    return NULL;

  input_buf->data = raw_input;

  return input_buf;
}

void rtail_input_buf_free(RBuffer *input_buf) {

  if (!input_buf)
    return;

  free(input_buf->data);
  free(input_buf);
}

static int rtail_read_in_buffer(FILE* stream, RBuffer* input_buf){

  int c;
  size_t i = 0;

  // Read stream in Buffer
  while ((c = getc(stream)) != EOF) {
    if (input_buf->size + 1 > input_buf->capacity) {
      if (rtail_ensure_buf_capacity(input_buf) != 0)
        return -1;
    }

    input_buf->data[i++] = (char)c;
    input_buf->size++;
  }

  // Check is last char is '\n'
  if (input_buf->size > 0 && (input_buf->data[input_buf->size - 1] != '\n')) {
    if (input_buf->size + 1 > input_buf->capacity) {
      if (rtail_ensure_buf_capacity(input_buf) != 0)
        return -1;
    }

    input_buf->data[input_buf->size] = '\n';
    input_buf->size++;
  }

  return 0;
}

static int rtail_read_and_print_stream(FILE *stream, RTailOptions *flags,
                                       RBuffer *input_buf) {

  int print_number = 0;

  if (flags->cflag) {
    print_number = rtail_get_argument_param(flags->cflag_param);

    if (print_number == -1)
      return -1;

    if(rtail_read_in_buffer(stream, input_buf) != 0)
      return -1;

    if (print_number > input_buf->size) {
      for (size_t i = 0; i < input_buf->size; ++i) {
        putc(input_buf->data[i], stdout);
      }
    } else {
      for (size_t i = input_buf->size - print_number; i < input_buf->size;
           ++i) {
        putc(input_buf->data[i], stdout);
      }
    }

  } else {
    print_number =
        flags->nflag ? rtail_get_argument_param(flags->nflag_param) : 10;

    if (print_number == -1)
      return -1;

    if(rtail_read_in_buffer(stream, input_buf) != 0)
      return -1;

    // Count '\n'

    size_t start_index = 0;
    size_t newlines = 0;
    size_t last_newline_pos = 0;

    for (size_t i = input_buf->size; i-- > 0;) {
      if (input_buf->data[i] == '\n') {
        newlines++;
        if (newlines == print_number + 1) {
          start_index = i + 1;
          break;
        }
      }
    }

    if (newlines <= print_number) {
      start_index = 0;
    }

    for (size_t i = start_index; i < input_buf->size; ++i) {
      putc(input_buf->data[i], stdout);
    }
  }

  input_buf->size = 0;

  return 0;
}

int rtail(RTailOptions *flags, int argc, char **argv) {

  int exit_status = 0;

  // TODO: use last flag like GNU version
  if (flags->cflag && flags->nflag) {
    fprintf(stderr, "you can't use -c and -n flags together\n");
    return -1;
  }

  RBuffer *input_buf = rtail_create_input_buf();

  if (!input_buf)
    return -1;

  if (optind == argc) {
    if (flags->vflag) {
      printf("===> standart input <===\n");
    }

    exit_status = rtail_read_and_print_stream(stdin, flags, input_buf);
    goto cleanup;
  }

  bool print_header = ((argc - optind) > 1) ? true : false;

  for (int i = optind; i < argc; ++i) {
    if (strcmp(argv[i], "-") == 0) {

      if ((flags->vflag || print_header) && !flags->qflag) {
        printf("===> standart input <===\n");
      }

      if (rtail_read_and_print_stream(stdin, flags, input_buf) != 0) {
        exit_status = -1;
        goto cleanup;
      }

    } else {

      FILE *file_stream = fopen(argv[i], "r");

      if (!file_stream) {
        perror("Error opening file");
        exit_status = -1;
        goto cleanup;
      }

      if ((flags->vflag || print_header) && !flags->qflag) {
        printf("===> %s <===\n", argv[i]);
      }

      if (rtail_read_and_print_stream(file_stream, flags, input_buf) != 0) {
        exit_status = -1;
        fclose(file_stream);
        goto cleanup;
      }

      fclose(file_stream);
    }
  }

  goto cleanup;

cleanup:
  rtail_input_buf_free(input_buf);
  return exit_status;
}

int main(int argc, char **argv) {

  RTailOptions flags = {0};

  struct option long_options[] = {{"bytes", required_argument, NULL, 'c'},
                                  {"lines", required_argument, NULL, 'n'},
                                  {"quiet", no_argument, NULL, 'q'},
                                  {"verbose", no_argument, NULL, 'v'},
                                  {"help", no_argument, NULL, 'h'},
                                  {"version", no_argument, NULL, 'w'}};

  int parse_options_res =
      rtail_parse_options_from_argv(long_options, &flags, argc, argv);

  if (parse_options_res == -1)
    return -1;
  else if (parse_options_res == 1)
    return 0;

  if (rtail(&flags, argc, argv) != 0)
    return -1;

  return 0;
}
