#include <stdio.h>

#include "raylib.h"

#include "sysmode-title.h"
#include "sysmode-file.h"
#include "sysmode-game.h"

enum exit_code {
  ERR = -1,
  OK = 0,
};

void begin(sys_t*);
void update(sys_t*);
void draw(sys_t*);
void end(sys_t*);

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

  while ((sys.state != SYS_EXIT) && !WindowShouldClose()) {
    begin(&sys);
    update(&sys);
    draw(&sys);
    end(&sys);
  }

  system_deinit(&sys);
  return OK;
}

void begin(sys_t *sys) {
  if (!sys->changing_state) return;
  switch(sys->state) {
    case SYS_TITLE: mode_title_begin(sys); break;
    case SYS_MENU:  mode_file_begin(sys); break;
    case SYS_TEST:  mode_game_begin(sys); break;

    default:
    TraceLog(LOG_FATAL, "Undefined system state %d", sys->state);
    system_change_state(sys, SYS_EXIT);
    break;
  }
  sys->changing_state = false;
}

void update(sys_t *sys) {
  if (sys->changing_state) return;
  system_update(sys);
  switch(sys->state) {
    case SYS_TITLE: mode_title_update(sys); break;
    case SYS_MENU:  mode_file_update(sys); break;
    case SYS_TEST:  mode_game_update(sys); break;

    default:
    TraceLog(LOG_FATAL, "Undefined system state %d", sys->state);
    system_change_state(sys, SYS_EXIT);
    break;
  }
}

void draw(sys_t *sys) {
  if (sys->changing_state) return;
  switch(sys->state) {
    case SYS_TITLE: mode_title_draw(sys); break;
    case SYS_MENU:  mode_file_draw(sys); break;
    case SYS_TEST:  mode_game_draw(sys); break;

    default:
    TraceLog(LOG_FATAL, "Undefined system state %d", sys->state);
    system_change_state(sys, SYS_EXIT);
    break;
  }

  BeginDrawing();
    ClearBackground(BLACK);
    surf_draw(&sys->surf_main.texture, 0, 0, sys->cfg.window_scale);
  EndDrawing();
}

void end(sys_t *sys) {
  if (!sys->changing_state) return;
  switch(sys->state) {
    case SYS_TITLE: mode_title_end(sys); break;
    case SYS_MENU:  mode_file_end(sys); break;
    case SYS_TEST:  mode_game_end(sys); break;

    default:
    TraceLog(LOG_FATAL, "Undefined system state %d", sys->state);
    system_change_state(sys, SYS_EXIT);
    break;
  }
  sys->state = sys->next_state;
}
