#ifndef SYS_H
#define SYS_H

#include "raylib.h"
#include "types.h"
#include "config.h"
#include "input.h"
#include "game.h"

#define W_WIDTH   192
#define W_HEIGHT  144

typedef struct window {
  u32 width;
  u32 height;
  u8  scale;
  u8  fps;
} window_t;
typedef struct window win_t;

u32 win_get_w(win_t*);
u32 win_get_h(win_t*);

enum sys_state {
  SYS_TITLE,
  SYS_MENU,
  SYS_TEST,
  SYS_EXIT
};

typedef struct system {
  win_t window;
  u8 state;
  u8 next_state;
  bool changing_state;
  u8 save_slot;
  config_t cfg;
  input_t inputs;
  game_t game;
  RenderTexture surf_main;
} system_t;
typedef struct system sys_t;

bool system_init(sys_t*);
void system_deinit(sys_t*);
void system_update(sys_t*);

bool system_change_state(sys_t*, int next_state);
void system_panic(sys_t*)__attribute__((unused));

void surf_draw(Texture*, i32 x, i32 y, i32 scale);

#endif // SYS_H
