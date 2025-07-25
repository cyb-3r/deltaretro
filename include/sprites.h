#ifndef SPRITES_H
#define SPRITES_H

#include "types.h"
#include "../lib/raylib.h"

#define ATLAS_NAME_LEN    0x20
#define ATLAS_LEN         0x08
#define FRAME_ARRAY_LEN   0x100
#define ANIM_LEN          0x10
#define ANIM_FRAME_LEN    0x08
#define ANIM_NAME_LEN     0x20
#define ATLASGRP_LEN      0x08
#define ANIGRP_NAME_LEN   0x20
#define PATH_LEN          0x100

typedef struct frame {
  f32 s_x, s_y, s_width, s_height,
      o_x, o_y;
  f32 duration;
} frame_t;

typedef struct animation {
  char name[ANIM_NAME_LEN];
  bool loop;
  u8   frame_count;
  u8   frames[ANIM_FRAME_LEN];
} animation_t;
typedef struct animation anim_t;

typedef struct frame_batch {
  u8      count;
  frame_t values[FRAME_ARRAY_LEN];
} frame_batch_t;

typedef struct anim_group {
  // metadata
  char name[ANIGRP_NAME_LEN];
  bool loaded;
  // data
  frame_batch_t frames;
  u8 anim_count;
  animation_t anims[ANIM_LEN];
} anim_group_t;

typedef struct spr_atlas {
  Texture texture;
  char anim_path[PATH_LEN];
  u8 group_count;
  anim_group_t groups[ATLASGRP_LEN];

  // this padding is placed here to check if we are dealing with memory
  // corruption from the goups array
  // CONCLUSION: it keeps setting loaded count to an absurd value despite
  // the padding being present, so it's coming from somewhere else...
  u8 padding[16];

  u8 loaded_count;
  u8 loaded_groups[ATLASGRP_LEN];
} spr_atlas_t;
typedef struct spr_atlas atlas_t;

typedef struct sprite {
  atlas_t *atlas;
  u8 grp_id;
  // state
  f32  timer;
  bool paused;
  u8   frame, anim_id;
} sprite_t;
typedef struct sprite spr_t;

void sprite_swap_anim
(sprite_t *self, const char *anim_tag);
void sprite_play_anim
(sprite_t *self, const char *anim_tag);
void sprite_update(sprite_t *self, f32 delta);
void sprite_draw(sprite_t *self, Vector2 pos);
bool sprite_set_grp(sprite_t *self, const char *group_tag);

bool atlas_load
(atlas_t **atlas, const char *texture_path, const char *anim_path);
bool atlas_unload(atlas_t **atlas);
i8 atlas_grp_req(atlas_t *atlas, const char *grp_name);

#endif // SPRITES_H
