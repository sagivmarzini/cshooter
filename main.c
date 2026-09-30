#include "bullet.h"
#include "common.h"
#include "enemy.h"
#include "player.h"

int main(void) {
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "C Shooter");
	SetTargetFPS(GAME_FPS);

	Player player = init_player("../assets/cop.png");
	Enemy enemy = init_enemy("../assets/thug.png");
	BulletManager bm = init_bullet_manager("../assets/bullet.png");

	bool paused = false;
	while (!WindowShouldClose() && player.combat.health > 0) {
		// Update
		float dt = GetFrameTime();
		if (!paused) {
			update_player(&player, &bm, dt);
			update_enemy(&enemy, &player, dt);
			update_bullets(&bm, &enemy, dt);
		}
		if (IsKeyPressed(KEY_ONE)) enemy.combat.health = 100;
		if (IsKeyPressed(KEY_TWO)) enemy.combat.health = 200;
		if (IsKeyPressed(KEY_SPACE)) paused = !paused;

		// Draw
		BeginDrawing();
		ClearBackground((Color){10, 10, 10, 255});

		draw_player(&player);
		draw_enemy(&enemy);
		draw_bullets(&bm);

		EndDrawing();
	}

	// Cleanup
	unload_player(&player);
	CloseWindow();

	return 0;
}
