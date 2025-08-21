#ifndef GAME_H
#define GAME_H

#include "entity.h"
#include "input.h"
#include "tileset.h"
#include "scene.h"

#define PLR_NAME_LEN 8
#define GAME_TLST_MAX 0x08

typedef struct plr_data {
  char name[PLR_NAME_LEN];
  i32 pts;
  u8 lv;
  i8 main_dir;
} plr_data_t;
typedef struct plr_data plrd_t;

typedef struct game {
  f32 delta;
  plrd_t plr_data;
  ent_t player;
  wld_t world;
} game_t;

void game_init(game_t*);
void game_update(game_t*, ipt_t*);
void game_draw(game_t*);

#endif // GAME_H
