#include "stage-game.h"
// #include "scene.h"
#include "ui.h"

/*== Utils ==*/
void hud_draw(const char *name, i32 pts) {
  draw_text(TextFormat("%s", name), 1, UI_UNIT);
  draw_text(TextFormat("P%04i", pts), UI_UNIT_N(10), UI_UNIT);
}

/*== Local vars ==*/
static tex_t  s_bg;
static rtex_t s_surf_game;

void stage_game_begin(sys_t *sys) {
  TraceLog(LOG_INFO, "Entering TEST ROOM");

  s_bg = LoadTexture("resources/gfx/tlmp-board.png");
  s_surf_game = LoadRenderTexture(
    sys->window.width, sys->window.height - UI_UNIT_N(2)
  );
}

void stage_game_update(sys_t *sys) {
  game_update(&sys->game, &sys->inputs);
}

void stage_game_draw(sys_t *sys) {
  const col_t bg_col = BLACK;
  BeginTextureMode(s_surf_game);
    ClearBackground(bg_col);
    DrawTexture(s_bg, 0, 0, WHITE);
    game_draw(&sys->game);
  EndTextureMode();

  BeginTextureMode(sys->surf_main);
    ClearBackground(bg_col);
    hud_draw(sys->game.plr_data.name, sys->game.plr_data.pts);
    surf_draw(&s_surf_game.texture, 0, 16, 1);
  EndTextureMode();
}

void stage_game_end(sys_t *sys) {
  TraceLog(LOG_INFO, "Exiting GAME");
  UnloadTexture(s_bg);
  UnloadRenderTexture(s_surf_game);
}
