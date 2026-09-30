//
// Created by sagivmarzini on 29/09/2026.
//

#ifndef CSHOOTER_ENEMY_H
#define CSHOOTER_ENEMY_H
#include "common.h"
#include "player.h"

#define ENEMY_SPEED 100.0f
#define ENEMY_MELEE_ATTACK_DISTANCE 64.0f
#define ENEMY_MELEE_ATTACK_DAMAGE 24.f
#define ENEMY_HIT_COOLDOWN 1 // in seconds

typedef struct {
	Entity entity;
	Combatant combat;
	Texture2D texture;
} Enemy;

Enemy init_enemy(const char* texture_path);

void update_enemy(Enemy* enemy, Player* player, float dt);

void draw_enemy(const Enemy* enemy);

void unload_enemy(Enemy* enemy);

void hit_enemy(Enemy* enemy, int damage, float angle);

#endif //CSHOOTER_ENEMY_H
