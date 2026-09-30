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

    while (!WindowShouldClose() && player.entity.health > 0) {
        // Update
        float dt = GetFrameTime();
        update_player(&player, &bm, dt);
        update_enemy(&enemy, &player, dt);
        update_bullets(&bm, dt);

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
