//
// Created by sagivmarzini on 29/09/2026.
//

#include "bullet.h"

#include <raymath.h>


BulletManager init_bullet_manager(const char* texture_path) {
	BulletManager bm = {0};

	bm.bullet_texture = LoadTexture(texture_path);

	for (int i = 0; i < MAX_BULLETS; i++) {
		bm.bullets[i].entity.active = false;
	}

	return bm;
}

void spawn_bullet(BulletManager* bm, Vector2 position, float angle_deg) {
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

void update_bullets(BulletManager* bm, float dt) {
	for (int i = 0; i < MAX_BULLETS; i++) {
		if (!bm->bullets[i].entity.active) continue;

		Bullet* b = &bm->bullets[i];

		// 1. Update position
		b->entity.pos = Vector2Add(b->entity.pos, Vector2Scale(b->entity.vel, dt));

		// 2. Countdown lifetime
		b->life_timer -= dt;

		// 3. Deactivate if lifetime expires or goes off-screen
		if (b->life_timer <= 0.0f ||
		    b->entity.pos.x < 0 || b->entity.pos.x > SCREEN_WIDTH ||
		    b->entity.pos.y < 0 || b->entity.pos.y > SCREEN_HEIGHT) {
			b->entity.active = false;
		}
	}
}

void draw_bullets(const BulletManager* bm) {
	for (int i = 0; i < MAX_BULLETS; i++) {
		if (bm->bullets[i].entity.active) {
			Rectangle sourceRec = {0.0f, 0.0f, (float) bm->bullet_texture.width, (float) bm->bullet_texture.height};
			Rectangle destRec = {
				bm->bullets[i].entity.pos.x, bm->bullets[i].entity.pos.y, (float) bm->bullet_texture.width,
				(float) bm->bullet_texture.height
			};
			Vector2 origin = {(float) bm->bullet_texture.width / 2.0f, (float) bm->bullet_texture.height / 2.0f};

			DrawTexturePro(bm->bullet_texture, sourceRec, destRec, origin, bm->bullets[i].entity.rotation, WHITE);
		}
	}
}
