#include "../include/sprites.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define METADATA_CAPACITY 0x10

typedef struct file_header {
    u16 total_frames;
    u16 total_anims;
    u16 group_count;
} file_header_t;

typedef struct group_header {
    char name[ANIGRP_NAME_LEN];
    u8 frame_count;
    u8 anim_count;
} group_header_t;

typedef struct grpmeta {
    char name[ANIGRP_NAME_LEN];
    i64 data_offset;
    u8 frm_ct;
    u8 ani_ct;
} grpmeta_t;

typedef struct atlas_internal {
    spr_atlas_t base;

    // useful data to apply LRU
    i32 idle_time[ATLASGRP_LEN];
    i32 priority[ATLASGRP_LEN];

    // metadata
    u8 total_file_groups;
    grpmeta_t file_groups[METADATA_CAPACITY];
} atlas_internal_t;

FILE *atlas_open_anim(atlas_t *self) {
  if (!self) return NULL;
  FILE *f = fopen(self->anim_path, "rb");
  return f;
}

bool atlas_read_meta(atlas_internal_t *atlas) {
  if (!atlas) return false;

  FILE *f = atlas_open_anim((atlas_t*)atlas);
  if (!f) return false;

  const u64 framesize = sizeof(frame_t);
  int fseek_err;

  file_header_t header;
  fread(&header, sizeof(file_header_t), 1, f);

  group_header_t grp_h;
  u8 ani_ct, ani_nm_l;
  for (u8 i = 0; i < header.group_count && i < METADATA_CAPACITY; i++) {
    grpmeta_t *cur = &atlas->file_groups[i];
    fread(&grp_h, sizeof(group_header_t), 1, f);
    const u64 namelen = sizeof(cur->name) - 1;

    // load group metadata in RAM
    fgetpos(f, &cur->data_offset);
    strncpy(cur->name, grp_h.name, namelen);
    cur->name[namelen] = '\0';
    TraceLog(LOG_DEBUG, "Loading group_meta['%s']", cur->name);
    cur->ani_ct = grp_h.anim_count;
    cur->frm_ct = grp_h.frame_count;

    // skipping the stored data
    fseek_err = fseek(f, (cur->frm_ct * framesize), SEEK_CUR);
    for (int i = 0; i < grp_h.anim_count; i++) {
      fread(&ani_nm_l, sizeof(u8), 1, f);
      fseek(f, ani_nm_l, SEEK_CUR);
      fread(&ani_ct, sizeof(u8), 1, f);
      fseek(f, 1 + sizeof(u8) * ani_ct, SEEK_CUR);
    }

    if (fseek_err) {
      TraceLog(LOG_ERROR, "Atlas file might be corrupted");
      break;
    }
    ++(atlas->total_file_groups);
  }

  fclose(f);
  return true;
}

bool atlas_load
(atlas_t **atlas, const char *texture_path, const char *anim_path) {
  if (!atlas || (*atlas) || !texture_path || !anim_path) return false;

  atlas_internal_t *internal =
  (atlas_internal_t*)malloc(sizeof(atlas_internal_t));
  if (!internal) return false;

  *atlas = (atlas_t*)internal;

  internal->base.texture = LoadTexture(texture_path);
  const u16 path_len = sizeof(internal->base.anim_path) - 1;
  strncpy(internal->base.anim_path, anim_path, path_len);
  internal->base.anim_path[path_len] = '\0';
  for (u8 i = 0; i < ATLASGRP_LEN; i++) internal->idle_time[i] = -1;
  atlas_read_meta(internal);
  return true;
}

bool atlas_unload(atlas_t **atlas) {
  if (!atlas || !(*atlas)) return false;

  UnloadTexture((*atlas)->texture);
  atlas_internal_t *internal = (atlas_internal_t*)*atlas;
  free(internal);
  *atlas = NULL;
  return true;
}

i8 atlas_is_grp_loaded(atlas_t *atlas, const char *name) {
  for (i8 i = 0; i < atlas->loaded_count; i++) {
    u8 loaded = atlas->loaded_groups[i];
    if (strcmp(atlas->groups[loaded].name, name) == 0)
      return (i8)loaded;
  }
  return -1;
}

i8 internal_find_unloaded(atlas_internal_t *internal) {
  if (!internal) return ERRVAL;

  for (i8 i = 0; i < ATLASGRP_LEN; i++) {
    if (!internal->base.groups[i].loaded) return i;
  }

  return ERRVAL;
}

bool atlas_fetch_grpinfo
(atlas_internal_t *self, const char *name, grpmeta_t **info) {
  if (!self || !info) return false;

  const i64 namelen = ANIGRP_NAME_LEN;

  for (u8 i = 0; i < self->total_file_groups; i++) {
    if (strncmp(self->file_groups[i].name, name, namelen) == 0) {
      *info = &self->file_groups[i];
      return true;
    }
  }

  return false;
}

