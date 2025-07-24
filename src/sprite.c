#include "../include/sprites.h"
#include <stdio.h>
#include <string.h>

// FRAME ] =====================================================================
Rectangle frame_get_source(frame_t *self) {
  if (!self) return (Rectangle)EMPTY;

  return (Rectangle) {
    .x = self->s_x,
    .y = self->s_y,
    .width = self->s_width,
    .height = self->s_height
  };
}

Vector2 frame_get_origin(frame_t *self) {
  if (!self) return (Vector2)EMPTY;

  return (Vector2) {
    .x = self->o_x,
    .y = self->o_y
  };
}

void frame_draw(frame_t *self, Texture *atlas, Vector2 position) {
  Rectangle source = frame_get_source(self);
  DrawTexturePro(
    *atlas,
    source,
    (Rectangle) {
      position.x, position.y,
      source.width, source.height
    },
    frame_get_origin(self),
    0.0f,
    WHITE
  );
}

// FRAME BATCH ] ===============================================================
frame_t batch_get_frame(frame_batch_t *self, u8 index) {
  if (!self) return (frame_t)EMPTY;
  TraceLog(LOG_DEBUG, "BATCH: %d < %d ?", index, self->count);
  return (index >= self->count) ?
    self->values[self->count - 1] :
    self->values[index];
}

// ANIMATION ] =================================================================
u8 anim_get_frame_index(animation_t *self, u8 index) {
  if (!self) return 0;
  for (int i = 0; i < self->frame_count; i++)
    TraceLog(LOG_DEBUG, "ANIM[%d] = %d", i, self->frames[i]);
  u8 out = (index >= self->frame_count) ?
          self->frames[self->frame_count - 1] :
          self->frames[index];
  TraceLog(LOG_DEBUG, "ANIM: %d < %d => %d", index, self->frame_count, out);
  return out;
}

// GROUP ] =====================================================================
frame_t group_get_frame(anim_group_t *grp, u8 a_index, u8 f_index) {
  if (!grp || !grp->loaded) return (frame_t)EMPTY;
  animation_t *anim = &grp->anims[a_index];
  u8 mapped_index = anim_get_frame_index(anim, f_index);
  TraceLog(
    LOG_DEBUG,
    "ANIM[%d] = FRAME #%d",
    f_index, mapped_index
  );
  return batch_get_frame(&grp->frames, mapped_index);
}

// ATLAS ] =====================================================================
frame_t atlas_get_frame(atlas_t *self, u8 grp_id, u8 a_index, u8 f_index) {
  if (!self) return (frame_t)EMPTY;
  anim_group_t *grp = &self->groups[grp_id];
  if (!grp->loaded) return (frame_t)EMPTY;
  return group_get_frame(grp, a_index, f_index);
}

u8 atlas_get_anim_id(atlas_t *self, u8 grp_id, const char *name) {
  if (!self || !name) return 0;

  anim_group_t *grp = &self->groups[grp_id];
  for (int i = 0; i < grp->anim_count; i++) {
    // TraceLog(LOG_DEBUG, "Checking: %s <> %s", name, grp->anims[i].name);
    if (strcmp(name, grp->anims[i].name) == 0) {
      // TraceLog(LOG_DEBUG, "Animation '%s' HIT", name);
      return i;
    }
  }
  // TraceLog(LOG_DEBUG, "Animation '%s' MISS", name);
  return 0;
}

// SPRITE ] ====================================================================
anim_group_t *spr_get_group(spr_t *self) {
  if (!self) return NULL;
  return &self->atlas->groups[self->grp_id];
}

animation_t *spr_get_anim(spr_t *self) {
  if (!self) return NULL;
  anim_group_t *grp = spr_get_group(self);
  if (!grp) return NULL;
  return &grp->anims[self->anim_id];
}

void sprite_update(sprite_t *self, f32 delta) {
  if (!self) return;
  if (self->paused) return;

  animation_t *current = spr_get_anim(self);
  if (!current) return;
  self->timer += delta;

  while (true) {
    frame_t current_frame = group_get_frame(
      &self->atlas->groups[self->grp_id],
      self->anim_id,
      self->frame
    );
    if (self->timer < current_frame.duration) break;
    self->timer -= current_frame.duration;
    self->frame++;
    if (self->frame >= current->frame_count)
      self->frame = (current->loop) ? 0 : current->frame_count - 1;
  }
}

void sprite_reset(sprite_t *self) {
  if (!self) return;
  self->frame = 0;
  self->paused = false;
  self->timer = 0.0f;
}

void sprite_swap_anim (sprite_t *self, const char *anim_tag) {
  if (!self) return;
  self->anim_id = atlas_get_anim_id(self->atlas, self->grp_id, anim_tag);
}

void sprite_play_anim (sprite_t *self, const char *anim_tag) {
  if (!self) return;
  sprite_swap_anim(self, anim_tag);
  sprite_reset(self);
}

void sprite_draw(sprite_t *self, Vector2 pos) {
  if (!self) return;
  animation_t *current = spr_get_anim(self);
  if (!current) {
    TraceLog(LOG_ERROR, "Sprite couldn't get animation");
    return;
  }
  frame_t current_frame = atlas_get_frame(
      self->atlas,
      self->grp_id,
      self->anim_id,
      self->frame
    );

  frame_draw(&current_frame, &self->atlas->texture, pos);
}
