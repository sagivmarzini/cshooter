#ifndef COMMON_H
#define COMMON_H

#include "map.h"
#include "raylib.h"

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 600
#define GAME_FPS 60
#define MAX_BULLETS 100
#define BULLET_DAMAGE 19

typedef struct {
	Vector2 position;
	Vector2 vel;
	float rotation;
	bool active;
} Entity;

// Anything that can be hit, knocked back, take damage
typedef struct {
	int health;
	Vector2 knockback_vel;
	float attack_cooldown;
} Combatant;

Vector2 calculate_knockback_velocity(Vector2 knockback_vel, float angle, float force);

void apply_knockback(Entity* entity, Combatant* combatant, const Map* map, float dt);

TileType tile_at_world_position(const Map* map, Vector2 position);

bool check_map_collision(const Map* map, Vector2 position);


#endif // COMMON_H
