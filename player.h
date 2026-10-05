#ifndef PLAYER_H
#define PLAYER_H

#include "common.h"
#include "map.h"

#define PLAYER_IDLE_TEXTURE "../assets/cop_idle.png"
#define PLAYER_SHOOTING_TEXTURE "../assets/cop_shooting.png"

#define PLAYER_SPEED 200.0f
#define PLAYER_AIMING_SPEED 100.f
#define PLAYER_HEALTH 100
#define GUN_BARREL_OFFSET 32 // the distance between the center of the sprite and the tip of its gun
#define MELEE_KNOCKBACK_FORCE   800.0f   // initial push speed
#define BULLET_KNOCKBACK_FORCE   150.0f
#define KNOCKBACK_FRICTION 6.0f    // higher = stops faster

typedef struct BulletManager BulletManager;
typedef struct GameContext GameContext;

typedef struct {
	Entity entity;
	Combatant combat;
	Texture2D idle_texture;
	Texture2D shooting_texture;
	bool is_aiming;
} Player;

void player_init(Player* player, const Map* map);

void player_update(GameContext* game, float dt);

void player_draw(const Player* player);

void player_unload(Player* player);

void player_melee_hit(Player* player, int damage, float angle);

#endif // PLAYER_H
