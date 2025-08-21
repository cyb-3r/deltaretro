#include "scene.h"

bool scr_collision(scr_t *self, int x, int y) {
  if (!self || (x >= SCR_T_W || y >= SCR_T_H)) return false;
  const u8 ttp = self->tile_type[x][y];
  return ttp == TTP_SOLID || ttp == TTP_SAVE;
}
