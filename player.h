#ifndef PLAYER_H
#define PLAYER_H

#include "common.h"

#define PLAYER_SPEED 200.0f
#define PLAYER_HEALTH 100
#define GUN_BARREL_OFFSET 32 // the distance between the center of the sprite and the tip of its gun
#define MELEE_KNOCKBACK_FORCE   800.0f   // initial push speed
#define BULLET_KNOCKBACK_FORCE   150.0f
#define KNOCKBACK_FRICTION 6.0f    // higher = stops faster

typedef struct BulletManager BulletManager;

typedef struct {
	Entity entity;
	Combatant combat;
	Texture2D texture;
} Player;

Player init_player(const char* texture_path);

void update_player(Player* player, BulletManager* bm, float dt);

void draw_player(const Player* player);

void unload_player(Player* player);

void melee_hit_player(Player* player, int damage, float angle);

#endif // PLAYER_H
