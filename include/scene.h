#ifndef SCENE_H
#define SCENE_H

#include "types.h"
#include "entity.h"
#include "tileset.h"

#define TILE_SIZE 0x10

#define SCR_W 192
#define SCR_H 128

#define SCR_T_W ((SCR_W) / (TILE_SIZE))
#define SCR_T_H ((SCR_H) / (TILE_SIZE))

#define COLLISION_BITS (sizeof(u16) * 8)
#define SCR_COLL_MAX ( ((SCR_T_W) * (SCR_T_H)) / (COLLISION_BITS) )

#define WLD_ENT_MAX 0x18

#define WORLD_WIDTH 0x10
#define WORLD_HEIGHT 0x10
#define WLD_SIZE 0x18

typedef struct screen {
  u8  tilemap[SCR_T_W][SCR_T_H];
  u16 collision[SCR_COLL_MAX];
  u8  exits[4];
} screen_t;
typedef struct screen scr_t;

typedef struct world {
  char   path[256];
  tlst_t theme;
  scr_t  cur_scr;
  tlmp_t next_scr;

  u8   type[WLD_ENT_MAX];
  f32  x[WLD_ENT_MAX];
  f32  y[WLD_ENT_MAX];
  i32  life[WLD_ENT_MAX];
  bool active[WLD_ENT_MAX];
} world_t;
typedef struct world wld_t;

#endif // SCENE_H
