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
