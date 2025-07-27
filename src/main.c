// std
#include <stdio.h>

// libs
#include "../lib/raylib.h"

// user
#include "../include/game.h"
#include "../include/config.h"
#include "../include/ui.h"
#include "../include/types.h"
#include "../include/sprites.h"

#define W_WIDTH 256
#define W_HEIGHT 224
#define GAME_FPS 61

#ifdef DEBUG
#define TITLE "MANTLE DEBUG"
#else
#define TITLE "MANTLE"
#endif

#define HUD_HEIGHT 32
#define TILE_SIZE 16

void draw_surface(Texture2D *self, i32 x, i32 y, i32 scale) {
  DrawTexturePro(
    *self,
    (Rectangle){
      0, 0,
      self->width, -(self->height)
    },
    (Rectangle){
      x, y,
      self->width * scale, self->height * scale
    },
    (Vector2){ 0, 0 },
    0.0f, WHITE
  );
}

i32 main(void) {
  #ifdef DEBUG
    SetTraceLogLevel(LOG_DEBUG);
    TraceLog(LOG_DEBUG, "Debug mode is ON");
  #endif

  config_t conf = config_default();
  config_load(&conf);

  game_t game = EMPTY;
  RenderTexture2D
    surface_main = EMPTY,
    surface_game = EMPTY;

  InitWindow(
    W_WIDTH * conf.window_scale,
    W_HEIGHT * conf.window_scale,
    TITLE
  );
  SetTargetFPS(GAME_FPS);

  Texture bg = LoadTexture("resources/gfx/tlmp-board.png");
  surface_main = LoadRenderTexture(W_WIDTH, W_HEIGHT);
  surface_game = LoadRenderTexture(W_WIDTH - 32, W_HEIGHT - 32);

  game_init(&game);
  ui_init();

  while (!WindowShouldClose()) {
    game_update(&game);

    BeginTextureMode(surface_game);
      ClearBackground(BLACK);
      DrawTexture(bg, 16, 16, WHITE);
      sprite_draw(
        &game.player.sprite,
        (Vector2){game.player.x, game.player.y}
      );
    EndTextureMode();

    BeginTextureMode(surface_main);
      ClearBackground(GRAY);
      draw_surface(&(surface_game.texture), 16, 16, 1);
    EndTextureMode();

    BeginDrawing();
      ClearBackground(GRAY);
      draw_surface(&(surface_main.texture), 0, 0, conf.window_scale);
    EndDrawing();
  }

  UnloadRenderTexture(surface_main);
  atlas_unload(&game.animations);
  UnloadTexture(bg);
  ui_deinit();
  CloseWindow();

  return 0;
}
