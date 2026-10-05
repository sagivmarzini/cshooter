#include "enemy.h"

#include <raymath.h>
#include "camera.h"
#include "game.h"


void enemy_manager_init(EnemyManager* enemy_manager, const char* texture_path) {
	enemy_manager->texture = LoadTexture(texture_path);
}

void enemy_spawn(EnemyManager* em, const Map* map) {
	for (int i = 0; i < MAX_ENEMIES; i++) {
		if (!em->enemies[i].entity.active) {
			Enemy* enemy = &em->enemies[i];
			*enemy = (Enemy){0};

			int spawn_direction = GetRandomValue(0, 1) ? -1 : 1;
			enemy->entity.position = GetScreenToWorld2D((Vector2){
				                                            spawn_direction == -1 ? -100 : SCREEN_WIDTH + 100,
				                                            GetRandomValue(0, SCREEN_HEIGHT)
			                                            }, *camera_get());
			while (check_map_collision(map, enemy->entity.position))
				enemy->entity.position.x += spawn_direction * TILE_SIZE / 2.f;

			enemy->combat.health = PLAYER_HEALTH;
			enemy->entity.active = true;

			break;
		}
	}
}

void enemies_update(GameContext* game, float dt) {
	for (int i = 0; i < MAX_ENEMIES; i++) {
		Enemy* enemy = &game->enemy_manager.enemies[i];
		if (!enemy->entity.active) continue;

		enemy->combat.attack_cooldown -= dt;

		// Face the player
		enemy->entity.rotation = atan2f(game->player.entity.position.y - enemy->entity.position.y,
		                                game->player.entity.position.x - enemy->entity.position.x) * RAD2DEG;

		float angleRad = enemy->entity.rotation * DEG2RAD;
		Vector2 direction = {cosf(angleRad), sinf(angleRad)};

		float player_distance = Vector2Length(Vector2Subtract(enemy->entity.position, game->player.entity.position));
		if (player_distance > ENEMY_MELEE_ATTACK_DISTANCE) {
			enemy->entity.vel = Vector2Scale(direction, ENEMY_SPEED);

			const Vector2 old_pos = enemy->entity.position;
			enemy->entity.position = Vector2Add(enemy->entity.position, Vector2Scale(enemy->entity.vel, dt));
			if (check_map_collision(get_active_map(&game->world), enemy->entity.position))
				enemy->entity.position = old_pos;
		} else if (enemy->combat.attack_cooldown <= 0) {
			player_melee_hit(&game->player, ENEMY_MELEE_ATTACK_DAMAGE, enemy->entity.rotation);

			enemy->combat.attack_cooldown = ENEMY_HIT_COOLDOWN;
		}

		apply_knockback(&enemy->entity, &enemy->combat, get_active_map(&game->world), dt);
	}
}

void enemies_draw(const EnemyManager* em) {
	for (int i = 0; i < MAX_ENEMIES; i++) {
		const Enemy* enemy = &em->enemies[i];
		if (!enemy->entity.active) continue;

		Rectangle sourceRec = {0.0f, 0.0f, (float) em->texture.width, (float) em->texture.height};
		Rectangle destRec = {
			enemy->entity.position.x, enemy->entity.position.y, (float) em->texture.width, (float) em->texture.height
		};
		Vector2 origin = {(float) em->texture.width / 2.0f - 8, (float) em->texture.height / 2.0f};

		DrawTexturePro(em->texture, sourceRec, destRec, origin, enemy->entity.rotation,
		               Vector2Equals(enemy->combat.knockback_vel, (Vector2){0, 0}) ? WHITE : MAROON);

		DrawRectangle((int) enemy->entity.position.x - 48, (int) enemy->entity.position.y - 64, 96, 10, GRAY);
		DrawRectangle((int) enemy->entity.position.x - 48, (int) enemy->entity.position.y - 64,
		              96 * enemy->combat.health / 100,
		              10, RED);
	}
}

void enemy_manager_unload(EnemyManager* em) {
	UnloadTexture(em->texture);
}

void enemy_hit(Enemy* enemy, int damage, float angle) {
	enemy->combat.health -= damage;
	if (enemy->combat.health <= 0) enemy->entity.active = false;

	enemy->combat.knockback_vel = calculate_knockback_velocity(enemy->combat.knockback_vel, angle,
	                                                           BULLET_KNOCKBACK_FORCE);
}
