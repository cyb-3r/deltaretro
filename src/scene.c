#include "../include/scene.h"

bool screen_gcoll(scr_t *self, const u8 x, const u8 y) {
  if (!self) return false;
  if (x >= SCR_T_W || y >= SCR_T_H) return false;

  const u32 idx = (y * SCR_T_W) + x;
  const u8 array_idx = idx / 16;
  const u8 bit_idx = idx % 16;

  const u32 mask = 1 << bit_idx;
  return (self->collision[array_idx] & mask) > 0;
}

void screen_scoll(scr_t *self, const u8 x, const u8 y) {
  if (!self) return;
  if (x >= SCR_T_W || y >= SCR_T_H) return;

  const u32 idx = (y * SCR_T_W) + x;
  const u8 array_idx = idx / 16;
  const u8 bit_idx = idx % 16;

  const u32 mask = 1 << bit_idx;
  self->collision[array_idx] |= mask;
}
