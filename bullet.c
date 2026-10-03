//
// Created by sagivmarzini on 29/09/2026.
//

#include "bullet.h"

#include <raymath.h>

#include "camera.h"
#include "game.h"
#include "my_math.h"


void bullet_manager_init(BulletManager* bullet_manager, const char* texture_path) {
	bullet_manager->bullet_texture = LoadTexture(texture_path);
}

void bullet_spawn(BulletManager* bm, Vector2 position, float angle_deg) {
	// Find the first inactive bullet slot in the pool
	for (int i = 0; i < MAX_BULLETS; i++) {
		if (!bm->bullets[i].entity.active) {
			Bullet* b = &bm->bullets[i];

			b->entity.pos = position;
			b->entity.rotation = angle_deg;
			b->entity.active = true;
			b->life_timer = BULLET_LIFETIME_SECONDS;

			// Convert rotation angle (degrees) to direction vector (cos/sin need radians)
			float angleRad = angle_deg * DEG2RAD;
			Vector2 direction = {cosf(angleRad), sinf(angleRad)};

			b->entity.vel = Vector2Scale(direction, BULLET_SPEED);
			break; // Stop after spawning one
		}
	}
}

void bullets_update(GameContext* game, float dt) {
	for (int i = 0; i < MAX_BULLETS; i++) {
		Bullet* bullet = &game->bullet_manager.bullets[i];
		if (!bullet->entity.active) continue;

		bullet->entity.pos = Vector2Add(bullet->entity.pos, Vector2Scale(bullet->entity.vel, dt));

		bullet->life_timer -= dt;

		// Deactivate if lifetime expires or goes off-screen
		Vector2 bullet_screen_pos = GetWorldToScreen2D(bullet->entity.pos, *camera_get());
		if (bullet->life_timer <= 0.0f ||
		    bullet_screen_pos.x < 0 || bullet_screen_pos.x > SCREEN_WIDTH ||
		    bullet_screen_pos.y < 0 || bullet_screen_pos.y > SCREEN_HEIGHT ||
		    check_map_collision(&game->map, bullet->entity.pos)) {
			bullet->entity.active = false;
		}

		for (int i = 0; i < MAX_ENEMIES; i++) {
			Enemy* enemy = &game->enemy_manager.enemies[i];
			if (CheckCollisionPointRotatedRect(bullet->entity.pos, enemy->entity.pos, 32, 64, enemy->entity.rotation)
			    && enemy->combat.health > 0) {
				enemy_hit(enemy, BULLET_DAMAGE, bullet->entity.rotation);

				bullet->entity.active = false;
			}
		}
	}
}

void bullets_draw(const BulletManager* bm) {
	for (int i = 0; i < MAX_BULLETS; i++) {
		if (bm->bullets[i].entity.active) {
			Rectangle sourceRec = {0.0f, 0.0f, (float) bm->bullet_texture.width, (float) bm->bullet_texture.height};
			Rectangle destRec = {
				bm->bullets[i].entity.pos.x, bm->bullets[i].entity.pos.y, (float) bm->bullet_texture.width * 2,
				(float) bm->bullet_texture.height * 2
			};
			Vector2 origin = {(float) bm->bullet_texture.width / 2.0f, (float) bm->bullet_texture.height / 2.0f};

			DrawTexturePro(bm->bullet_texture, sourceRec, destRec, origin, bm->bullets[i].entity.rotation, WHITE);
		}
	}
}

void bullet_manager_unload(BulletManager* bullet_manager) {
	UnloadTexture(bullet_manager->bullet_texture);
}
