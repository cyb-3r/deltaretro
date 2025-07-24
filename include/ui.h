#ifndef UI_H
#define UI_H

#include "../lib/raylib.h"

void ui_init();
void ui_deinit();
void draw_text_col(const char *text, Vector2 pos, Color color);
void draw_text(const char *text, Vector2 pos);

#endif
