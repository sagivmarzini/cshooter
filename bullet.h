//
// Created by sagivmarzini on 29/09/2026.
//

#ifndef CSHOOTER_BULLET_H
#define CSHOOTER_BULLET_H

#include "common.h"
#include "enemy.h"

#define BULLET_SPEED 900.0f
#define BULLET_LIFETIME_SECONDS 2.0f

typedef struct {
	Entity entity;
	float life_timer;
} Bullet;

typedef struct BulletManager {
	Bullet bullets[MAX_BULLETS];
	Texture2D bullet_texture;
} BulletManager;

BulletManager init_bullet_manager(const char* texture_path);

void spawn_bullet(BulletManager* bm, Vector2 position, float angle_deg);

void update_bullets(BulletManager* bm, Enemy enemies[], float dt);

void draw_bullets(const BulletManager* bm);

#endif //CSHOOTER_BULLET_H
