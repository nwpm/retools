#ifndef RETOOLS_RCAT_H
#define RETOOLS_RCAT_H

#include <stddef.h>

#define RCAT_DEFAULT_BUF_CAPACITY 4096

typedef struct RCatFlags {
  int Eflag;
  int nflag;
  int bflag;
  int vflag;
  int Tflag;
  int sflag;
} RCatFlags;

int rcat_parse_flags_from_argv(RCatFlags *flags, int argc, char **argv);
int rcat(int argc, char** argv, RCatFlags *flags);

#endif // RETOOLS_RCAT_H
