#ifndef CONFIG_H
#define CONFIG_H

#include "types.h"

typedef struct config {
  u8 window_scale;
} config_t;

config_t config_default();
bool config_load(config_t *self);

#endif
