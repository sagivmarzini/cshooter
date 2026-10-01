//
// Created by sagivmarzini on 29/09/2026.
//

#ifndef CSHOOTER_ENEMY_H
#define CSHOOTER_ENEMY_H
#include "common.h"
#include "player.h"

#define MAX_ENEMIES 48
#define ENEMY_SPEED 100.0f
#define ENEMY_MELEE_ATTACK_DISTANCE 64.0f
#define ENEMY_MELEE_ATTACK_DAMAGE 24.f
#define ENEMY_HIT_COOLDOWN 1 // in seconds

typedef struct {
	Entity entity;
	Combatant combat;
} Enemy;

typedef struct {
	Enemy enemies[MAX_ENEMIES];
	Texture2D texture;
} EnemyManager;

EnemyManager init_enemy_manager(const char* texture_path);

void spawn_enemy(EnemyManager* em);

void update_enemies(EnemyManager* em, Player* player, float dt);

void draw_enemies(const EnemyManager* em);

void unload_enemies(EnemyManager* em);

void hit_enemy(Enemy* enemy, int damage, float angle);

#endif //CSHOOTER_ENEMY_H
