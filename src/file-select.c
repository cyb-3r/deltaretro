#include "file-select.h"
#include "types.h"
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

struct context_file_sel {
  tex_t background;
  i8    select;
  sd_t  save_data[SAVE_MAX];
};

void stage_file_begin(sys_t *sys) {
  TraceLog(LOG_INFO, "Entering MENU");

  system_flush_temp(sys);
  struct context_file_sel *state = (struct context_file_sel*)sys->temp;
  state->background = LoadTexture("resources/gfx/bg-save.png");

  bool sd_ok = sf_init();
  if (!sd_ok) {
    TraceLog(LOG_ERROR, "Save system is broken :(");
    system_exit(sys);
  }

  for (i8 i = 0; i < SAVE_MAX; i++)
    sd_load(&state->save_data[i], (i + 1));
}

void stage_file_update(sys_t *sys) {
  struct context_file_sel *state = (struct context_file_sel*)sys->temp;

  if (input_is_pressed(&sys->inputs, INPUT_PRIM)) {
    if (state->select < POS_COPY) {
      sys->save_slot = state->select + 1;

      for (int i = 0; i < SAVE_NAME_LEN; i++)
        sys->game.plr_data.name[i] = state->save_data[state->select].name[i];

      sys->game.plr_data.pts = state->save_data[state->select].points;
      sys->game.plr_data.lv = state->save_data[state->select].lv;

      system_change_state(sys, SYS_GAME);
      return;
    } else {
      system_exit(sys);
      return;
    }
  }

  if (input_is_pressed(&sys->inputs, INPUT_MENU))
    state->select = (state->select + 1) % 6;
}

void stage_file_draw(sys_t *sys) {
  struct context_file_sel *state = (struct context_file_sel*)sys->temp;

  BeginTextureMode(sys->surface);
    ClearBackground(BLACK);
    DrawTexture(state->background, 0, 0, WHITE);
    draw_text("FILE SELECT", UI_UNIT_N(2), UI_UNIT);
    draw_cursor(state->select);
    const i8 y_off = UI_UNIT_N(4);
    for (i8 i = 0; i < SAVE_MAX; i++) {
      const i16 loop_off = i * y_off;
      sd_draw(&state->save_data[i], y_off + loop_off);
    }
    draw_text(
      "COPY   ERASE   END",
      UI_UNIT_N(3),
      sys->window.height - UI_UNIT_N(2)
    );
  EndTextureMode();
}

void stage_file_end(sys_t *sys) {
  struct context_file_sel *state = (struct context_file_sel*)sys->temp;

  TraceLog(LOG_INFO, "Exiting MENU");
  UnloadTexture(state->background);
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
