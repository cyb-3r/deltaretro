#include "../include/game.h"
#include "../include/player.h"

#define W_WIDTH 256
#define W_HEIGHT 224
#define HUD_HEIGHT 32

void game_init(game_t *self) {
  self->player.life_max = 20;
  self->player.life = 20;
  self->player.x = ((float)W_WIDTH / 2.0f) - 8.0f;
  self->player.y = ((float)(W_HEIGHT - HUD_HEIGHT) / 2.0f) - 8.0f;
}

void update_delta(game_t *self) {
  self->delta = GetFrameTime();
}

void plr_mvmt(game_t *self, ipt_t *ipt) {
  ent_t *plr = &self->player;

  bool
  left =    input_is_down(ipt, INPUT_LEFT),
  right =   input_is_down(ipt, INPUT_RIGHT),
  up =      input_is_down(ipt, INPUT_UP),
  down =    input_is_down(ipt, INPUT_DOWN);

  if (!left && !right && !up && !down) {
    plr->state = IDLE;
  } else plr->state = MOVING;

  plr->xsp = (plr->state == MOVING) ? ((right - left) * ENT_SPD) : 0;
  plr->ysp = (plr->state == MOVING) ? ((down - up) * ENT_SPD) : 0;

  if (right)  plr->facing = ENT_DIR_R;
  if (left)   plr->facing = ENT_DIR_L;
  if (down)   plr->facing = ENT_DIR_D;
  if (up)     plr->facing = ENT_DIR_U;
}

void plr_update()__attribute__((unused));

void game_update(game_t *self, ipt_t *ipt) {
  update_delta(self);

  plr_mvmt(self, ipt);
  ent_apply_spd(&self->player);
}

void game_draw(game_t *self) {
  DrawRectangleRec(ent_rect(&self->player), BLUE);
  DrawRectangleRec(ent_coll(&self->player), RED);
}
