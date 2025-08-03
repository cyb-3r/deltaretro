#include "../include/entity.h"

void ent_apply_spd(ent_t *self) {
  self->x += self->xsp;
  self->y += self->ysp;
}

Rectangle ent_rect(ent_t *self) {
  if (!self) return (Rectangle)EMPTY;
  return (Rectangle) {
    .x = self->x,
    .y = self->y,
    .width = ENT_SIZE,
    .height = ENT_SIZE
  };
}

Rectangle ent_coll(ent_t *self) {
  if (!self) return (Rectangle)EMPTY;
  const int half = ENT_SIZE/2;
  return (Rectangle) {
    .x = self->x,
    .y = self->y + half,
    .width = ENT_SIZE,
    .height = half
  };
}