i32 int_max(i32 *arr, const i32 len) {
  if (!arr || len <= 0) return -1;

  i32 index = 0,
      max = arr[index];

  for (i32 i = 0; i < len; i++) {
    if (arr[i] > max) {
      max = arr[i];
      index = i;
    }
  }

  return index;
};

void count_idletime(atlas_internal_t *internal) {
  if (!internal) return;

  for (u8 i = 0; i < ATLASGRP_LEN; i++) {
    if (internal->idle_time[i] < 0) continue;
    internal->idle_time[i]++;
  }
}

// loads a group and may replace one if needed (LRU)
i8 atlas_load_grp(atlas_t *atlas, const char *grp_name) {
  if (!atlas) return ERRVAL;
  FILE *f = atlas_open_anim(atlas); if (!f) return ERRVAL;

  atlas_internal_t *internal = (atlas_internal_t*)atlas;
  grpmeta_t *grpinfo;
  bool ok = atlas_fetch_grpinfo(internal, grp_name, &grpinfo);
  if (!ok) { fclose(f); return ERRVAL; }

  int err = fsetpos(f, &(grpinfo->data_offset));
  if (err) { fclose(f); return ERRVAL; }

  count_idletime(internal);
  bool evicting = false;
  i32 to_replace = internal_find_unloaded(internal);
  if (to_replace == ERRVAL) {
    evicting = true;
    to_replace = int_max(internal->idle_time, ATLASGRP_LEN);
  }
  TraceLog(
    LOG_DEBUG,
    evicting ? "Atlas about to evict a group" : "Atlas about to add a group"
  );
  anim_group_t *group = &internal->base.groups[to_replace];

  // loading frames
  group->frames.count = grpinfo->frm_ct;
  fread(group->frames.values, sizeof(frame_t), grpinfo->frm_ct, f);

  // loading anims
  u8 anim_name_len;
  char anim_name[ANIM_NAME_LEN];

  group->anim_count = grpinfo->ani_ct;

  for (int i = 0; i < grpinfo->ani_ct; i++) {
    animation_t *cur_ani = &group->anims[i];
    fread(&anim_name_len, sizeof(u8), 1, f);
    TraceLog(LOG_DEBUG, "Anim name len = %d", anim_name_len);
    if (anim_name_len > ANIM_NAME_LEN) {
      TraceLog(LOG_ERROR, "Animation name in file is too long");
      fclose(f);
      return ERRVAL;
    }

    // the animation name nightmare
    fread(anim_name, sizeof(u8), anim_name_len, f);
    strncpy(group->anims[i].name, anim_name, ANIM_NAME_LEN);
    if (anim_name_len < ANIM_NAME_LEN) {
      group->anims[i].name[anim_name_len] = '\0';
    } else {
      group->anims[i].name[ANIM_NAME_LEN-1] = '\0';
    }
    TraceLog(LOG_DEBUG, "Animation name: %s", group->anims[i].name);

    fread(&cur_ani->frame_count, sizeof(u8), 1, f);
    TraceLog(LOG_DEBUG, "Loading local frame len (%d)", cur_ani->frame_count);
    fread(&cur_ani->loop, sizeof(bool), 1, f);
    fread(cur_ani->frames, sizeof(u8), cur_ani->frame_count, f);
    for (int i = 0; i < cur_ani->frame_count; i++)
      TraceLog(LOG_DEBUG, "Loading local frame %d", cur_ani->frames[i]);
  }

  if (!evicting) {
    internal->base.loaded_count++;
    internal->base.loaded_groups[internal->base.loaded_count-1] = to_replace;
  }

  group->loaded = true;
  internal->idle_time[to_replace] = 0;

  fclose(f);
  return to_replace;
}

void reset_idletime(atlas_internal_t *internal, u8 index) {
  if (!internal || index >= ATLASGRP_LEN) return;
  internal->idle_time[index] = 0;
}

i8 atlas_grp_req(atlas_t *atlas, const char *grp_name) {
  if (!atlas) return false;
  i8 grp_id = atlas_is_grp_loaded(atlas, grp_name);

  if (grp_id != -1) {
    reset_idletime((atlas_internal_t*)atlas, (u8)grp_id);
    return (u8)grp_id;
  }

  // in case of MISS, load it from the private metadata
  grp_id = atlas_load_grp(atlas, grp_name);

  return grp_id;
}

bool sprite_set_grp(sprite_t *self, const char *group_tag) {
  if (!self) return false;

  i8 grp_id = atlas_grp_req(self->atlas, group_tag);
  if (grp_id == -1) {
    TraceLog(LOG_ERROR, "Failed requesting group: '%s'", group_tag);
    return false;
  }
  self->grp_id = (u8)grp_id;
  return true;
}

