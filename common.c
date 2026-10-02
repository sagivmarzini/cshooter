#include "common.h"

#include <raymath.h>

#include "player.h"

Vector2 calculate_knockback_velocity(Vector2 knockback_vel, float angle, float force) {
	float angleRad = angle * DEG2RAD;
	Vector2 direction = {cosf(angleRad), sinf(angleRad)};

	return Vector2Add(knockback_vel, Vector2Scale(direction, force));
}

void apply_knockback(Entity* entity, Combatant* combatant, float dt) {
	entity->pos = Vector2Add(
		entity->pos,
		Vector2Scale(combatant->knockback_vel, dt)
	);

	float decay = expf(-KNOCKBACK_FRICTION * dt);
	combatant->knockback_vel = Vector2Scale(combatant->knockback_vel, decay);

	if (Vector2LengthSqr(combatant->knockback_vel) < 100.0f) {
		combatant->knockback_vel = (Vector2){0};
	}
}

bool check_map_collision(const Map* map, Vector2 position) {
	return tile_at(map, position.x / TILE_SIZE, position.y / TILE_SIZE) == TILE_BUILDING;
}
