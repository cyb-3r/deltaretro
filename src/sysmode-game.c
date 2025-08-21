#include "sysmode/game.h"
#include "raylib.h"
#include "scene.h"
#include "ui.h"

/*== Utils ==*/
void hud_draw(const char *name, int pts) {
  draw_text(TextFormat("%s", name), 8, 8);
  draw_text(TextFormat("P%04i", pts), 8 * 10, 8);
}

static Texture bg;
static RenderTexture surf_game;

void mode_game_begin(sys_t *sys) {
  TraceLog(LOG_INFO, "Entering TEST ROOM");

  bg = LoadTexture("resources/gfx/tlmp-board.png");
  surf_game = LoadRenderTexture(
    sys->window.width, sys->window.height - 16
  );

  /* Test Collisions */
  sys->game.world.cur_scr.tile_type[0][0] = TTP_SOLID;
}

void mode_game_update(sys_t *sys) {
  game_update(&sys->game, &sys->inputs);
}

void mode_game_draw(sys_t *sys) {
  BeginTextureMode(surf_game);
    ClearBackground(BLACK);
    DrawTexture(bg, 0, 0, WHITE);
    game_draw(&sys->game);
  EndTextureMode();

  BeginTextureMode(sys->surf_main);
    ClearBackground(BLACK);
    hud_draw(sys->game.plr_data.name, sys->game.plr_data.pts);
    surf_draw(&surf_game.texture, 0, 16, 1);
  EndTextureMode();

  BeginDrawing();
    ClearBackground(BLACK);
    surf_draw(&sys->surf_main.texture, 0, 0, sys->cfg.window_scale);
  EndDrawing();
}

void mode_game_end(sys_t *sys) {
  TraceLog(LOG_INFO, "Exiting GAME");
  UnloadTexture(bg);
  UnloadRenderTexture(surf_game);
}
