#include "system.h"
#include "raylib.h"
#include "ui.h"

#define W_WIDTH   192
#define W_HEIGHT  144
#define GAME_FPS  30
#define CFG_PATH  "./config.toml"
#define TEMP_MEM_SIZE 1024

u32 win_get_w(win_t*);
u32 win_get_h(win_t*);

u32 win_get_w(win_t *self) {
  return self->width * self->scale;
}

u32 win_get_h(win_t *self) {
  return self->height * self->scale;
}

bool system_changing_state(sys_t *self) {
  return self != NULL && self->state != self->next_state;
}

void system_flush_temp(sys_t *self) {
  TraceLog(LOG_INFO, "Flushing temporary memory");
  u8 *temp = (u8*)self->temp;
  for (u16 i = 0; i < TEMP_MEM_SIZE; i++)
    temp[i] = 0;
}

bool system_init(sys_t *self) {
  ChangeDirectory(GetApplicationDirectory());

  self->temp = MemAlloc(TEMP_MEM_SIZE);
  if (self->temp) {
    TraceLog(LOG_INFO, "Temp memory successfully allocated");
    system_flush_temp(self);
  } else {
    TraceLog(LOG_ERROR, "Temp memory allocation failed, exiting..");
    return false;
  }

  self->config = config_default();
  config_load(&self->config, CFG_PATH);

  self->window = (win_t) {
    .width = W_WIDTH,
    .height = W_HEIGHT,
    .scale = self->config.window_scale,
    .fps = GAME_FPS
  };

  InitWindow(win_get_w(&self->window), win_get_h(&self->window), TITLE);
  SetTargetFPS(self->window.fps);

  /* Debug exit key */
  #ifdef DEBUG
  SetExitKey(KEY_BACKSPACE);
  #else
  SetExitKey(KEY_NULL);
  #endif

  ui_init();
  self->surface = LoadRenderTexture(W_WIDTH, W_HEIGHT);
  self->inputs = inputs_default();
  input_load(&(self->inputs));
  game_init(&self->game, W_WIDTH, W_HEIGHT);

  return true;
}

void system_deinit(sys_t *self) {
  TraceLog(LOG_INFO, "Closing program");
  if (self->temp) {
    MemFree(self->temp);
    self->temp = NULL;
  }
  if (IsTextureValid(self->surface.texture))
    UnloadRenderTexture(self->surface);
  ui_deinit();
  CloseWindow();
}

void system_update(sys_t *self) {
  self->delta = GetFrameTime();
  input_update(&self->inputs);
}

bool system_change_state(sys_t *self, i8 next_state) {
  if (!self || next_state > SYS_EXIT) return false;
  self->next_state = next_state;
  return true;
}

void system_exit(sys_t *self) {
  system_change_state(self, SYS_EXIT);
}

void surf_draw(tex_t *self, i32 x, i32 y, i32 scale) {
  DrawTexturePro(
    *self,
    (rec_t){ 0, 0, self->width, -(self->height) },
    (rec_t){ x, y, (self->width * scale), (self->height * scale) },
    (v2_t)EMPTY,
    0.0f, WHITE
  );
}
