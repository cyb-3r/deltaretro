#ifndef UI_H
#define UI_H

#include "raylib.h"
#include "types.h"

#define UI_UNIT 8
#define UI_UNIT_N(n) ((n) * (UI_UNIT))
#define UI_PX_TO_UNIT(x) ((i32)(x) / (UI_INIT))

void ui_init(void);
void ui_deinit(void);
void draw_text_col(const char *text, i32 x, i32 y, Color color);
void draw_text(const char *text, i32 x, i32 y);

void lifebar_draw(i32 life_max, i32 life, i32 x, i32 y, u8 width);

#endif
