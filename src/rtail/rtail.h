#ifndef RETOOLS_RTAIL_H
#define RETOOLS_RTAIL_H

#include <stdbool.h>
#include <stddef.h>

typedef struct RTailOptions {
  bool cflag;
  const char* cflag_param;

  bool nflag;
  const char* nflag_param;
  
  bool qflag;
  bool vflag;
} RTailOptions;

typedef struct RBuffer {
  size_t size;
  size_t capacity;
  char* data;
} RBuffer;

int rtail_parse_options_from_argv(RTailOptions *flags, int argc, char **argv);
int rtail(RTailOptions *flags, int argc, char** argv);

#endif
