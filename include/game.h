#ifndef GAME_H
#define GAME_H

#include "entity.h"
#include "input.h"
#include "tileset.h"

#define GAME_TLST_MAX 0x08

typedef struct game {
  input_t inputs;
  atlas_t *animations;
  tlst_t tilesets[GAME_TLST_MAX];

  f32 delta;
  entity_t player;
} game_t;

void game_init(game_t *self);
void game_update(game_t *self);

#endif // GAME_H
