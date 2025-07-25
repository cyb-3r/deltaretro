// std
#include <stdio.h>

// libs
#include "../lib/raylib.h"

// internal headers
#include "../include/config.h"
#include "../include/input.h"
#include "../include/ui.h"
#include "../include/tileset.h"
#include "../include/types.h"
#include "../include/sprites.h"

// window
#define W_WIDTH 256
#define W_HEIGHT 224
#define GAME_FPS 60

#ifdef DEBUG
#define TITLE "MANTLE DEBUG"
#else
#define TITLE "MANTLE"
#endif

#define HUD_HEIGHT 32
#define TILE_SIZE 16

#define ENTITY_SPEED 1.0f

enum directions {
  DIR_DOWN,
  DIR_UP,
  DIR_LEFT,
  DIR_RIGHT,
};

typedef struct entity {
  f32 x, y;
  f32 xsp, ysp;
  i32 life_max, life;
  u8 mobility;
  u8 facing;
  u16 state;
  sprite_t sprite;
} entity_t;

typedef struct game {
  input_t       inputs;
  entity_t      player;
  atlas_t       *animations;
} game_t;

void player_handle_movement(entity_t *self, input_t *inputs) {
  u8
  left =    input_is_down(inputs, INPUT_LEFT),
  right =   input_is_down(inputs, INPUT_RIGHT),
  up =      input_is_down(inputs, INPUT_UP),
  down =    input_is_down(inputs, INPUT_DOWN);

  self->xsp = (right - left) * ENTITY_SPEED;
  self->ysp = (down - up) * ENTITY_SPEED;

  if (right)  self->facing = DIR_RIGHT;
  if (left)   self->facing = DIR_LEFT;
  if (down)   self->facing = DIR_DOWN;
  if (up)     self->facing = DIR_UP;
}

void apply_speed(entity_t *self) {
  self->x += self->xsp;
  self->y += self->ysp;

  if (self->x < 0.0f)
    self->x = 0.0f;
  if (self->y < 0.0f)
    self->y = 0.0f;

  if (self->x + TILE_SIZE >= 256.0f)
    self->x = 256.0f - TILE_SIZE;
  if (self->y + TILE_SIZE >= 224.0f - 32.0f)
    self->y = (224.0f - 32.0f) - TILE_SIZE;
}

void lifebar_draw(i32 life_max, i32 life, i32 x, i32 y, u8 width) {
  DrawRectangle(x, y, width, 8, DARKGRAY);
  DrawRectangle(x, y,
    (int)(width * ((float)life/(float)life_max)),
    8, WHITE
  );
}

void game_init(game_t *self) {
  self->player.life_max = 20;
  self->player.life = 20;
  self->player.x = ((float)W_WIDTH / 2.0f) - 8.0f;
  self->player.y = ((float)(W_HEIGHT - HUD_HEIGHT) / 2.0f) - 8.0f;

  bool ok = atlas_load(
    &self->animations,
    "resources/gfx/heroes.png",
    "resources/gfx/heroes.anim"
  );
  if (!ok) TraceLog(LOG_ERROR, "Failed loading atlas");

  self->player.sprite.atlas = self->animations;
  ok = sprite_set_grp(&self->player.sprite, "HERO_SWORD");
  if (!ok) TraceLog(LOG_ERROR, "Failed setting sprite group for player");

  self->inputs = inputs_default();
  input_load(&(self->inputs));
}

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

  f32 delta = 0.0f;
  game_t game = EMPTY;
  RenderTexture2D
    surface_main  = EMPTY,
    surface_game  = EMPTY,
    surface_hud   = EMPTY;
  tileset_t tileset_main = EMPTY;
  tilemap_t map = EMPTY;

  InitWindow(
    W_WIDTH * conf.window_scale,
    W_HEIGHT * conf.window_scale,
    TITLE
  );
  SetTargetFPS(GAME_FPS);

  surface_main = LoadRenderTexture(W_WIDTH, W_HEIGHT);
  surface_game = LoadRenderTexture(W_WIDTH, W_HEIGHT - HUD_HEIGHT);
  surface_hud  = LoadRenderTexture(W_WIDTH, HUD_HEIGHT);
  tileset_main = tileset_load("resources/gfx/test-tileset.png", 16, 16);
  tilemap_init(&map, &tileset_main, W_WIDTH/16, (W_HEIGHT - HUD_HEIGHT)/16);
  game_init(&game);
  ui_init();

  for (int i = 0; i < (map.width * map.height); i++) {
    map.tiles[i] = 5;
  }

  while (!WindowShouldClose()) {
    delta = GetFrameTime();
    input_update(&(game.inputs));

    player_handle_movement(&(game.player), &(game.inputs));
    apply_speed(&(game.player));

    switch(game.player.facing) {
      case DIR_DOWN:
        sprite_swap_anim(&game.player.sprite, "D.Walk");
      break;
      case DIR_UP:
        sprite_swap_anim(&game.player.sprite, "U.Walk");
      break;
      case DIR_RIGHT:
        sprite_swap_anim(&game.player.sprite, "R.Walk");
      break;
      case DIR_LEFT:
        sprite_swap_anim(&game.player.sprite, "L.Walk");
      break;
      default:
        sprite_swap_anim(&game.player.sprite, "D.Walk");
      break;
    }

    sprite_update(&(game.player.sprite), delta);
    // TraceLog(LOG_DEBUG, "Player anim = %d", game.player.sprite.anim_id);

    BeginTextureMode(surface_hud);
      ClearBackground(BLACK);
      draw_text("HP", (Vector2){ 8.0f, 8.0f - 2.0f });
      lifebar_draw(game.player.life_max, game.player.life, 24, 8, 32);
    EndTextureMode();

    BeginTextureMode(surface_game);
      ClearBackground(GRAY);
      tilemap_render(&map, (Vector2)EMPTY);
      // player_draw(&(game.player));
      sprite_draw(
        &game.player.sprite,
        (Vector2){game.player.x, game.player.y}
      );
    EndTextureMode();

    BeginTextureMode(surface_main);
      ClearBackground(GRAY);
      draw_surface(&(surface_hud.texture), 0, 0, 1);
      draw_surface(&(surface_game.texture), 0, HUD_HEIGHT, 1);
    EndTextureMode();

    BeginDrawing();
      ClearBackground(GRAY);
      draw_surface(&(surface_main.texture), 0, 0, conf.window_scale);
    EndDrawing();
  }

  UnloadRenderTexture(surface_main);
  atlas_unload(&game.animations);
  tilemap_deinit(&map);
  tileset_unload(tileset_main);
  ui_deinit();
  CloseWindow();

  return 0;
}
