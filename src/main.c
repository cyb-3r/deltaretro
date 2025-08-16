#include <stdio.h>

#include "raylib.h"

#include "system.h"

enum exit_code {
  ERR = -1,
  OK,
};

int main(void) {
  #ifdef DEBUG
    SetTraceLogLevel(LOG_DEBUG);
    TraceLog(LOG_DEBUG, "Debug mode is ON");
  #endif

  sys_t sys = EMPTY;
  const bool ok = system_init(&sys);
  if (!ok) {
    TraceLog(LOG_FATAL, "Failed to init system");
    return ERR;
  }

  while (sys.state != SYS_EXIT && !WindowShouldClose()) {
    switch(sys.state) {
      case SYS_TITLE: title_scr(&sys); break;
      case SYS_MENU: main_menu(&sys);  break;
      case SYS_TEST: test_loop(&sys);  break;

      default:
      TraceLog(LOG_FATAL, "Undefined system state %d", sys.state);
      sys.state = SYS_EXIT;
      break;
    }
  }

  system_deinit(&sys);
  return OK;
}
