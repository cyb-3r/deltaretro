#include "sysmode/file.h"
#include "raylib.h"
#include "ui.h"
#include "save.h"

/*== Utils ==*/
void draw_cursor(int);
void sd_draw(sd_t*, int);

static Texture bg;
static int select;
static sd_t save_data[SAVE_MAX];

void mode_file_begin(sys_t *sys) {
  TraceLog(LOG_INFO, "Entering MENU");
  select = 0;
  bg = LoadTexture("resources/gfx/bg-save.png");

  for (int i = 0; i < SAVE_MAX; i++) sd_load(&save_data[i], (i + 1));
}

void mode_file_update(sys_t *sys) {
  if (input_is_pressed(&sys->inputs, INPUT_PRIM)) {
    if (select < 3) {
      sys->save_slot = select + 1;
      for (int i = 0; i < SAVE_NAME_LEN; i++)
        sys->game.plr_data.name[i] = save_data[select].name[i];
      sys->game.plr_data.pts = save_data[select].points;
      sys->game.plr_data.lv = save_data[select].lv;
      system_change_state(sys, SYS_TEST);
      return;
    } else {
      system_change_state(sys, SYS_EXIT);
      return;
    }
  }

  if (input_is_pressed(&sys->inputs, INPUT_MENU))
    select = (select + 1) % 6;
}

void mode_file_draw(sys_t *sys) {
  BeginTextureMode(sys->surf_main);
    ClearBackground(BLACK);
    DrawTexture(bg, 0, 0, WHITE);
    draw_text("FILE SELECT", 16, 8);
    draw_cursor(select);
    const int y_off = 32;
    for (int i = 0; i < SAVE_MAX; i++) {
      const int loop_off = i * (4 * 8);
      sd_draw(&save_data[i], y_off + loop_off);
    }
    draw_text("COPY   ERASE   END", 24, W_HEIGHT - 16);
  EndTextureMode();

  BeginDrawing();
    ClearBackground(BLACK);
    surf_draw(&sys->surf_main.texture, 0, 0, sys->cfg.window_scale);
  EndDrawing();
}

void mode_file_end(sys_t *sys) {
  TraceLog(LOG_INFO, "Exiting MENU");
  UnloadTexture(bg);
}

void draw_cursor(int select) {
  const Color c = RED;
  const v2_t size = { 7, 7 };
  v2_t pos = EMPTY;
  switch (select) {
    case 0: pos = (v2_t){ 24, 32 };   break;
    case 1: pos = (v2_t){ 24, 64 };   break;
    case 2: pos = (v2_t){ 24, 96 };   break;
    case 3: pos = (v2_t){ 16, 128 };  break;
    case 4: pos = (v2_t){ 72, 128 };  break;
    case 5: pos = (v2_t){ 136, 128 }; break;
    default: return;
  }
  DrawRectangle(pos.x, pos.y, size.x, size.y,c);
}

void sd_draw(sd_t *data, int y) {
  DrawRectangleRec((rec_t){ 40, y, 16, 16 }, BLUE);
  draw_text(TextFormat("%s", data->name), 64, y);
  draw_text(TextFormat("%04i LV-%i", data->points, data->lv), 80,y + 8);
}
