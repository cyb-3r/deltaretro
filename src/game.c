#include "game.h"
#include "player.h"
#include "raylib.h"

#define HUD_HEIGHT 32
#define ENT_SPD 60.00f

struct dirs_input {
  bool down;
  bool right;
  bool up;
  bool left;
};

/*== Utils ==*/
void plr_process_coll(ent_t*, scr_t*);
void plr_process_dirs(ent_t*, struct dirs_input);
void apply_delta(ent_t*, f32);

void game_init(game_t *self, u32 width, u32 height) {
  const u32 actual_height = height - HUD_HEIGHT;
  self->width = width;
  self->height = actual_height;
  self->player.life_max = 20;
  self->player.life = 20;
  self->player.x = ((f32)width / 2.0f) - 8.0f;
  self->player.y = ((f32)actual_height / 2.0f) - 8.0f;
}

bool will_collide(f32 x, f32 y, scr_t *data) {
  if (!data) return false;
  return scr_collision(data, (x / TILE_SIZE), (y / TILE_SIZE));
}

void plr_mvmt(ent_t *plr, ipt_t *ipt) {
  struct dirs_input dirs = {
    input_is_down(ipt, INPUT_DOWN),
    input_is_down(ipt, INPUT_RIGHT),
    input_is_down(ipt, INPUT_UP),
    input_is_down(ipt, INPUT_LEFT),
  };

  if ((dirs.down | dirs.right | dirs.up | dirs.left) == 0) {
    plr->state = PLR_IDLE;
  } else plr->state = PLR_MOVING;

  plr->xsp = (plr->state == PLR_MOVING) ?
    ((dirs.right - dirs.left) * ENT_SPD) : 0;
  plr->ysp = (plr->state == PLR_MOVING) ?
    ((dirs.down - dirs.up) * ENT_SPD) : 0;

  plr_process_dirs(plr, dirs);
}

void plr_update()__attribute__((unused));

void game_update(game_t *self, ipt_t *ipt, f32 delta) {
  plr_mvmt(&self->player, ipt);
  apply_delta(&self->player, delta);
  plr_process_coll(&self->player, &self->world.cur_scr);
  ent_apply_spd(&self->player);
}

void game_draw(game_t *self) {
  DrawRectangleRec(ent_rect(&self->player), BLUE);
  DrawRectangleRec(ent_coll(&self->player), RED);
}

void plr_process_coll(ent_t *self, scr_t *data) {
  /*== Collisions WIP ==*/
  if (scr_collision(data,
    (self->x + self->xsp) / TILE_SIZE,
    self->y / TILE_SIZE)
  ) self->xsp = 0;

  if (scr_collision(data,
    self->x / TILE_SIZE,
    (self->y + self->ysp) / TILE_SIZE)
  ) self->ysp = 0;
}

void plr_process_dirs(ent_t *self, struct dirs_input dirs) {
  if (!self) return;
  if (dirs.right)  self->facing = ENT_DIR_R;
  if (dirs.left)   self->facing = ENT_DIR_L;
  if (dirs.down)   self->facing = ENT_DIR_D;
  if (dirs.up)     self->facing = ENT_DIR_U;
}

void apply_delta(ent_t *self, f32 delta) {
  if (!self) return;
  self->xsp *= delta;
  self->ysp *= delta;
}

void game_incpts(game_t *self, i16 amount) {
  if (!self) return;
  self->plr_data.pts += amount;
}

void game_setpts(game_t *self, i16 amount) {
  if (!self) return;
  self->plr_data.pts = amount;
}
