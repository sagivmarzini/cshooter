#include "bullet.h"
#include "common.h"
#include "enemy.h"
#include "map.h"
#include "player.h"
#include "camera.h"

int main(void) {
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "C Shooter");
	SetTargetFPS(GAME_FPS);

	Map map = map_init("../assets/map/road.png", "../assets/map/road_left.png", "../assets/map/grass.png", "../assets/map/roof.png");
	Player player = init_player("../assets/cop.png");
	EnemyManager enemies = init_enemy_manager("../assets/thug.png");
	BulletManager bullets = init_bullet_manager("../assets/bullet.png");
	camera_init();

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
		if (IsKeyPressed(KEY_KP_ADD)) camera_get()->zoom = 1.f;
		if (IsKeyPressed(KEY_KP_SUBTRACT)) camera_get()->zoom = 0.1f;

		camera_get()->target = player.entity.pos;

		// Draw
		BeginDrawing();
		ClearBackground((Color){10, 10, 10, 255});
		BeginMode2D(*camera_get());

		map_draw(&map);
		draw_player(&player);
		draw_enemies(&enemies);
		draw_bullets(&bullets);

		EndMode2D();
		EndDrawing();
	}

	// Cleanup
	unload_player(&player);
	CloseWindow();

	return 0;
}
