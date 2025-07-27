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

  bool ok = atlas_load(
    &self->animations,
    "resources/gfx/heroes.png",
    "resources/gfx/heroes.anim"
  );
  if (!ok) TraceLog(LOG_ERROR, "Failed loading atlas");

  self->player.sprite.atlas = self->animations;
  ok = sprite_set_grp(&self->player.sprite, "HERO_SWORD");
  if (!ok) TraceLog(LOG_ERROR, "Failed setting sprite group for player");

  self->inputs = inputs_default();
  input_load(&(self->inputs));
}

void update_delta(game_t *self) {
  self->delta = GetFrameTime();
}

void plr_mvmt(game_t *self) {
  input_t *inp = &self->inputs;
  ent_t *plr = &self->player;

  u8
  left =    input_is_down(inp, INPUT_LEFT),
  right =   input_is_down(inp, INPUT_RIGHT),
  up =      input_is_down(inp, INPUT_UP),
  down =    input_is_down(inp, INPUT_DOWN);

  if (!left && !right && !up && !down) {
    plr->state = IDLE;
  } else plr->state = MOVING;

  plr->xsp = (plr->state == MOVING) ? ((right - left) * ENT_SPD) : 0;
  plr->ysp = (plr->state == MOVING) ? ((down - up) * ENT_SPD) : 0;

  if (right)  plr->facing = DIR_RIGHT;
  if (left)   plr->facing = DIR_LEFT;
  if (down)   plr->facing = DIR_DOWN;
  if (up)     plr->facing = DIR_UP;
}

void plr_update()__attribute__((unused));

void game_update(game_t *self) {
  update_delta(self);
  input_update(&self->inputs);

  plr_mvmt(self);
  ent_apply_spd(&self->player);

  ent_t *plr = &self->player;
  int pstate = plr->state;
  spr_t *plr_spr = &self->player.sprite;

  switch(plr->facing) {
    case DIR_DOWN:
      if (pstate == MOVING)
        sprite_swap_anim(plr_spr, "D.Walk");
      else sprite_swap_anim(plr_spr, "D.Idle");
    break;
    case DIR_UP:
      if (pstate == MOVING)
        sprite_swap_anim(plr_spr, "U.Walk");
      else sprite_swap_anim(plr_spr, "U.Idle");
    break;
    case DIR_RIGHT:
      if (pstate == MOVING)
        sprite_swap_anim(plr_spr, "R.Walk");
      else sprite_swap_anim(plr_spr, "R.Idle");
    break;
    case DIR_LEFT:
      if (pstate == MOVING)
        sprite_swap_anim(plr_spr, "L.Walk");
      else sprite_swap_anim(plr_spr, "L.Idle");
    break;
    default:
      sprite_swap_anim(plr_spr, "D.Idle");
    break;
  }

  sprite_update(plr_spr, self->delta);
}
