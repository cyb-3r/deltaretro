#include "sysmode-file.h"
#include "ui.h"
#include "save.h"

/*== Utils ==*/
void draw_cursor(i8);
void sd_draw(sd_t*, i16);

enum cursor_pos {
  POS_SAVE_1,
  POS_SAVE_2,
  POS_SAVE_3,
  POS_COPY,
  POS_ERASE,
  POS_END,
};

/*== Local vars ==*/
static tex_t  s_bg;
static i8     s_select;
static sd_t   s_save_data[SAVE_MAX];

void mode_file_begin(sys_t *sys) {
  TraceLog(LOG_INFO, "Entering MENU");

  s_select = 0;
  s_bg = LoadTexture("resources/gfx/bg-save.png");

  bool sd_ok = sf_init();
  if (!sd_ok) {
    TraceLog(LOG_ERROR, "Save system is broken :(");
    system_exit(sys);
  }

  for (i8 i = 0; i < SAVE_MAX; i++)
    sd_load(&s_save_data[i], (i + 1));
}

void mode_file_update(sys_t *sys) {
  if (input_is_pressed(&sys->inputs, INPUT_PRIM)) {
    if (s_select < POS_COPY) {
      sys->save_slot = s_select + 1;

      for (int i = 0; i < SAVE_NAME_LEN; i++)
        sys->game.plr_data.name[i] = s_save_data[s_select].name[i];
      sys->game.plr_data.pts = s_save_data[s_select].points;
      sys->game.plr_data.lv = s_save_data[s_select].lv;

      system_change_state(sys, SYS_TEST);
      return;
    } else {
      system_exit(sys);
      return;
    }
  }

  if (input_is_pressed(&sys->inputs, INPUT_MENU))
    s_select = (s_select + 1) % 6;
}

void mode_file_draw(sys_t *sys) {
  BeginTextureMode(sys->surf_main);
    ClearBackground(BLACK);
    DrawTexture(s_bg, 0, 0, WHITE);
    draw_text("FILE SELECT", 16, 8);
    draw_cursor(s_select);
    const i8 y_off = 32;
    for (i8 i = 0; i < SAVE_MAX; i++) {
      const i16 loop_off = i * (4 * 8);
      sd_draw(&s_save_data[i], y_off + loop_off);
    }
    draw_text("COPY   ERASE   END", 24, win_get_h(&sys->window) - 16);
  EndTextureMode();
}

void mode_file_end(sys_t *sys) {
  TraceLog(LOG_INFO, "Exiting MENU");
  UnloadTexture(s_bg);
}

/*== Utils impl ==*/
void draw_cursor(i8 select) {
  const col_t c = RED;
  const v2_t size = { 7, 7 };
  v2_t pos = EMPTY;
  switch (select) {
    case POS_SAVE_1:  pos = (v2_t){ 24, 32 };   break;
    case POS_SAVE_2:  pos = (v2_t){ 24, 64 };   break;
    case POS_SAVE_3:  pos = (v2_t){ 24, 96 };   break;
    case POS_COPY:    pos = (v2_t){ 16, 128 };  break;
    case POS_ERASE:   pos = (v2_t){ 72, 128 };  break;
    case POS_END:     pos = (v2_t){ 136, 128 }; break;
    default: return;
  }
  DrawRectangle(pos.x, pos.y, size.x, size.y,c);
}

void sd_draw(sd_t *data, i16 y) {
  DrawRectangleRec((rec_t){ 40, y, 16, 16 }, BLUE);
  draw_text(TextFormat("%s", data->name), 64, y);
  draw_text(TextFormat("%04i LV-%i", data->points, data->lv), 80,y + 8);
}
