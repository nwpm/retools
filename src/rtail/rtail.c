#include "rtail.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define RTAIL_DEFAULT_BUF_CAPACITY 1024

// TODO: make -v -z --help flags functions

int rtail_parse_options_from_argv(RTailOptions *flags, int argc, char **argv) {

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

static int rtail_read_and_print_stream(FILE *stream, RTailOptions *flags,
                                       RBuffer *input_buf) {

  int print_number = 0;

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

  if (flags->cflag) {
    print_number = rtail_get_argument_param(flags->cflag_param);

    if (print_number == -1)
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
    if (!flags->qflag) {
      printf("===> standart input <===\n");
    }

    exit_status = rtail_read_and_print_stream(stdin, flags, input_buf);
    goto cleanup;
  }

  for (int i = optind; i < argc; ++i) {
    if (strcmp(argv[i], "-") == 0) {

      if (!flags->qflag) {
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

      if (!flags->qflag) {
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

  if (rtail_parse_options_from_argv(&flags, argc, argv) != 0)
    return -1;

  if (rtail(&flags, argc, argv) != 0)
    return -1;

  return 0;
}
