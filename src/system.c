#include "system.h"
#include "ui.h"
#include "save.h"

#define GAME_FPS  61
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
  game_update(&self->game, &self->inputs);
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

void title_scr(sys_t *sys) {
  TraceLog(LOG_INFO, "Entering TITLE");
  const Color c = {0x42, 0x45, 0xE5 , 0xFF};
  while (!WindowShouldClose()) {
    system_update(sys);

    if (input_is_pressed(&sys->inputs, INPUT_PAUSE)) {
      sys->state = SYS_MENU;
      break;
    }

    BeginTextureMode(sys->surf_main);
      ClearBackground(c);
      draw_text("PRESS  START", 48, 112);
    EndTextureMode();

    BeginDrawing();
      ClearBackground(c);
      surf_draw(&sys->surf_main.texture, 0, 0, sys->cfg.window_scale);
    EndDrawing();
  }
}

void draw_cursor(int select) {
  const Color c = RED;
  const v2_t size = { 7, 7 };
  v2_t pos = EMPTY;
  switch (select) {
    case 0: pos = (v2_t){ 24, 32 };   break;
    case 1: pos = (v2_t){ 24, 64 };   break;
    case 2: pos = (v2_t){ 24, 96 };   break;
    case 3: pos = (v2_t){ 16, 128 };  break;
    case 4: pos = (v2_t){ 72, 128 };  break;
    case 5: pos = (v2_t){ 136, 128 }; break;
    default: return;
  }
  DrawRectangle(pos.x, pos.y, size.x, size.y,c);
}

void sd_draw(sd_t *data, int y) {
  DrawRectangleRec((rec_t){ 40, y, 16, 16 }, BLUE);
  draw_text(TextFormat("%s", data->name), 64, y);
  draw_text(TextFormat("%04i LV-%i", data->points, data->lv), 80,y + 8);
}

void main_menu(sys_t *sys) {
  TraceLog(LOG_INFO, "Entering MENU");
  int select = 0;
  sd_t saves[SAVE_MAX];
  Texture bg = LoadTexture("resources/gfx/bg-save.png");

  for (int i = 0; i < SAVE_MAX; i++) {
    sd_load(&saves[i], (i + 1));
  }

  while (!WindowShouldClose()) {
    system_update(sys);

    if (input_is_pressed(&sys->inputs, INPUT_PRIM)) {
      if (select < 3) {
        sys->save_slot = select + 1;
        for (int i = 0; i < SAVE_NAME_LEN; i++)
          sys->game.plr_data.name[i] = saves[select].name[i];
        sys->game.plr_data.pts = saves[select].points;
        sys->game.plr_data.lv = saves[select].lv;
        sys->state = SYS_TEST;
        break;
      } else {
        sys->state = SYS_EXIT;
        break;
      }
    }

    if (input_is_pressed(&sys->inputs, INPUT_MENU))
      select = (select + 1) % 6;

    BeginTextureMode(sys->surf_main);
      ClearBackground(BLACK);
      DrawTexture(bg, 0, 0, WHITE);
      draw_text("FILE SELECT", 16, 8);
      draw_cursor(select);
      const int y_off = 32;
      for (int i = 0; i < SAVE_MAX; i++) {
        const int loop_off = i * (4 * 8);
        sd_draw(&saves[i], y_off + loop_off);
      }
      draw_text("COPY   ERASE   END", 24, W_HEIGHT - 16);
    EndTextureMode();

    BeginDrawing();
      ClearBackground(BLACK);
      surf_draw(&sys->surf_main.texture, 0, 0, sys->cfg.window_scale);
    EndDrawing();
  }
  UnloadTexture(bg);
}

inline void hud_draw(const char *name, int pts) {
  draw_text(TextFormat("%s", name), 8, 8);
  draw_text(TextFormat("P%04i", pts), 8 * 10, 8);
}

void test_loop(sys_t *sys) {
  TraceLog(LOG_INFO, "Entering TEST ROOM");
  Texture bg = LoadTexture("resources/gfx/tlmp-board.png");
  RenderTexture surf_game = LoadRenderTexture(
    sys->window.width, sys->window.height - 16
  );

  while (!WindowShouldClose()) {
    system_update(sys);

    BeginTextureMode(surf_game);
      ClearBackground(BLACK);
      DrawTexture(bg, 0, 0, WHITE);
      game_draw(&sys->game);
    EndTextureMode();

    BeginTextureMode(sys->surf_main);
      ClearBackground(BLACK);
      hud_draw(sys->game.plr_data.name, sys->game.plr_data.pts);
      surf_draw(&surf_game.texture, 0, 16, 1);
    EndTextureMode();

    BeginDrawing();
      ClearBackground(BLACK);
      surf_draw(&sys->surf_main.texture, 0, 0, sys->cfg.window_scale);
    EndDrawing();
  }

  UnloadTexture(bg);
  UnloadRenderTexture(surf_game);
}
