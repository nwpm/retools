#ifndef RETOOLS_RTW_H
#define RETOOLS_RTW_H

#include <stddef.h>
#include <stdbool.h>

#define RTW_DEFAULT_BUF_CAPACITY 1024
#define RTW_DEFAULT_BUF_AND_ARRAY_CAPACITY 4

typedef struct RTWFlags {
  bool cflag;
  bool mflag;
  bool lflag;
  bool wflag;
} RTWFlags;

typedef struct RTWResult {
  size_t bytes;
  size_t chars;
  size_t newlines;
  size_t words;
  char *file_name;
} RTWResult;

typedef struct RTWResultArray {
  size_t size;
  size_t capacity;
  RTWResult **data;
} RTWResultArray;

typedef struct RTWBuffer {
  size_t size;
  size_t capacity;
  char *buffer;
} RTWBuffer;

typedef struct RTWBufArray {
  size_t size;
  size_t capacity;
  RTWBuffer **data;
} RTWBufArray;

int rtw_read_from_stdin(RTWBufArray *buf_arr);
int rtw_read_from_files(RTWBufArray *buf_arr, int argc, char **argv);

RTWBufArray *rtw_create_buffer_arr();
RTWResultArray *rtw_create_result_arr();

int rtw_parse_input_buf(RTWBufArray *buf_arr, const RTWFlags *flags,
                        RTWResultArray *res, char **argv, int read_from_files);
int rtw_parse_flags_from_argv(RTWFlags *flags, int argc, char **argv);

void rtw_buf_arr_free(RTWBufArray *buf_array);
void rtw_res_arr_free(RTWResultArray *res_array);

#endif // RETOOLS_RTW_H
