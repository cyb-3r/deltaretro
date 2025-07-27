#include "../include/entity.h"

void ent_apply_spd(ent_t *self) {
  self->x += self->xsp;
  self->y += self->ysp;
}
