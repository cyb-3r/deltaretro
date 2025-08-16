#include "ui.h"

#define FONT_PATH "resources/gfx/font-mono.png"
#define FONT_SPACING 0.0f

static Font main_font = {0};

void ui_init() {
  TraceLog(LOG_INFO, "Loading game font");
  main_font = LoadFont(FONT_PATH);
}

void ui_deinit() {
  TraceLog(LOG_INFO, "Unloading game font");
  UnloadFont(main_font);
}

Vector2 adjust_pos(i32 x, i32 y) {
  return (v2_t) {x, y - 2};
}

void draw_text_col(const char *text, i32 x, i32 y, Color color) {
  DrawTextEx(main_font,
    text,
    adjust_pos(x, y),
    main_font.baseSize,
    FONT_SPACING,
    color
  );
}

void draw_text(const char *text, i32 x, i32 y) {
  DrawTextEx(main_font,
    text,
    adjust_pos(x, y),
    main_font.baseSize,
    FONT_SPACING,
    WHITE
  );
}

void lifebar_draw(i32 life_max, i32 life, i32 x, i32 y, u8 width) {
  DrawRectangle(x, y, width, 8, DARKGRAY);
  DrawRectangle(x, y,
    (int)(width * ((f32)life/(f32)life_max)),
    8, WHITE
  );
}
