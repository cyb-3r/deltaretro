#ifndef SPRITES_H
#define SPRITES_H

#include "types.h"
#include "raylib.h"

#define ATLAS_NAME_LEN 0x20

#define FRAME_ARRAY_LEN 0x100
#define ANIM_LEN        0x10

#define ANIM_FRAME_LEN 0x08
#define ANIM_NAME_LEN  0x18

#define ATLAS_REG_LEN      0x08
#define ATLAS_REG_NAME_LEN 0x18
#define PATH_LEN          0x100

#define QUEUE_LEN 0x10

enum anim_dirs {
  DOWN,
  UP,
  LEFT,
  RIGHT,
  ANIM_DIR_CT
};

typedef struct frame {
  f32 s_x, s_y, s_w, s_h,
      o_x, o_y;
  f32 duration;
} frame_t;

typedef struct animation {
  const char name[ANIM_NAME_LEN];

  const bool loop;
  const bool directionnal;
  const i8 length;
  const i8 index[ANIM_DIR_CT];
} animation_t;
typedef struct animation ani_t;

typedef struct atlas_region {
  char name[ATLAS_REG_NAME_LEN];

  u8 frct;
  frame_t frames[FRAME_ARRAY_LEN];
  u8 anct;
  animation_t anims[ANIM_LEN];
} atlas_region_t;
typedef struct atlas_region atrg_t;

typedef struct spr_atlas {
  Texture texture;
  char file_path[PATH_LEN];

  u8 region_count;
  atrg_t groups[ATLAS_REG_LEN];

  u8 loaded_count;
  u8 loaded_groups[ATLAS_REG_LEN];
} spr_atlas_t;
typedef struct spr_atlas atlas_t;

typedef struct frame_queue {
  u8 queue[QUEUE_LEN];
  i8 head, tail;
} frame_queue_t;

typedef struct sprite {
  atlas_t *atlas;
  u8 grp_id;

  f32  timer;
  bool paused;
  u8 frame, anim_id;

  i8 pending_anim;
  frame_queue_t fq;
} sprite_t;
typedef struct sprite spr_t;

void sprite_swap_anim
(sprite_t *self, const char *anim_tag);
void sprite_update(sprite_t *self, f32 delta);
void sprite_draw(sprite_t *self, Vector2 pos);
bool sprite_set_grp(sprite_t *self, const char *group_tag);

bool atlas_load
(atlas_t **atlas, const char *texture_path, const char *anim_path);
bool atlas_unload(atlas_t **atlas);
i8 atlas_grp_req(atlas_t *atlas, const char *grp_name);

#endif // SPRITES_H
