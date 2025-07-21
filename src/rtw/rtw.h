#ifndef RETOOLS_RTW_H
#define RETOOLS_RTW_H

#include <stddef.h>

#define RTW_START_BUFF_CAPACITY 1024

typedef struct RTWResult {
  size_t bytes;
  size_t chars;
  size_t newlines;
  size_t words;
} RTWResult;

typedef struct RTWFlags {
  int cflag;
  int mflag;
  int lflag;
  int wflag;
} RTWFlags;

typedef struct RTWBuffer {
  size_t size;
  size_t capacity;
  char *buffer;
} RTWBuffer;

size_t rtw_count_chars(const RTWBuffer *input_buf);
size_t rtw_count_newlines(const RTWBuffer *input_buf);
size_t rtw_count_words(const RTWBuffer *input_buf);

int rtw_read_from_stdin(RTWBuffer *input_buf);
int rtw_read_from_files(RTWBuffer *input_buf);
int rtw_resize_buf(RTWBuffer *input_buf);
int rtw_parse_input_buf(const RTWBuffer *input_buf, const RTWFlags *flags,
                        RTWResult *res);
int rtw_set_flags(RTWFlags *flags, int argc, char **argv);

void rtw_print_result(RTWResult *res);

#endif // RETOOLS_RTW_H
