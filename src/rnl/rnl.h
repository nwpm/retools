#ifndef RETOOLS_RNL_H
#define RETOOLS_RNL_H

#include <stddef.h>
#include <stdbool.h>

#define RNL_DEFAULT_BUF_CAPACITY 4096

typedef struct RNlOptions {
  bool bflag;
  bool fflag;
  bool hflag;
  bool dflag;
  bool nflag;
  const char* separator_str;
  bool wflag;
  int start_line_position;
  int line_step;
  bool lflag;
} RNlOptions;

int rnl_parse_flags_from_argv(RNlOptions *flags, int argc, char **argv);
int rnl(int argc, char** argv, RNlOptions *flags);

#endif // RETOOLS_RNL_H

