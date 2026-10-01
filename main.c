#include "bullet.h"
#include "common.h"
#include "enemy.h"
#include "player.h"

int main(void) {
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "C Shooter");
	SetTargetFPS(GAME_FPS);

	Player player = init_player("../assets/cop.png");
	EnemyManager enemies = init_enemy_manager("../assets/thug.png");
	BulletManager bullets = init_bullet_manager("../assets/bullet.png");

	bool paused = false;
	while (!WindowShouldClose() && player.combat.health > 0) {
		// Update
		float dt = GetFrameTime();
		if (!paused) {
			update_player(&player, &bullets, dt);
			update_enemies(&enemies, &player, dt);
			update_bullets(&bullets, enemies.enemies, dt);
		}
		if (IsKeyPressed(KEY_SPACE)) paused = !paused;
		if (IsKeyPressed(KEY_E)) spawn_enemy(&enemies);

		// Draw
		BeginDrawing();
		ClearBackground((Color){10, 10, 10, 255});

		draw_player(&player);
		draw_enemies(&enemies);
		draw_bullets(&bullets);

		EndDrawing();
	}

	// Cleanup
	unload_player(&player);
	CloseWindow();

	return 0;
}
