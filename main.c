#include "bullet.h"
#include "common.h"
#include "player.h"

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "C Shooter");
    SetTargetFPS(GAME_FPS);

    Player player = init_player("../assets/cop.png");
    BulletManager bm = init_bullet_manager("../assets/bullet.png");

    while (!WindowShouldClose()) {
        // Update
        float dt = GetFrameTime();
        update_player(&player, &bm, dt);
        update_bullets(&bm, dt);

        // Draw
        BeginDrawing();
        ClearBackground((Color){10, 10, 10, 255});

        draw_player(&player);
        draw_bullets(&bm);

        EndDrawing();
    }

    // Cleanup
    unload_player(&player);
    CloseWindow();

    return 0;
}
