#include "sysmode/title.h"
#include "raylib.h"
#include "system.h"
#include "ui.h"

#define START_PROMPT    "PRESS  START"
#define START_PROMPT_X  48
#define START_PROMPT_Y  112

void mode_title_begin(sys_t *sys) {
  TraceLog(LOG_INFO, "Entering TITLE");
}

void mode_title_update(sys_t *sys) {
  if (input_is_pressed(&sys->inputs, INPUT_PAUSE)) {
    system_change_state(sys, SYS_MENU);
    return;
  }
}

void mode_title_draw(sys_t *sys) {
  const col_t bg_color = { 0x42, 0x45, 0xE5, 0xFF };
  BeginTextureMode(sys->surf_main);
    ClearBackground(bg_color);
    draw_text(START_PROMPT, START_PROMPT_X, START_PROMPT_Y);
  EndTextureMode();

  BeginDrawing();
    ClearBackground(bg_color);
    surf_draw(&sys->surf_main.texture, 0, 0, sys->cfg.window_scale);
  EndDrawing();
}

void mode_title_end(sys_t *sys) {
  TraceLog(LOG_INFO, "Exiting TITLE");
}
