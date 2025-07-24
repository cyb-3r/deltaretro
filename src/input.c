#include "../include/input.h"
#include "../lib/raylib.h"
#include "../lib/toml.h"

#ifndef CONFIG_PATH
#define CONFIG_PATH "resources/config.toml"
#endif

input_t inputs_default() {
  return (input_t){
    .commands = {
      [INPUT_LEFT] = KEY_LEFT,
      [INPUT_UP] = KEY_UP,
      [INPUT_RIGHT] = KEY_RIGHT,
      [INPUT_DOWN] = KEY_DOWN,
      [INPUT_PRIM] = KEY_Z,
      [INPUT_SECOND] = KEY_A,
      [INPUT_PAUSE] = KEY_ENTER,
      [INPUT_MENU] = KEY_E,
    }
  };
}

const char *get_input_name(u8 input_id) {
  if (input_id >= INPUT_COUNT) return "none";
  const char *input_keys[INPUT_COUNT] = {
    [INPUT_LEFT]   = "left",
    [INPUT_RIGHT]  = "right",
    [INPUT_UP]     = "up",
    [INPUT_DOWN]   = "down",
    [INPUT_PRIM]   = "primary",
    [INPUT_SECOND] = "secondary",
    [INPUT_PAUSE]  = "pause",
    [INPUT_MENU]   = "menu"
  };
  return input_keys[input_id];
}

bool command_load(input_t *self, toml_result_t *res, u8 input_id) {
  if (input_id >= INPUT_COUNT || !self || !res) return false;
  const char *input_keys[INPUT_COUNT] = {
    [INPUT_LEFT]   = "inputs.left",
    [INPUT_RIGHT]  = "inputs.right",
    [INPUT_UP]     = "inputs.up",
    [INPUT_DOWN]   = "inputs.down",
    [INPUT_PRIM]   = "inputs.primary",
    [INPUT_SECOND] = "inputs.secondary",
    [INPUT_PAUSE]  = "inputs.pause",
    [INPUT_MENU]   = "inputs.menu"
  };
  toml_datum_t input = toml_seek(res->toptab, input_keys[input_id]);
  if (input.type == TOML_INT64) {
    self->commands[input_id] = input.u.int64;
    return true;
  }
  else return false;
}

void input_load(input_t *self) {
  TraceLog(LOG_INFO, "Loading inputs from config");
  toml_result_t input_data = toml_parse_file_ex(CONFIG_PATH);
  if (!input_data.ok) {
    TraceLog(LOG_ERROR, "Couldn't load inputs");
    return;
  }
  for (u8 i = 0; i < INPUT_COUNT; i++) {
    const char *input_name = get_input_name(i);
    TraceLog(LOG_DEBUG, "Loading '%s' input", input_name);
    bool ok = command_load(self, &input_data, i);
    TraceLog(LOG_DEBUG,
      !ok ? "Failed loading '%s' input" : "Loaded '%s' input successfully",
      input_name
    );
  }
  TraceLog(LOG_INFO, "Finished loading inputs");
  toml_free(input_data);
}

u8 input_read(input_t *self) {
  u8 new_status = 0;
  bool
  left = IsKeyDown(self->commands[INPUT_LEFT]),
  right = IsKeyDown(self->commands[INPUT_RIGHT]),
  up = IsKeyDown(self->commands[INPUT_UP]),
  down = IsKeyDown(self->commands[INPUT_DOWN]);

  if (right != left) {
    if (right) new_status |= INPUT_M_RIGHT;
    if (left) new_status |= INPUT_M_LEFT;
  }

  if (down != up) {
    if (down) new_status |= INPUT_M_DOWN;
    if (up) new_status |= INPUT_M_UP;
  }

  if (IsKeyDown(self->commands[INPUT_PRIM])) new_status |= INPUT_M_PRIM;
  if (IsKeyDown(self->commands[INPUT_SECOND])) new_status |= INPUT_M_SECOND;
  if (IsKeyDown(self->commands[INPUT_PAUSE])) new_status |= INPUT_M_PAUSE;
  if (IsKeyDown(self->commands[INPUT_MENU])) new_status |= INPUT_M_MENU;

  return new_status;
}

void input_update(input_t *self) {
  self->previous = self->status;
  self->status = input_read(self);
}

bool input_is_pressed(input_t *self, u8 command) {
  u8 mask = (1 << command);
  return (self->status & mask) && !(self->previous & mask);
}

bool input_is_released(input_t *self, u8 command) {
  u8 mask = (1 << command);
  return !(self->status & mask) && (self->previous & mask);
}

bool input_is_down(input_t *self, u8 command) {
  u8 mask = (1 << command);
  return (self->status & mask);
}
