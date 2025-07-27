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
frame_t batch_get_frame(atlas_region_t *self, u8 index) {
  if (!self) return (frame_t)EMPTY;
  return (index >= self->count) ?
    self->frames[self->count - 1] :
    self->frames[index];
}

// ANIMATION ] =================================================================
u8 anim_get_frame_index(animation_t *self, u8 index) {
  if (!self) return 0;
  u8 out = (index >= self->frame_count) ?
          self->frames[self->frame_count - 1] :
          self->frames[index];
  return out;
}

// GROUP ] =====================================================================
frame_t group_get_frame(anim_group_t *grp, u8 a_index, u8 f_index) {
  if (!grp || !grp->loaded) return (frame_t)EMPTY;
  animation_t *anim = &grp->anims[a_index];
  u8 mapped_index = anim_get_frame_index(anim, f_index);
  return batch_get_frame(&grp->frames, mapped_index);
}

// ATLAS ] =====================================================================
frame_t atlas_get_frame(atlas_t *self, u8 grp_id, u8 f_index) {
  if (!self) return (frame_t)EMPTY;
  anim_group_t *grp = &self->groups[grp_id];
  if (!grp->loaded) return (frame_t)EMPTY;
  return grp->frames.frames[f_index];
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

bool is_last(spr_t *self) {
  if (!self) return true;

  animation_t *ani = spr_get_anim(self);
  return (self->frame == ani->frames[ani->frame_count - 1]);
}

i8 sprite_dq(spr_t *self) {
  if (!self) return 0;

  frame_queue_t *fq = &self->fq;
  i8 to_dq = fq->queue[fq->head];
  if (fq->head != fq->tail) {
    fq->head = (fq->head + 1) % QUEUE_LEN;
  }

  return to_dq;
}

bool sprite_q(spr_t *self, u8 val) {
  if (!self) return false;

  TraceLog(LOG_DEBUG, "queued val = %d", val);
  frame_queue_t *fq = &self->fq;

  const int next = (fq->tail + 1) % QUEUE_LEN;

  if (next == fq->head) return false;
  if (fq->queue[fq->tail-1] == val) return true;

  fq->queue[fq->tail] = val;
  fq->tail = next;
  return true;
}

bool sprite_nq(spr_t *self, u8 *vals, const u8 n) {
  if (!self) return false;

  bool ok = true;
  for (u8 i = 0; i < n; i++) {
    ok = sprite_q(self, vals[i]);
    if (!ok) break;
  }

  return ok;
}

void sprite_qres(spr_t *self) {
  if (!self) return;

  frame_queue_t *fq = &self->fq;
  for (int i = 0; i < QUEUE_LEN; i++) {
    fq->queue[i] = 0;
  }
  fq->head = 0;
  fq->tail = 0;

  animation_t *ani = spr_get_anim(self);
  sprite_nq(self, ani->frames, ani->frame_count);
  self->frame = fq->queue[fq->head];
}

bool sprite_queue_empty(spr_t *self) {
  if (!self) return true;
  frame_queue_t *fq = &self->fq;
  return fq->head == fq->tail;
}

void sprite_update(sprite_t *self, f32 delta) {
  if (!self) return;
  if (self->paused) return;
  TraceLog(LOG_DEBUG, "FQPOS HEAD: %d TAIL: %d", self->fq.head, self->fq.tail);

  frame_t current_frame =
      self->atlas->groups[self->grp_id].frames.frames[self->frame];

  animation_t *current = spr_get_anim(self);
  if (!current) return;
  self->timer +=
    (!sprite_queue_empty(self) || self->timer < current_frame.duration) ?
    delta : 0;

  while (true) {
    if (self->timer < current_frame.duration) break;
    if (sprite_queue_empty(self)) break;

    self->timer -= current_frame.duration;
    self->frame = sprite_dq(self);

    current_frame =
      self->atlas->groups[self->grp_id].frames.frames[self->frame];
    if (is_last(self)) sprite_nq(self, current->frames, current->frame_count);
  }
  TraceLog(LOG_DEBUG, "timer = %.2f", self->timer);
}

bool sprite_anim_finished(sprite_t *self) {
  if (!self) return false;

  animation_t *anim = spr_get_anim(self);
  if (!anim) return false;

  return self->frame == (anim->frame_count - 1);
}

void sprite_reset(sprite_t *self) {
  if (!self) return;
  sprite_qres(self);
  self->paused = false;
  self->timer = 0.0f;
}

void sprite_swap_anim (sprite_t *self, const char *anim_tag) {
  if (!self) return;

  int anid = atlas_get_anim_id(self->atlas, self->grp_id, anim_tag);

  if (anid == self->anim_id) return;
  self->anim_id = anid;
  animation_t *ani = spr_get_anim(self);
  sprite_nq(self, ani->frames, ani->frame_count);
}

// void sprite_play_anim (sprite_t *self, const char *anim_tag) {
//   if (!self) return;
//   sprite_swap_anim(self, anim_tag);
//   sprite_reset(self);
// }

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
      self->frame
    );

  frame_draw(&current_frame, &self->atlas->texture, pos);
}
