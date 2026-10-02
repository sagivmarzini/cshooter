#include "enemy.h"

#include <raymath.h>

#include "camera.h"


EnemyManager init_enemy_manager(const char* texture_path) {
	EnemyManager em = {0};

	em.texture = LoadTexture(texture_path);

	return em;
}

void spawn_enemy(EnemyManager* em) {
	for (int i = 0; i < MAX_ENEMIES; i++) {
		if (!em->enemies[i].entity.active) {
			Enemy* enemy = &em->enemies[i];
			*enemy = (Enemy){0};

			enemy->entity.pos = GetScreenToWorld2D((Vector2){
				                                       GetRandomValue(0, 1) ? -100 : SCREEN_WIDTH + 100,
				                                       GetRandomValue(0, SCREEN_HEIGHT)
			                                       }, *camera_get());
			enemy->combat.health = PLAYER_HEALTH;
			enemy->entity.active = true;

			break;
		}
	}
}

void update_enemies(EnemyManager* em, Player* player, float dt) {
	for (int i = 0; i < MAX_ENEMIES; i++) {
		Enemy* enemy = &em->enemies[i];
		if (!enemy->entity.active) continue;

		enemy->combat.attack_cooldown -= dt;

		// Face the player
		enemy->entity.rotation = atan2f(player->entity.pos.y - enemy->entity.pos.y,
		                                player->entity.pos.x - enemy->entity.pos.x) * RAD2DEG;

		float angleRad = enemy->entity.rotation * DEG2RAD;
		Vector2 direction = {cosf(angleRad), sinf(angleRad)};

		float player_distance = Vector2Length(Vector2Subtract(enemy->entity.pos, player->entity.pos));
		if (player_distance > ENEMY_MELEE_ATTACK_DISTANCE) {
			enemy->entity.vel = Vector2Scale(direction, ENEMY_SPEED);
			enemy->entity.pos = Vector2Add(enemy->entity.pos, Vector2Scale(enemy->entity.vel, dt));
		} else if (enemy->combat.attack_cooldown <= 0) {
			melee_hit_player(player, ENEMY_MELEE_ATTACK_DAMAGE, enemy->entity.rotation);

			enemy->combat.attack_cooldown = ENEMY_HIT_COOLDOWN;
		}

		apply_knockback(&enemy->entity, &enemy->combat, dt);
	}
}

void draw_enemies(const EnemyManager* em) {
	for (int i = 0; i < MAX_ENEMIES; i++) {
		const Enemy* enemy = &em->enemies[i];
		if (!enemy->entity.active) continue;

		Rectangle sourceRec = {0.0f, 0.0f, (float) em->texture.width, (float) em->texture.height};
		Rectangle destRec = {
			enemy->entity.pos.x, enemy->entity.pos.y, (float) em->texture.width, (float) em->texture.height
		};
		Vector2 origin = {(float) em->texture.width / 2.0f - 8, (float) em->texture.height / 2.0f};

		DrawTexturePro(em->texture, sourceRec, destRec, origin, enemy->entity.rotation,
		               Vector2Equals(enemy->combat.knockback_vel, (Vector2){0, 0}) ? WHITE : MAROON);

		DrawRectangle((int) enemy->entity.pos.x - 48, (int) enemy->entity.pos.y - 64, 96, 10, GRAY);
		DrawRectangle((int) enemy->entity.pos.x - 48, (int) enemy->entity.pos.y - 64, 96 * enemy->combat.health / 100,
		              10, RED);
	}
}

void unload_enemies(EnemyManager* em) {
	UnloadTexture(em->texture);
}

void hit_enemy(Enemy* enemy, int damage, float angle) {
	enemy->combat.health -= damage;
	if (enemy->combat.health <= 0) enemy->entity.active = false;

	enemy->combat.knockback_vel = calculate_knockback_velocity(enemy->combat.knockback_vel, angle,
	                                                           BULLET_KNOCKBACK_FORCE);
}
