#ifndef RETOOLS_RCAT_H
#define RETOOLS_RCAT_H

#include <stddef.h>
#include <stdbool.h>

#define RCAT_DEFAULT_BUF_CAPACITY 4096

typedef struct RCatFlags {
  bool Eflag;
  bool nflag;
  bool bflag;
  bool vflag;
  bool Tflag;
  bool sflag;
} RCatFlags;

int rcat_parse_flags_from_argv(RCatFlags *flags, int argc, char **argv);
int rcat(int argc, char** argv, RCatFlags *flags);

#endif // RETOOLS_RCAT_H
