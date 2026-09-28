#include "raylib.h"
#include "raymath.h"

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - input keys");

    Texture2D sprite = LoadTexture("../cop.png");

    Vector2 ballPosition = {(float) screenWidth / 2, (float) screenHeight / 2};

    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        Vector2 direction = {0.0f, 0.0f};

        if (IsKeyDown(KEY_D)) direction.x += 1.0f;
        if (IsKeyDown(KEY_A)) direction.x -= 1.0f;
        if (IsKeyDown(KEY_S)) direction.y += 1.0f;
        if (IsKeyDown(KEY_W)) direction.y -= 1.0f;

        float speed = 200.0f;
        if (Vector2Length(direction) > 0.0f) {
            direction = Vector2Normalize(direction);
            ballPosition.x += direction.x * speed * GetFrameTime();
            ballPosition.y += direction.y * speed * GetFrameTime();
        }

        // Draw
        Vector2 mousePos = GetMousePosition();
        float angle = atan2f(mousePos.y - ballPosition.y, mousePos.x - ballPosition.x) * RAD2DEG;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // 2. Define the source rectangle (the full size of your loaded image)
        Rectangle sourceRec = {0.0f, 0.0f, (float) sprite.width, (float) sprite.height};

        // 3. Define the destination rectangle (where it goes and its size on screen)
        Rectangle destRec = {ballPosition.x, ballPosition.y, (float) sprite.width, (float) sprite.height};

        // 4. Set the pivot point to the EXACT center of the sprite
        Vector2 origin = {(float) sprite.width / 2.0f - 8, (float) sprite.height / 2.0f};

        // 5. Draw the rotated texture
        // Note: If your image natively faces RIGHT, an angle of 0.0f means it points perfectly right.
        DrawTexturePro(sprite, sourceRec, destRec, origin, angle, WHITE);

        EndDrawing();
    }

    // De-Initialization
    UnloadTexture(sprite);
    CloseWindow();

    return 0;
}
