#ifndef SAVE_H
#define SAVE_H

#include "types.h"

#define SAVE_MAX 3
#define SAVE_NAME_LEN 8

typedef struct item_handle {
  u8 id;
  u8 quantity;
} item_handle_t;

typedef struct save_data {
  char  name[SAVE_NAME_LEN];
  i32   points;
  u8    lv;
} save_data_t;
typedef struct save_data sd_t;

sd_t sd_create(const char*);
// file io
bool sf_check(void);
bool sf_init(void);

bool sd_write(sd_t*, const int slot);
bool sd_load(sd_t*, const int slot);
bool sd_erase(const int slot);
bool sd_copy(const int from, const int to);
// tests
bool sd_read(void);

#endif // SAVE_H
