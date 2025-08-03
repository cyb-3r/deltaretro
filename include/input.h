#ifndef INPUT_H
#define INPUT_H

#include "types.h"

enum input_commmands {
  INPUT_LEFT,
  INPUT_UP,
  INPUT_RIGHT,
  INPUT_DOWN,

  INPUT_PRIM,
  INPUT_SECOND,

  INPUT_PAUSE,
  INPUT_MENU,

  INPUT_COUNT
};

enum input_masks {

  INPUT_M_NONE    = 0,

  INPUT_M_LEFT    = (1 << 0),
  INPUT_M_UP      = (1 << 1),
  INPUT_M_RIGHT   = (1 << 2),
  INPUT_M_DOWN    = (1 << 3),

  INPUT_M_PRIM    = (1 << 4),
  INPUT_M_SECOND  = (1 << 5),

  INPUT_M_PAUSE   = (1 << 6),
  INPUT_M_MENU    = (1 << 7),
};

typedef struct input {
  u16 commands[INPUT_COUNT];
  u8 previous;
  u8 status;
} input_t;
typedef struct input ipt_t;

input_t inputs_default(void);
bool input_load(input_t*);
void input_update(input_t*);
bool input_is_pressed(input_t*, u8 command);
bool input_is_released(input_t*, u8 command);
bool input_is_down(input_t*, u8 command);

#endif
