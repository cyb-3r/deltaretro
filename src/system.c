#include "system.h"
#include "raylib.h"
#include "ui.h"
#include "save.h"

#define GAME_FPS  30
#define CFG_PATH  "./config.toml"

u32 win_get_w(win_t*);
u32 win_get_h(win_t*);
void draw_cursor(int select);
void sd_draw(sd_t*, int y);
void hud_draw(const char *name, int pts);

u32 win_get_w(win_t *self) {
  return self->width * self->scale;
}

u32 win_get_h(win_t *self) {
  return self->height * self->scale;
}

bool system_init(sys_t *self) {
  ChangeDirectory(GetApplicationDirectory());

  self->cfg = config_default();
  config_load(&self->cfg, CFG_PATH);

  self->window = (win_t) {
    .width = W_WIDTH,
    .height = W_HEIGHT,
    .scale = self->cfg.window_scale,
    .fps = GAME_FPS
  };

  InitWindow(win_get_w(&self->window), win_get_h(&self->window), TITLE);
  SetTargetFPS(self->window.fps);

  bool sd_ok = sf_init();
  if (!sd_ok) TraceLog(LOG_ERROR, "Save system is broken :(");

  #ifdef DEBUG
  SetExitKey(KEY_BACKSPACE);
  #else
  SetExitKey(KEY_NULL);
  #endif // DEBUG EXIT
  ui_init();
  self->surf_main = LoadRenderTexture(W_WIDTH, W_HEIGHT);
  self->inputs = inputs_default();
  input_load(&(self->inputs));
  game_init(&self->game);

  return true;
}

void system_deinit(sys_t *self) {
  TraceLog(LOG_INFO, "Closing program");
  UnloadRenderTexture(self->surf_main);
  ui_deinit();
  CloseWindow();
}

void system_update(sys_t *self) {
  input_update(&self->inputs);
}

bool system_change_state(sys_t *self, int next_state) {
  if (!self || next_state > SYS_EXIT) return false;
  self->next_state = next_state;
  self->changing_state = true;
  return true;
}

void surf_draw(Texture *self, i32 x, i32 y, i32 scale) {
  DrawTexturePro(
    *self,
    (rec_t){ 0, 0, self->width, -(self->height) },
    (rec_t){ x, y, (self->width * scale), (self->height * scale) },
    (v2_t)EMPTY,
    0.0f, WHITE
  );
}
