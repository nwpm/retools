#include "rtw.h"
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

#include <stdio.h>

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

  if(!flags->cflag && !flags->lflag && !flags->mflag && !flags->wflag){
    flags->cflag = 1;
    flags->lflag = 1;
    flags->mflag = 1;
    flags->wflag = 1;
  }

  return 0;
}

int rtw_resize_buffer(RTWBuffer *input_buf){

  char* new_raw_buffer = realloc(input_buf->buffer, input_buf->capacity * 2);

  if(!new_raw_buffer){
    return -1;
  }

  return 0;
}

int rtw_read_from_stdin(RTWBuffer *input_buf) {

  int c;
  int i = 0;

  while ((c = fgetc(stdin)) != EOF) {
    if(input_buf->size + 1 > input_buf->capacity){
      if(rtw_resize_buffer(input_buf) != 0)
        return -1;
    }

    input_buf->buffer[i++] = (char)c;
    input_buf->size++;
  }

  return 0;
}

int rtw_parse_input_buff(const RTWBuffer *input_buf, const RTWFlags *flags, RTWResult *res){

  if(flags->cflag){
    res->chars = input_buf->size;
  }
  if(flags->lflag){
    //res->newlines = rtw_count_newlines(input_buf);
  }
  if(flags->mflag){
    res->bytes = input_buf->size;
  }
  if(flags->wflag){
    //res->words = rtw_count_words(input_buf);
  }

  return 0;
}

void rtw_print_result(RTWResult *res){
  printf("\n%zu", res->bytes);
  printf("\t%zu", res->newlines);
  printf("\t%zu", res->words);
  printf("\n");
}

int main(int argc, char **argv) {

  RTWFlags flags = {0};

  if (rtw_set_flags(&flags, argc, argv) != 0)
    return -1;

  RTWBuffer input_buf = {
      .size = 0, .capacity = RTW_START_BUFF_CAPACITY, .buffer = NULL};

  char *raw_input = malloc(input_buf.capacity);

  if (!raw_input)
    return -1;

  input_buf.buffer = raw_input;

  int read_status = 0;

  if (optind < argc) {
    // read_status = rtw_read_from_files(&input_buf);
  } else {
    read_status = rtw_read_from_stdin(&input_buf);
  }

  if (read_status != 0){
    free(input_buf.buffer);
    return -1;
  }

  RTWResult res = {0};
  rtw_parse_input_buff(&input_buf, &flags, &res);

  rtw_print_result(&res);

  free(input_buf.buffer);

  return 0;
}
