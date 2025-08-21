#include "sysmode-game.h"
// #include "scene.h"
#include "ui.h"

/*== Utils ==*/
void hud_draw(const char *name, i32 pts) {
  draw_text(TextFormat("%s", name), 8, 8);
  draw_text(TextFormat("P%04i", pts), 8 * 10, 8);
}

/*== Local vars ==*/
static tex_t  s_bg;
static rtex_t s_surf_game;

void mode_game_begin(sys_t *sys) {
  TraceLog(LOG_INFO, "Entering TEST ROOM");

  s_bg = LoadTexture("resources/gfx/tlmp-board.png");
  s_surf_game = LoadRenderTexture(
    sys->window.width, sys->window.height - 16
  );
}

void mode_game_update(sys_t *sys) {
  game_update(&sys->game, &sys->inputs);
}

void mode_game_draw(sys_t *sys) {
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

void mode_game_end(sys_t *sys) {
  TraceLog(LOG_INFO, "Exiting GAME");
  UnloadTexture(s_bg);
  UnloadRenderTexture(s_surf_game);
}
