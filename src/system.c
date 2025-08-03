#include "../include/system.h"

u32 win_get_w(win_t *self) {
  return self->width * self->scale;
}

u32 win_get_h(win_t *self) {
  return self->height * self->scale;
}

bool system_init(sys_t *self) {
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

  self->surf_main = LoadRenderTexture(W_WIDTH, W_HEIGHT);

  self->inputs = inputs_default();
  input_load(&(self->inputs));

  game_init(&self->game);

  return true;
}

void system_deinit(sys_t *self) {
  TraceLog(LOG_INFO, "Closing program");
  UnloadRenderTexture(self->surf_main);
  CloseWindow();
}

void system_update(sys_t *self) {
  input_update(&self->inputs);
  game_update(&self->game, &self->inputs);
}
