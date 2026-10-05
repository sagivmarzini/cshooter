//
// Created by sagivmarzini on 29/09/2026.
//

#ifndef CSHOOTER_BULLET_H
#define CSHOOTER_BULLET_H

#include "common.h"

#define BULLET_SPEED 1000.0f
#define BULLET_LIFETIME_SECONDS 2.0f

typedef struct GameContext GameContext;

typedef struct {
	Entity entity;
	float life_timer;
} Bullet;

typedef struct BulletManager {
	Bullet bullets[MAX_BULLETS];
	Texture2D bullet_texture;
} BulletManager;

void bullet_manager_init(BulletManager* bullet_manager, const char* texture_path);

void bullet_spawn(BulletManager* bm, Vector2 position, float angle_deg);

void bullets_update(GameContext* game, float dt);

void bullets_draw(const BulletManager* bm);

void bullet_manager_unload(BulletManager* bullet_manager);


#endif //CSHOOTER_BULLET_H
