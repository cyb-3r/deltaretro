#include "../include/system.h"
#include "../include/ui.h"
#include "../include/save.h"

#define GAME_FPS  61
#define CFG_PATH  "./config.toml"

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

  sd_t test_sd = (sd_t) {"Cybits", 69, 1};
  bool sd_ok = sf_init();
  if (!sd_ok) TraceLog(LOG_ERROR, "Save system is broken :(");
  else {
    TraceLog(LOG_DEBUG, "Save system works :)");
    sd_ok = sd_write(&test_sd, 2);
    sd_ok = sd_read();
    if (!sd_ok) TraceLog(LOG_ERROR, "But can't read save properly :(");
  }

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

void draw_surface(Texture2D *self, i32 x, i32 y, i32 scale) {
  DrawTexturePro(
    *self,
    (Rectangle){ 0, 0, self->width, -(self->height) },
    (Rectangle){ x, y, (self->width * scale), (self->height * scale) },
    (Vector2)EMPTY,
    0.0f, WHITE
  );
}

void title_scr(sys_t *sys) {
  TraceLog(LOG_INFO, "Entering TITLE");
  while (!WindowShouldClose()) {
    system_update(sys);

    if (input_is_pressed(&sys->inputs, INPUT_PAUSE)) {
      sys->state = SYS_MENU;
      break;
    }

    BeginTextureMode(sys->surf_main);
      ClearBackground(BLACK);
      draw_text("DELTARETRO.JPEG", 8, 8);
      draw_text("POWERED BY RAYLIB", 8, W_HEIGHT - 8);
    EndTextureMode();

    BeginDrawing();
      ClearBackground(BLACK);
      draw_surface(&sys->surf_main.texture, 0, 0, sys->cfg.window_scale);
    EndDrawing();
  }
}

void draw_cursor(int slc) {
  switch (slc) {
    case 0:
    DrawRectangleRec((Rectangle){
      24, 32, 7, 7
    }, RED);
    break;
    case 1:
    DrawRectangleRec((Rectangle){
      24, 64, 7, 7
    }, RED);
    break;
    case 2:
    DrawRectangleRec((Rectangle){
      24, 96, 7, 7
    }, RED);
    break;
    case 3:
    DrawRectangleRec((Rectangle){
      16, 128, 7, 7
    }, RED);
    break;
    case 4:
    DrawRectangleRec((Rectangle){
      72, 128, 7, 7
    }, RED);
    break;
    case 5:
    DrawRectangleRec((Rectangle){
      136, 128, 7, 7
    }, RED);
    break;
    default:
    break;
  }
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
        DrawRectangleRec((Rectangle){
          40, y_off + loop_off,
          16, 16
        }, BLUE);
        draw_text(
          TextFormat("%s", saves[i].name),
          64,
          y_off + loop_off
        );
        draw_text(
          TextFormat("%04i LV-%i", saves[i].points, saves[i].lv),
          80,
          y_off + loop_off + 8
        );
      }
      draw_text("COPY   ERASE   END", 24, W_HEIGHT - 16);
    EndTextureMode();

    BeginDrawing();
      ClearBackground(BLACK);
      draw_surface(&sys->surf_main.texture, 0, 0, sys->cfg.window_scale);
    EndDrawing();
  }
  UnloadTexture(bg);
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
      draw_text(TextFormat("%s", sys->game.plr_data.name), 8, 0);
      draw_text(TextFormat("P %04i", sys->game.plr_data.pts), W_WIDTH - 48, 0);
      draw_surface(&surf_game.texture, 0, 16, 1);
    EndTextureMode();

    BeginDrawing();
      ClearBackground(BLACK);
      draw_surface(&sys->surf_main.texture, 0, 0, sys->cfg.window_scale);
    EndDrawing();
  }

  UnloadTexture(bg);
  UnloadRenderTexture(surf_game);
}
