#ifndef ENTITY_H
#define ENTITY_H

#include "../lib/raylib.h"
#include "types.h"

#define ENT_SIZE 16
#define ENT_SPD 1.0f

enum ent_directions {
  ENT_DIR_D,
  ENT_DIR_R,
  ENT_DIR_U,
  ENT_DIR_L,
};

enum ent_type {
  ENT_TP_PLR,
  ENT_TP_NPC,
  ENT_TP_ENN,
};

typedef struct entity {
  f32 x;
  f32 y;
  f32 xsp;
  f32 ysp;

  i16 life_max;
  i16 life;

  u8 facing;
  u8 state;
} entity_t;
typedef struct entity ent_t;

Vector2 ent_get_pos(ent_t *self);
Vector2 ent_get_spd(ent_t *self);
Rectangle ent_rect(ent_t *self);
Rectangle ent_coll(ent_t *self);

void ent_apply_spd(ent_t *self);

#endif // ENTITY_H
