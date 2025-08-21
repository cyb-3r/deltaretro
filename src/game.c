#include "game.h"
#include "player.h"
#include "raylib.h"
#include "scene.h"

#define W_WIDTH 256
#define W_HEIGHT 224
#define HUD_HEIGHT 32
#define ENT_SPD 30.0f

void game_init(game_t *self) {
  self->player.life_max = 20;
  self->player.life = 20;
  self->player.x = ((f32)W_WIDTH / 2.0f) - 8.0f;
  self->player.y = (((f32)W_HEIGHT - (f32)HUD_HEIGHT) / 2.0f) - 8.0f;
}

void update_delta(game_t *self) {
  self->delta = GetFrameTime();
}

bool will_collide(f32 x, f32 y, scr_t *data) {
  if (!data) return false;
  return scr_collision(data, (x / TILE_SIZE), (y / TILE_SIZE));
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

  TraceLog(LOG_DEBUG, "Player x speed = %f", plr->xsp);
  TraceLog(LOG_DEBUG, "Player x pox   = %f", plr->x);

  if (scr_collision(&self->world.cur_scr,
    (plr->x + plr->xsp) / TILE_SIZE,
    plr->y / TILE_SIZE)
  ) plr->xsp = 0;

  if (scr_collision(&self->world.cur_scr,
    plr->x / TILE_SIZE,
    (plr->y + plr->ysp) / TILE_SIZE)
  ) plr->ysp = 0;

  if (right)  plr->facing = ENT_DIR_R;
  if (left)   plr->facing = ENT_DIR_L;
  if (down)   plr->facing = ENT_DIR_D;
  if (up)     plr->facing = ENT_DIR_U;
}

void plr_update()__attribute__((unused));

void game_update(game_t *self, ipt_t *ipt) {
  update_delta(self);
  // TraceLog(LOG_DEBUG, "Delta Time: %f", self->delta);

  plr_mvmt(self, ipt);
  ent_apply_spd(&self->player, self->delta);
}

void game_draw(game_t *self) {
  DrawRectangleRec(ent_rect(&self->player), BLUE);
  DrawRectangleRec(ent_coll(&self->player), RED);
}
