#include "debug.h"
#include "raylib.h"

#define DBG_PAUSE_KEY KEY_P
#define DBG_FRMSKP_KEY KEY_N

void dbg_step(dbg_t *self) {
  self->cooldwn = false;
  if (self->pause) {
    self->frmskp = false;

    switch (GetKeyPressed()) {
      case DBG_FRMSKP_KEY:
      self->frmskp = true;
      TraceLog(LOG_DEBUG, "RESUMING 1 FRAME");
      break;

      case DBG_PAUSE_KEY:
      self->pause = false;
      self->cooldwn = true;
      TraceLog(LOG_DEBUG, "RESUMING PROGRAM");
      break;

      default: break;
    }
  }

  if (IsKeyPressed(DBG_PAUSE_KEY) && !self->cooldwn) {
    self->pause = true;
    TraceLog(LOG_DEBUG, "PAUSING PROGRAM");
  }
}
