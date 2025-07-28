#ifndef RETOOLS_RHEAD_H
#define RETOOLS_RHEAD_H

#include <stdbool.h>
#include <stddef.h>

typedef struct RHeadOptions {
  bool cflag;
  const char* cflag_param;

  bool nflag;
  const char* nflag_param;
  
  bool qflag;
  bool vflag;
} RHeadOptions;

int rhead_parse_options_from_argv(RHeadOptions *flags, int argc, char **argv);
int rhead(RHeadOptions *flags, int argc, char** argv);

#endif
