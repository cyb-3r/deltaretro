#include "tileset.h"
#include "raylib.h"
#include <math.h>

tileset_t tileset_load(const char *path, u16 tile_w, u16 tile_h) {
  tileset_t set = {
    .texture =      LoadTexture(path),
    .tile_width =   tile_w,
    .tile_height =  tile_h,
    .cols =         set.texture.width / tile_w,
    .rows =         set.texture.height / tile_h
  };

  return set;
}

u32 tileset_get_total(tileset_t *self) {
  return (self == NULL) ? 0 : self->cols * self->rows;
}

Rectangle tile_get_rect(tileset_t *self, u32 tile_id) {
  if (!self) return (Rectangle){0};
  if (tile_id >= tileset_get_total(self)) return (Rectangle){0};

  u16 row = tile_id / self->cols;
  u16 col = tile_id % self->cols;

  return (Rectangle){
    col * self->tile_width,
    row * self->tile_height,
    self->tile_width,
    self->tile_height
  };
}

void tileset_draw_tile(tileset_t *self, u32 tile_id, Vector2 pos) {
  if (!self) return;

  Rectangle src = tile_get_rect(self, tile_id);
  const v2_t dest = {
    floor(pos.x),
    floor(pos.y)
  };

  DrawTextureRec(self->texture, src, dest, WHITE);
}

void tileset_unload(tileset_t self) {
  UnloadTexture(self.texture);
}



void tilemap_init(tilemap_t *self, u16 width, u16 height) {
  if (!self) return;
  if (self->tiles != NULL) {
    TraceLog(LOG_INFO, "Tile data found, overwriting it");
    MemFree(self->tiles);
  }
  TraceLog(LOG_INFO, "Initializing tilemap");

  void *ok = MemAlloc((width * height)*sizeof(u16));
  if (!ok) return;
  u16 *tiles = (u16*)(ok);
  for (int j = 0; j < (width * height); j++) tiles[j] = 0;

  self->width = width;
  self->height = height;
  self->tiles = tiles;
}

void tilemap_render(tilemap_t *map, tileset_t *set, Vector2 pos) {
  if (!map || !(map->tiles)) return;

  for (u16 i = 0; i < (map->height*map->width); i++) {
    tileset_draw_tile(
      set,
      map->tiles[i],
      (Vector2) {
        pos.x + ((i % map->width) * set->tile_width),
        pos.y + (((f32)i / map->width) * set->tile_height)
    });
  }
}

void tilemap_deinit(tilemap_t *self) {
  if (!self || self->tiles == NULL) return;
  TraceLog(LOG_INFO, "Freeing tilemap");
  MemFree(self->tiles);
  self->tiles = NULL;
}
