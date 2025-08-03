#ifndef CONFIG_H
#define CONFIG_H

#include "types.h"

#define CFG_PATH_LEN 0x100
#ifdef DEBUG
#define TITLE "MANTLE DEBUG"
#else
#define TITLE "MANTLE"
#endif

typedef struct config {
  char path[CFG_PATH_LEN];
  u8 window_scale;
} config_t;

config_t config_default();
bool config_load(config_t*, const char *path);

#endif
