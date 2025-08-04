#ifndef UI_H
#define UI_H

#include "../lib/raylib.h"
#include "types.h"

void ui_init(void);
void ui_deinit(void);
void draw_text_col(const char *text, i32 x, i32 y, Color color);
void draw_text(const char *text, i32 x, i32 y);

void lifebar_draw(i32 life_max, i32 life, i32 x, i32 y, u8 width);

#endif
