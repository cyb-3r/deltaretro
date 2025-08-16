#include "tileset.h"
#include <stdlib.h>
#include <math.h>

// TILESET IMPL]================================================================

tileset_t tileset_load(const char *path, u16 tile_w, u16 tile_h) {
  tileset_t set = {0};

  set.texture = LoadTexture(path);
  set.tile_width = tile_w;
  set.tile_height = tile_h;
  set.cols = set.texture.width / tile_w;
  set.rows = set.texture.height / tile_h;

  return set;
}

u32 tileset_get_total(tileset_t *self) {
  return (self == NULL) ? 0 : self->cols * self->rows;
}

Rectangle tile_get_rect(tileset_t *set, u32 tile_id) {
  if (!set) return (Rectangle){0};
  if (tile_id >= tileset_get_total(set)) return (Rectangle){0};

  u16 row = tile_id / set->cols;
  u16 col = tile_id % set->cols;

  return (Rectangle){
    col * set->tile_width,
    row * set->tile_height,
    set->tile_width,
    set->tile_height
  };
}

void tileset_draw_tile(tileset_t *self, u32 tile_id, Vector2 pos) {
  if (!self) return;

  Rectangle src = tile_get_rect(self, tile_id);
  Rectangle dest = {
    floor(pos.x),
    floor(pos.y),
    self->tile_width,
    self->tile_height
  };

  DrawTexturePro(
    self->texture,
    src, dest,
    (Vector2){0},
    0.0f,
    WHITE
  );
}

void tileset_unload(tileset_t self) {
  UnloadTexture(self.texture);
}

// TILEMAP IMPL]================================================================

void tilemap_init(tilemap_t *self, tileset_t *set, u16 width, u16 height) {
  if (!self) return;
  if (self->tiles != NULL) free(self->tiles);

  void *ok = malloc((width * height)*sizeof(u32));
  if (!ok) return;
  u32 *tiles = (u32*)(ok);
  for (int j = 0; j < (width * height); j++) tiles[j] = 0;

  self->set = set;
  self->width = width;
  self->height = height;
  self->tiles = tiles;
}

void tilemap_render(tilemap_t *self, Vector2 pos) {
  if (!self || !(self->tiles)) return;

  for (int i = 0; i < (self->height*self->width); i++) {
    tileset_draw_tile(
      self->set,
      self->tiles[i],
      (Vector2) {
        pos.x + ((i % self->width) * self->set->tile_width),
        pos.y + ((i / self->width) * self->set->tile_height)
    });
  }
}

void tilemap_deinit(tilemap_t *self) {
  if (!self || self->tiles == NULL) return;
  free(self->tiles);
  self->set = NULL;
}
