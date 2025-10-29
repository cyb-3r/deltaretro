#ifndef SYS_H
#define SYS_H

#include "raylib.h"
#include "types.h"
#include "config.h"
#include "input.h"
#include "game.h"

typedef struct window {
  u32 width;
  u32 height;
  u8  scale;
  u8  fps;
} window_t;
typedef struct window win_t;

u32 win_get_w(win_t*);
u32 win_get_h(win_t*);

enum system_state {
  SYS_EXIT,
  SYS_BOOT,
  SYS_TITLE,
  SYS_MENU,
  SYS_GAME,
  SYS_SETTINGS
};

typedef struct system {
  /* data */
  u8    state;
  u8    next_state;
  u8    save_slot;
  f32   delta;
  void *temp;

  /* structs */
  win_t     window;
  config_t  config;
  input_t   inputs;
  game_t    game;
  rtex_t    surface;
} system_t;
typedef struct system sys_t;

bool system_init(sys_t*);
void system_deinit(sys_t*);
void system_update(sys_t*);

void system_flush_temp(sys_t*);
bool system_changing_state(sys_t*);

bool system_change_state(sys_t*, i8 next_state);
void system_exit(sys_t*);

void surf_draw(Texture*, i32 x, i32 y, i32 scale);

#endif /* SYS_H */
