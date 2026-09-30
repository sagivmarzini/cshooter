#include "player.h"

#include "raymath.h"

Player init_player(const char* texture_path) {
	Player player = {0};
	player.texture = LoadTexture(texture_path);
	player.entity.pos = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
	player.entity.active = true;
	player.entity.health = PLAYER_HEALTH;
	return player;
}

void update_player(Player* player, BulletManager* bm, float dt) {
	Vector2 direction = {0.0f, 0.0f};

	if (IsKeyDown(KEY_D)) direction.x += 1.0f;
	if (IsKeyDown(KEY_A)) direction.x -= 1.0f;
	if (IsKeyDown(KEY_S)) direction.y += 1.0f;
	if (IsKeyDown(KEY_W)) direction.y -= 1.0f;

	Vector2 dir = Vector2Normalize(direction);
	player->entity.vel = Vector2Scale(dir, PLAYER_SPEED);
	player->entity.pos = Vector2Add(player->entity.pos, Vector2Scale(player->entity.vel, dt));

	// knockback
	player->entity.pos = Vector2Add(
		player->entity.pos,
		Vector2Scale(player->knockback_vel, dt)
	);

	float decay = expf(-KNOCKBACK_FRICTION * dt);
	player->knockback_vel = Vector2Scale(player->knockback_vel, decay);

	if (Vector2LengthSqr(player->knockback_vel) < 100.0f) {
		player->knockback_vel = (Vector2){0};
	}

	Vector2 mousePos = GetMousePosition();
	player->entity.rotation = atan2f(mousePos.y - player->entity.pos.y, mousePos.x - player->entity.pos.x) * RAD2DEG;

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		Vector2 gun_offset = {
			GUN_BARREL_OFFSET * cosf(player->entity.rotation * DEG2RAD),
			GUN_BARREL_OFFSET * sinf(player->entity.rotation * DEG2RAD)
		};

		spawn_bullet(bm, Vector2Add(player->entity.pos, gun_offset), player->entity.rotation);
	}
}

void draw_player(const Player* player) {
	Rectangle sourceRec = {0.0f, 0.0f, (float) player->texture.width, (float) player->texture.height};
	Rectangle destRec = {
		player->entity.pos.x, player->entity.pos.y, (float) player->texture.width, (float) player->texture.height
	};
	Vector2 origin = {(float) player->texture.width / 2.0f - 8, (float) player->texture.height / 2.0f};

	DrawTexturePro(player->texture, sourceRec, destRec, origin, player->entity.rotation,
	               Vector2Equals(player->knockback_vel, (Vector2){0, 0}) ? WHITE : MAROON);
	DrawRectangle((int) player->entity.pos.x - 48, (int) player->entity.pos.y - 64, 96, 10, GRAY);
	DrawRectangle((int) player->entity.pos.x - 48, (int) player->entity.pos.y - 64, 96 * player->entity.health / 100,
	              10, GREEN);
}

void unload_player(Player* player) {
	UnloadTexture(player->texture);
}

void melee_hit_player(Player* player, int damage, float angle) {
	player->entity.health -= damage;

	float angleRad = angle * DEG2RAD;
	Vector2 direction = {cosf(angleRad), sinf(angleRad)};

	player->knockback_vel = Vector2Add(
		player->knockback_vel,
		Vector2Scale(direction, KNOCKBACK_FORCE)
	);
}
