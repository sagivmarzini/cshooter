#include "common.h"

#include <raymath.h>

#include "player.h"

Vector2 calculate_knockback_velocity(Vector2 knockback_vel, float angle, float force) {
	float angleRad = angle * DEG2RAD;
	Vector2 direction = {cosf(angleRad), sinf(angleRad)};

	return Vector2Add(knockback_vel, Vector2Scale(direction, force));
}

void apply_knockback(Entity* entity, Combatant* combatant, const Map* map, float dt) {
	Vector2 old_pos = entity->position;
	entity->position = Vector2Add(
		entity->position,
		Vector2Scale(combatant->knockback_vel, dt)
	);
	if (check_map_collision(map, entity->position))
		entity->position = old_pos;

	float decay = expf(-KNOCKBACK_FRICTION * dt);
	combatant->knockback_vel = Vector2Scale(combatant->knockback_vel, decay);

	if (Vector2LengthSqr(combatant->knockback_vel) < 100.0f) {
		combatant->knockback_vel = (Vector2){0};
	}
}


TileType tile_at_world_position(const Map* map, Vector2 position) {
	return tile_at(map, position.x / TILE_SIZE, position.y / TILE_SIZE);
}

uint8_t building_id_at_world_position(uint8_t building_id[64][64], Vector2 position) {
	int tile_x = (int) (position.x / TILE_SIZE);
	int tile_y = (int) (position.y / TILE_SIZE);

	return building_id[tile_x][tile_y];
}

bool check_map_collision(const Map* map, Vector2 position) {
	const TileType tile = tile_at_world_position(map, position);
	return tile == TILE_BUILDING || tile == TILE_NONE;
}
