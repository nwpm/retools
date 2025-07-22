#ifndef RETOOLS_RTW_H
#define RETOOLS_RTW_H

#include <stddef.h>
#include <stdio.h>

#define RTW_BUF_CAPACITY 1024
#define RTW_BUF_ARR_CAPACITY 4

typedef struct RTWFlags {
  int cflag;
  int mflag;
  int lflag;
  int wflag;
} RTWFlags;

typedef struct RTWResult {
  size_t bytes;
  size_t chars;
  size_t newlines;
  size_t words;
} RTWResult;

typedef struct RTWResultArray{
  size_t size;
  size_t capacity;
  RTWResult** data;
} RTWResultArray;

typedef struct RTWBuffer {
  size_t size;
  size_t capacity;
  char *buffer;
} RTWBuffer;

typedef struct RTWBufArray{
  size_t size;
  size_t capacity;
  RTWBuffer** data;
} RTWBufArray;

size_t rtw_count_newlines(const RTWBuffer *input_buf);
size_t rtw_count_words(const RTWBuffer *input_buf);

int rtw_read_from_stdin(RTWBufArray *buf_arr);
int rtw_read_from_files(RTWBufArray *buf_arr, int argc, char** argv);
int rtw_resize_buf(RTWBuffer *input_buf);
int rtw_parse_input_buf(RTWBufArray *buf_arr, const RTWFlags *flags,
                        RTWResultArray *res);
int rtw_set_flags(RTWFlags *flags, int argc, char **argv);
RTWBuffer* rtw_read_stream(FILE* file_stream);
int rtw_resize_buf_arr(RTWBufArray *buf_arr);
int rtw_resize_res_arr(RTWResultArray *res_arr);

RTWBuffer* rtw_create_input_buf();
RTWBufArray* rtw_create_buffer_arr();

RTWResultArray* rtw_create_result_arr();

void rtw_buf_arr_free(RTWBufArray *buf_array);
void rtw_res_arr_free(RTWResultArray *res_array);
void rtw_print_result(RTWResultArray *res_arr);

#endif // RETOOLS_RTW_H
