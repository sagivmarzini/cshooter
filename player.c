#include "player.h"

#include <string.h>

#include "bullet.h"
#include "camera.h"
#include "game.h"
#include "map.h"
#include "raymath.h"

void player_init(Player* player, const char* texture_path, const Map* map) {
	player->texture = LoadTexture(texture_path);
	player->entity.position = (Vector2){MAP_WIDTH * TILE_SIZE / 2.0f, MAP_HEIGHT * TILE_SIZE / 2.0f};
	player->entity.active = true;
	player->combat.health = PLAYER_HEALTH;

	while (check_map_collision(map, player->entity.position))
		player->entity.position.x -= TILE_SIZE / 2.f;
}

void player_update(GameContext* game, float dt) {
	Vector2 direction = {0.0f, 0.0f};

	if (IsKeyDown(KEY_D)) direction.x += 1.0f;
	if (IsKeyDown(KEY_A)) direction.x -= 1.0f;
	if (IsKeyDown(KEY_S)) direction.y += 1.0f;
	if (IsKeyDown(KEY_W)) direction.y -= 1.0f;

	Vector2 dir = Vector2Normalize(direction);
	game->player.entity.vel = Vector2Scale(dir, PLAYER_SPEED);
	Vector2 old_pos = game->player.entity.position;
	game->player.entity.position = Vector2Add(game->player.entity.position, Vector2Scale(game->player.entity.vel, dt));
	if (check_map_collision(get_active_map(&game->world), game->player.entity.position))
		game->player.entity.position = old_pos;

	if (tile_at_world_position(&game->world.city, game->player.entity.position) == TILE_DOOR)
		world_enter_building(&game->world,
		                     building_id_at_world_position(game->world.city.building_id, game->player.entity.position));

	apply_knockback(&game->player.entity, &game->player.combat, get_active_map(&game->world), dt);

	Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), *camera_get());
	game->player.entity.rotation = atan2f(mousePos.y - game->player.entity.position.y,
	                                      mousePos.x - game->player.entity.position.x) * RAD2DEG;

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		Vector2 gun_offset = {
			GUN_BARREL_OFFSET * cosf(game->player.entity.rotation * DEG2RAD),
			GUN_BARREL_OFFSET * sinf(game->player.entity.rotation * DEG2RAD)
		};

		bullet_spawn(&game->bullet_manager, Vector2Add(game->player.entity.position, gun_offset),
		             game->player.entity.rotation);
	}
}

void player_draw(const Player* player) {
	Rectangle sourceRec = {0.0f, 0.0f, (float) player->texture.width, (float) player->texture.height};
	Rectangle destRec = {
		player->entity.position.x, player->entity.position.y, (float) player->texture.width,
		(float) player->texture.height
	};
	Vector2 origin = {(float) player->texture.width / 2.0f - 8, (float) player->texture.height / 2.0f};

	DrawTexturePro(player->texture, sourceRec, destRec, origin, player->entity.rotation,
	               Vector2Equals(player->combat.knockback_vel, (Vector2){0, 0}) ? WHITE : MAROON);
	DrawRectangle((int) player->entity.position.x - 48, (int) player->entity.position.y - 64, 96, 10, GRAY);
	DrawRectangle((int) player->entity.position.x - 48, (int) player->entity.position.y - 64,
	              96 * player->combat.health / 100,
	              10, GREEN);
}

void player_unload(Player* player) {
	UnloadTexture(player->texture);
}

void player_melee_hit(Player* player, int damage, float angle) {
	player->combat.health -= damage;

	player->combat.knockback_vel = calculate_knockback_velocity(player->combat.knockback_vel, angle,
	                                                            MELEE_KNOCKBACK_FORCE);
}
