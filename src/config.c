#include "../include/config.h"
#include "../lib/toml.h"
#include "../lib/raylib.h"

#include <stdio.h>
#include <string.h>

config_t config_default() {
  return (config_t){
    .window_scale = 1
  };
}

bool config_load(config_t *self, const char *path) {
  TraceLog(LOG_INFO, "Loading configs");

  strncpy(self->path, path, CFG_PATH_LEN-1);
  self->path[CFG_PATH_LEN-1] = '\0';

  FILE *f = fopen(self->path, "r");
  if (!f) {
    TraceLog(LOG_ERROR, "Failed loading config");
    return false;
  }

  toml_result_t config_data = toml_parse_file(f);
  if (config_data.ok) {
    toml_datum_t scale = toml_seek(config_data.toptab, "config.scale");
    self->window_scale = scale.u.int64;
    TraceLog(LOG_INFO, "Configs loaded successfully");
  } else {
    TraceLog(LOG_ERROR, config_data.errmsg);
    toml_free(config_data);
    fclose(f);
    return false;
  }

  toml_free(config_data);
  fclose(f);
  return true;
}

