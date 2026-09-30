#ifndef COMMON_H
#define COMMON_H

#include "raylib.h"

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 600
#define GAME_FPS 60
#define MAX_BULLETS 100
#define BULLET_DAMAGE 19

typedef struct {
	Vector2 pos;
	Vector2 vel;
	float rotation;
	bool active;
} Entity;

// Anything that can be hit, knocked back, take damage
typedef struct {
	int health;
	Vector2 knockback_vel;
	float attack_cooldown; // invuln/hitstun frames after taking a hit
} Combatant;

Vector2 calculate_knockback_velocity(Vector2 knockback_vel, float angle, float force);

void apply_knockback(Entity* entity, Combatant* combatant, float dt);


#endif // COMMON_H
