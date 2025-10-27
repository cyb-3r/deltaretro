#ifndef TILESET_H
#define TILESET_H

#include "raylib.h"
#include "types.h"

typedef enum tile_size {
  TILE_SIZE_SMALL = 8,
  TILE_SIZE_NORMAL = 16
} tile_size_t;

typedef struct tileset {
  Texture2D texture;
  u16 tile_width, tile_height;
  u8 cols, rows;
} tileset_t;
typedef struct tileset tlst_t;

tileset_t tileset_load(const char *path, u16 tile_w, u16 tile_h);
void tileset_draw_tile(tileset_t *self, u32 tile_id, Vector2 pos);
void tileset_unload(tileset_t self);

typedef struct tilemap {
  u16 width, height;
  u16 *tiles;
} tilemap_t;
typedef struct tilemap tlmp_t;

void tilemap_init(tilemap_t*, u16 width, u16 height);
void tilemap_render(tilemap_t*, tileset_t*, Vector2 pos);
void tilemap_deinit(tilemap_t *self);

#endif
