#include "entity.h"

void ent_apply_spd(ent_t *self, f32 delta) {
  self->x += self->xsp * delta;
  self->y += self->ysp * delta;
}

rec_t ent_rect(ent_t *self) {
  if (!self) return (rec_t)EMPTY;
  return (rec_t) {
    .x = self->x,
    .y = self->y,
    .width = ENT_SIZE,
    .height = ENT_SIZE
  };
}

rec_t ent_coll(ent_t *self) {
  if (!self) return (rec_t)EMPTY;
  const int half = ENT_SIZE/2;
  return (rec_t) {
    .x = self->x,
    .y = self->y + half,
    .width = ENT_SIZE,
    .height = half
  };
}
