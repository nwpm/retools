#include "rtw.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int rtw_set_flags(RTWFlags *flags, int argc, char **argv) {

  int c;

  while ((c = getopt(argc, argv, "cmlw")) != -1) {
    switch (c) {
    case 'c':
      flags->cflag = 1;
      break;
    case 'm':
      flags->mflag = 1;
      break;
    case 'l':
      flags->lflag = 1;
      break;
    case 'w':
      flags->wflag = 1;
      break;
    case '?':
      printf("Try rtw --help for more information\n");
      return -1;
    }
  }

  if (!(flags->cflag || flags->lflag || flags->mflag || flags->wflag)) {
    flags->cflag = 1;
    flags->lflag = 1;
    flags->mflag = 1;
    flags->wflag = 1;
  }

  return 0;
}

RTWBuffer *rtw_create_input_buf() {

  RTWBuffer *input_buf = malloc(sizeof(RTWBuffer));

  if (!input_buf)
    return NULL;

  input_buf->size = 0;
  input_buf->capacity = RTW_BUF_CAPACITY;

  char *raw_input = malloc(input_buf->capacity);

  if (!raw_input)
    return NULL;

  input_buf->buffer = raw_input;

  return input_buf;
}

int rtw_resize_buffer(RTWBuffer *input_buf) {

  char *new_raw_buffer = realloc(input_buf->buffer, input_buf->capacity * 2);

  if (!new_raw_buffer) {
    return -1;
  }

  return 0;
}

int rtw_resize_buf_arr(RTWBufArray *buf_arr) {

  RTWBuffer **new_arr_data = realloc(buf_arr->data, buf_arr->capacity * 2);

  if (!new_arr_data) {
    return -1;
  }

  return 0;
}

RTWBufArray *rtw_create_buffer_arr() {

  RTWBufArray *buf_arr = malloc(sizeof(RTWBufArray));

  if (!buf_arr)
    return NULL;

  RTWBuffer **data = malloc(sizeof(RTWBuffer *) * RTW_BUF_ARR_CAPACITY);

  if (!data)
    return NULL;

  buf_arr->size = 0;
  buf_arr->capacity = RTW_BUF_ARR_CAPACITY;
  buf_arr->data = data;

  return buf_arr;
}

RTWBuffer *rtw_read_stream(FILE *file_stream) {

  RTWBuffer *input_buf = rtw_create_input_buf();

  if (!input_buf)
    return NULL;

  int c;
  int i = 0;

  while ((c = fgetc(file_stream)) != EOF) {
    if (input_buf->size + 1 > input_buf->capacity) {
      if (rtw_resize_buffer(input_buf) != 0)
        return NULL;
    }

    input_buf->buffer[i++] = (char)c;
    input_buf->size++;
  }

  return input_buf;
}

int rtw_read_from_stdin(RTWBufArray *buf_arr) {

  buf_arr->data[0] = rtw_read_stream(stdin);
  buf_arr->size++;

  return (buf_arr->data[0] == NULL) ? -1 : 0;
}

int rtw_read_from_files(RTWBufArray *buf_arr, int argc, char **argv) {

  size_t j = 0;
  for (int i = optind; i < argc; ++i) {

    if (buf_arr->size + 1 > buf_arr->capacity) {
      if (rtw_resize_buf_arr(buf_arr) != 0)
        return -1;
    }

    FILE *file_stream = fopen(argv[i], "r");

    if (!file_stream)
      return -1;

    buf_arr->data[j] = rtw_read_stream(file_stream);
    buf_arr->size++;

    if (!buf_arr->data[j]) {
      fclose(file_stream);
      return -1;
    }

    ++j;
    fclose(file_stream);
  }

  return 0;
}

size_t rtw_count_newlines(const RTWBuffer *input_buf) {

  size_t res = 0;

  for (size_t i = 0; i < input_buf->size; ++i) {
    if (input_buf->buffer[i] == '\n')
      res++;
  }

  return res;
}

