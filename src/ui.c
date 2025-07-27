#include "../include/ui.h"

#define FONT_PATH "resources/gfx/font-mono.png"
#define FONT_SPACING 0.0f

static Font main_font = {0};

void ui_init() {
  main_font = LoadFont(FONT_PATH);
}

void ui_deinit() {
  UnloadFont(main_font);
}

void draw_text_col(const char *text, Vector2 pos, Color color) {
  DrawTextEx(main_font, text, pos, main_font.baseSize, FONT_SPACING, color);
}

void draw_text(const char *text, Vector2 pos) {
  DrawTextEx(main_font, text, pos, main_font.baseSize, FONT_SPACING, WHITE);
}

void lifebar_draw(i32 life_max, i32 life, i32 x, i32 y, u8 width) {
  DrawRectangle(x, y, width, 8, DARKGRAY);
  DrawRectangle(x, y,
    (int)(width * ((float)life/(float)life_max)),
    8, WHITE
  );
}
