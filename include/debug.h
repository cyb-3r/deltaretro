#ifndef DEBUG_H
#define DEBUG_H

#include "types.h"

struct debug {
  bool pause;
  bool frmskp;
  bool cooldwn;
};
typedef struct debug dbg_t;

void dbg_step(dbg_t*);

#endif // DEBUG_H