size_t rtw_count_words(const RTWBuffer *input_buf) {

  size_t res = 0;
  int in_word = 0;

  for (size_t i = 0; i < input_buf->size; ++i) {
    char c = input_buf->buffer[i];
    int c_is_space = isspace(c);
    if (c_is_space && in_word) {
      in_word = 0;
    } else if (!c_is_space && !in_word) {
      in_word = 1;
      res++;
    }
  }

  return res;
}

RTWResult *rtw_create_result() {

  RTWResult *res = malloc(sizeof(RTWResult));

  if (!res)
    return NULL;

  res->bytes = 0;
  res->chars = 0;
  res->newlines = 0;
  res->words = 0;

  return res;
}

RTWResultArray *rtw_create_result_arr() {

  RTWResultArray *res_arr = malloc(sizeof(RTWResultArray));

  if (!res_arr)
    return NULL;

  RTWResult **data = malloc(sizeof(RTWResult *) * RTW_BUF_ARR_CAPACITY);

  if (!data)
    return NULL;

  res_arr->size = 0;
  res_arr->capacity = RTW_BUF_ARR_CAPACITY;
  res_arr->data = data;

  return res_arr;
}

int rtw_resize_res_arr(RTWResultArray *res_arr) {

  RTWResult **new_arr_data = realloc(res_arr->data, res_arr->capacity * 2);

  if (!new_arr_data) {
    return -1;
  }

  return 0;
}

int rtw_parse_input_buff(RTWBufArray *buf_arr, const RTWFlags *flags,
                         RTWResultArray *res_arr) {

  for (size_t i = 0; i < buf_arr->size; ++i) {

    if (res_arr->size + 1 > res_arr->capacity) {
      if (rtw_resize_res_arr(res_arr) != 0)
        return -1;
    }

    RTWResult *res = rtw_create_result();

    if (!res)
      return -1;

    if (flags->cflag) {
      res->chars = buf_arr->data[i]->size;
    }
    if (flags->lflag) {
      res->newlines = rtw_count_newlines(buf_arr->data[i]);
    }
    if (flags->mflag) {
      res->bytes = buf_arr->data[i]->size;
    }
    if (flags->wflag) {
      res->words = rtw_count_words(buf_arr->data[i]);
    }

    res_arr->data[i] = res;
    res_arr->size++;
  }

  return 0;
}

void rtw_buf_arr_free(RTWBufArray *buf_array) {

  for (size_t i = 0; i < buf_array->size; ++i) {
    free(buf_array->data[i]->buffer);
    free(buf_array->data[i]);
  }

  free(buf_array);
}

void rtw_res_arr_free(RTWResultArray *res_array) {

  for (size_t i = 0; i < res_array->size; ++i) {
    free(res_array->data[i]);
  }

  free(res_array);
}

void rtw_print_result(RTWResultArray *res_arr) {

  for (size_t i = 0; i < res_arr->size; ++i) {
    if (res_arr->data[i]->bytes)
      printf("\n%zu bytes: %zu", i + 1, res_arr->data[i]->bytes);
    if (res_arr->data[i]->chars)
      printf("\t%zu chars: %zu", i + 1, res_arr->data[i]->chars);
    if (res_arr->data[i]->newlines)
      printf("\t%zu newlines: %zu", i + 1, res_arr->data[i]->newlines);
    if (res_arr->data[i]->words)
      printf("\t%zu words: %zu", i + 1, res_arr->data[i]->words);
    printf("\n");
  }
}

int main(int argc, char **argv) {

  RTWFlags flags = {0};

  if (rtw_set_flags(&flags, argc, argv) != 0)
    return -1;

  RTWBufArray *buf_array = rtw_create_buffer_arr();

  if (!buf_array)
    return -1;

  int read_status = 0;

  if (optind < argc) {
    read_status = rtw_read_from_files(buf_array, argc, argv);
  } else {
    read_status = rtw_read_from_stdin(buf_array);
  }

  if (read_status != 0) {
    rtw_buf_arr_free(buf_array);
    return -1;
  }

  RTWResultArray *res_arr = rtw_create_result_arr();

  if (!res_arr) {
    rtw_buf_arr_free(buf_array);
    return -1;
  }

  rtw_parse_input_buff(buf_array, &flags, res_arr);

  rtw_print_result(res_arr);

  rtw_buf_arr_free(buf_array);

  return 0;
}
