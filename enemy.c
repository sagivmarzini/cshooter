#include "enemy.h"

#include <raymath.h>

Enemy init_enemy(const char* texture_path) {
	Enemy enemy = {0};
	enemy.texture = LoadTexture(texture_path);
	enemy.entity.pos = (Vector2){SCREEN_WIDTH, SCREEN_HEIGHT};
	enemy.entity.active = true;
	return enemy;
}

void update_enemy(Enemy* enemy, Player* player, float dt) {
	enemy->hit_cooldown -= dt;

	// Face the player
	enemy->entity.rotation = atan2f(player->entity.pos.y - enemy->entity.pos.y,
	                                player->entity.pos.x - enemy->entity.pos.x) * RAD2DEG;

	float angleRad = enemy->entity.rotation * DEG2RAD;
	Vector2 direction = {cosf(angleRad), sinf(angleRad)};

	float player_distance = Vector2Length(Vector2Subtract(enemy->entity.pos, player->entity.pos));
	if (player_distance > ENEMY_MELEE_ATTACK_DISTANCE) {
		enemy->entity.vel = Vector2Scale(direction, ENEMY_SPEED);
		enemy->entity.pos = Vector2Add(enemy->entity.pos, Vector2Scale(enemy->entity.vel, dt));
	} else if (enemy->hit_cooldown <= 0) {
		melee_hit_player(player, ENEMY_MELEE_ATTACK_DAMAGE, enemy->entity.rotation);

		enemy->hit_cooldown = ENEMY_HIT_COOLDOWN;
	}
}

void draw_enemy(const Enemy* enemy) {
	Rectangle sourceRec = {0.0f, 0.0f, (float) enemy->texture.width, (float) enemy->texture.height};
	Rectangle destRec = {
		enemy->entity.pos.x, enemy->entity.pos.y, (float) enemy->texture.width, (float) enemy->texture.height
	};
	Vector2 origin = {(float) enemy->texture.width / 2.0f - 8, (float) enemy->texture.height / 2.0f};

	DrawTexturePro(enemy->texture, sourceRec, destRec, origin, enemy->entity.rotation, WHITE);
}

void unload_enemy(Enemy* enemy) {
	UnloadTexture(enemy->texture);
}
