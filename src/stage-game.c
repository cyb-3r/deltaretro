#include "stage-game.h"
#include "system.h"
#include "ui.h"

/*== Utils ==*/
void hud_draw(const char *name, i32 pts);

struct context_game {
  tex_t   background;
  rtex_t  surface;
};

void stage_game_begin(sys_t *sys) {
  TraceLog(LOG_INFO, "Entering GAME");

  system_flush_temp(sys);
  struct context_game *state = (struct context_game*)sys->temp;
  state->background = LoadTexture("resources/gfx/tlmp-board.png");
  state->surface = LoadRenderTexture(
    sys->window.width, sys->window.height - UI_UNIT_N(2)
  );
}

void stage_game_update(sys_t *sys) {
  game_update(&sys->game, &sys->inputs, sys->delta);
}

void stage_game_draw(sys_t *sys) {
  struct context_game *state = (struct context_game*)sys->temp;
  const col_t bg_col = BLACK;

  BeginTextureMode(state->surface);
    ClearBackground(bg_col);
    DrawTexture(state->background, 0, 0, WHITE);
    game_draw(&sys->game);
  EndTextureMode();

  BeginTextureMode(sys->surface);
    ClearBackground(bg_col);
    hud_draw(sys->game.plr_data.name, sys->game.plr_data.pts);
    surf_draw(&state->surface.texture, 0, 16, 1);
  EndTextureMode();
}

void stage_game_end(sys_t *sys) {
  TraceLog(LOG_INFO, "Exiting GAME");
  struct context_game *state = (struct context_game*)sys->temp;
  UnloadTexture(state->background);
  UnloadRenderTexture(state->surface);
}

/*== Utils impl ==*/
void hud_draw(const char *name, i32 pts) {
  draw_text(TextFormat("%s", name), 1, UI_UNIT);
  draw_text(TextFormat("P%04i", pts), UI_UNIT_N(10), UI_UNIT);
}
