#include <stdio.h>
#include "raylib.h"
#include "save.h"

#define ITEM_AMOUNT 8
#define SAVE_LOC "./save.bin"

#define SAVE_EMPTY ((sd_t){"Kris", 0, 0})

struct sf_header {
  char magic[8];
  u8 version;
};

struct sd_header {
  bool empty;
};

bool sf_create_empty(void);

save_data_t sd_create(const char *name) {
  save_data_t new_save = { 0 };
  for (int i = 0; i < SAVE_NAME_LEN - 1; i++)
    new_save.name[i] = name[i];
  new_save.name[SAVE_NAME_LEN - 1] = '\0';
  new_save.lv = 1;
  return new_save;
}

bool sf_create_empty(void) {
  sd_t saves[SAVE_MAX] = { SAVE_EMPTY, SAVE_EMPTY, SAVE_EMPTY };

  FILE *f = fopen(SAVE_LOC, "wb");
  if (!f) {
    TraceLog(LOG_ERROR, "Couldn't create save file");
    return false;
  }

  i64 out = fwrite(saves, sizeof(sd_t), SAVE_MAX, f);
  if (out < SAVE_MAX) {
    TraceLog(LOG_ERROR, "Couldn't write save data to file");
    return false;
  }

  TraceLog(LOG_INFO, "Created empty save file");
  fclose(f);
  return true;
}

bool sf_check() {
  FILE *f = fopen(SAVE_LOC, "rb");
  if (!f) return false;
  fclose(f);
  return true;
}

bool sf_init() {
  TraceLog(LOG_INFO, "Initializing save file");
  bool ok = sf_check();
  if (ok) {
    TraceLog(LOG_INFO, "Save file exists, success");
    return true;
  }
  ok = sf_create_empty();
  if (!ok) return false;
  TraceLog(LOG_INFO, "Initialized save file successfully");
  return true;
}

bool sd_write(sd_t *self, const int slot) {
  if (!self || (slot > 3 || slot <= 0)) return false;
  TraceLog(LOG_INFO, "Saving data..");

  FILE *f = fopen(SAVE_LOC, "rb+");
  if (!f) TraceLog(LOG_INFO, "Save file was not found. Creating file");
  bool ok = sf_create_empty();
  if (!ok) {
    TraceLog(LOG_ERROR, "Failed saving data to file");
    return false;
  }
  f = fopen(SAVE_LOC, "rb+");
  fseek(f, sizeof(sd_t) * (slot - 1), SEEK_SET);
  i64 out = fwrite(self, sizeof(sd_t), 1, f);
  if (out < 1) {
    TraceLog(LOG_ERROR, "Failed saving data to file");
    fclose(f);
    return false;
  }

  TraceLog(LOG_INFO, "File[%d] saved successfully", slot);
  fclose(f);
  return true;
}

bool sd_load(sd_t *self, const int slot) {
  if (!self || (slot > 3 || slot <= 0)) return false;
  TraceLog(LOG_INFO, "Loading file[%d]", slot);

  FILE *f = fopen(SAVE_LOC, "rb");
  if (!f) {
    TraceLog(LOG_INFO, "Save file was not found");
    return false;
  }
  fseek(f, sizeof(sd_t) * (slot - 1), SEEK_SET);
  fread(self, sizeof(sd_t), 1, f);

  TraceLog(LOG_INFO, "File[%d] loaded successfully", slot);
  fclose(f);
  return true;
}

bool sd_erase(const int slot) {
  if ((slot > 3 || slot <= 0)) return false;
  TraceLog(LOG_INFO, "Erasing file[%d]", slot);

  sd_t empty = SAVE_EMPTY;
  FILE *f = fopen(SAVE_LOC, "rb+");
  if (!f) {
    TraceLog(LOG_ERROR, "Save file was not found");
    return false;
  }
  fseek(f, sizeof(sd_t) * (slot - 1), SEEK_SET);
  i64 out = fwrite(&empty, sizeof(sd_t), 1, f);
  if (out < 1) {
    TraceLog(LOG_ERROR, "Failed reseting data in file");
    fclose(f);
    return false;
  }

  TraceLog(LOG_ERROR, "File[%d] successfully erased", slot);
  fclose(f);
  return true;
}

bool sd_copy(const int from, const int to) {
  if ((from > 3 || from <= 0) || (to > 3 || to <= 0)) return false;
  TraceLog(LOG_INFO, "Copying file[%d] -> file[%d]", from, to);

  sd_t to_copy;
  FILE *f = fopen(SAVE_LOC, "rb+");
  if (!f) {
    TraceLog(LOG_ERROR, "Save file was not found");
    return false;
  }
  fseek(f, sizeof(sd_t) * (from - 1), SEEK_SET);
  fread(&to_copy, sizeof(sd_t), 1, f);

  fseek(f, sizeof(sd_t) * (to - 1), SEEK_SET);
  i64 out = fwrite(&to_copy, sizeof(sd_t), 1, f);
  if (out < 1) {
    TraceLog(LOG_ERROR, "Failed reseting data in file");
    fclose(f);
    return false;
  }

  TraceLog(LOG_ERROR, "File[%d] successfully copied to file[%d]", from, to);
  fclose(f);
  return true;
}

bool sd_read(void) {
  TraceLog(LOG_INFO, "Attempting to read file data");
  FILE *f = fopen(SAVE_LOC, "rb");
  if (!f) {
    TraceLog(LOG_ERROR, "Failed to open save file");
    return false;
  }

  sd_t save = { 0 };
  for (int i = 0; i < SAVE_MAX; i++) {
    fread(&save, sizeof(sd_t), 1, f);
    TraceLog(LOG_INFO,
      "File[%d] = { name: %s, pts: %d, lv: %d }",
      (i + 1), save.name, save.points, save.lv
    );
  }

  TraceLog(LOG_INFO, "Attempt successful");
  fclose(f);
  return true;
}
