#include "bullet.h"
#include "common.h"
#include "enemy.h"
#include "map.h"
#include "player.h"
#include "camera.h"
#include "game.h"

int main(void) {
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "C Shooter");
	SetTargetFPS(GAME_FPS);

	GameContext game = {0};
	game_context_init(&game);

	bool paused = false;
	while (!WindowShouldClose() && game.player.combat.health > 0) {
		// Update
		float dt = GetFrameTime();
		if (!paused) {
			player_update(&game, dt);
			enemies_update(&game, dt);
			bullets_update(&game, dt);
		}
		if (IsKeyPressed(KEY_SPACE)) paused = !paused;
		if (IsKeyPressed(KEY_E)) enemy_spawn(&game.enemy_manager, &game.world.city);
		if (IsKeyPressed(KEY_KP_ADD)) camera_get()->zoom = 1.5f;
		if (IsKeyPressed(KEY_KP_SUBTRACT)) camera_get()->zoom = 0.1f;

		camera_get()->target = game.player.entity.position;

		// Draw
		BeginDrawing();
		ClearBackground((Color){10, 10, 10, 255});
		BeginMode2D(*camera_get());

		world_draw(&game.world, &game.tile_textures);
		player_draw(&game.player);
		enemies_draw(&game.enemy_manager);
		bullets_draw(&game.bullet_manager);

		EndMode2D();
		EndDrawing();
	}

	// Cleanup
	game_context_unload(&game);
	CloseWindow();

	return 0;
}
