#include "common.h"
#include "player.h"

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "C Shooter");
    SetTargetFPS(GAME_FPS);

    Player player = InitPlayer("../cop.png");

    while (!WindowShouldClose()) {
        // Update
        float dt = GetFrameTime();
        UpdatePlayer(&player, dt);

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawPlayer(&player);

        EndDrawing();
    }

    // Cleanup
    UnloadPlayer(&player);
    CloseWindow();

    return 0;
}
