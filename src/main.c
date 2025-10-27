#include <stdio.h>

#include "raylib.h"

#ifdef DEBUG
#include "debug.h"
#endif

#include "stage-title.h"
#include "stage-file.h"
#include "stage-game.h"

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
  dbg_t debug = { 0 };
  #endif

  sys_t sys = { 0 };
  const bool ok = system_init(&sys);
  if (!ok) {
    TraceLog(LOG_FATAL, "Failed to init system");
    return ERR;
  }

  while ((sys.state != SYS_EXIT) && !WindowShouldClose()) {
    begin(&sys);

    #ifdef DEBUG  /* debug pausing allowed */
    dbg_step(&debug);
    if (!debug.pause) update(&sys);
    else { if (debug.frmskp) update(&sys); }
    #else         /* normal update */
    update(&sys);
    #endif

    draw(&sys);
    end(&sys);
  }

  system_deinit(&sys);
  return OK;
}

void begin(sys_t *sys) {
  if (!system_changing_state(sys)) return;
  sys->state = sys->next_state;

  switch(sys->state) {
    case SYS_TITLE: stage_title_begin(sys); break;
    case SYS_MENU:  stage_file_begin(sys); break;
    case SYS_TEST:  stage_game_begin(sys); break;

    default:
    TraceLog(LOG_FATAL, "Undefined system state %d", sys->state);
    system_change_state(sys, SYS_EXIT);
    break;
  }
}

void update(sys_t *sys) {
  if (system_changing_state(sys)) return;

  system_update(sys);
  switch(sys->state) {
    case SYS_TITLE: stage_title_update(sys); break;
    case SYS_MENU:  stage_file_update(sys); break;
    case SYS_TEST:  stage_game_update(sys); break;

    default:
    TraceLog(LOG_FATAL, "Undefined system state %d", sys->state);
    system_change_state(sys, SYS_EXIT);
    break;
  }
}

void draw(sys_t *sys) {
  if (system_changing_state(sys)) return;

  switch(sys->state) {
    case SYS_TITLE: stage_title_draw(sys); break;
    case SYS_MENU:  stage_file_draw(sys); break;
    case SYS_TEST:  stage_game_draw(sys); break;

    default:
    TraceLog(LOG_FATAL, "Undefined system state %d", sys->state);
    system_change_state(sys, SYS_EXIT);
    break;
  }

  BeginDrawing();
    ClearBackground(BLACK);
    surf_draw(&sys->surface.texture, 0, 0, sys->config.window_scale);
  EndDrawing();
}

void end(sys_t *sys) {
  if (!system_changing_state(sys)) return;

  switch(sys->state) {
    case SYS_TITLE: stage_title_end(sys); break;
    case SYS_MENU:  stage_file_end(sys); break;
    case SYS_TEST:  stage_game_end(sys); break;

    default:
    TraceLog(LOG_FATAL, "Undefined system state %d", sys->state);
    system_change_state(sys, SYS_EXIT);
    break;
  }
}
