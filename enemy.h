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

typedef struct GameContext GameContext;

typedef struct {
	Entity entity;
	Combatant combat;
} Enemy;

typedef struct {
	Enemy enemies[MAX_ENEMIES];
	Texture2D texture;
} EnemyManager;

void enemy_manager_init(EnemyManager* enemy_manager, const char* texture_path);

void enemy_spawn(EnemyManager* em, const Map* map);

void enemies_update(GameContext* game, float dt);

void enemies_draw(const EnemyManager* em);

void enemy_hit(Enemy* enemy, int damage, float angle);

void enemy_manager_unload(EnemyManager* em);

#endif //CSHOOTER_ENEMY_H
