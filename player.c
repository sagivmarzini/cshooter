#include "player.h"

Player InitPlayer(const char *texturePath) {
    Player player = {0};
    player.texture = LoadTexture(texturePath);
    player.entity.pos = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
    player.entity.active = true;
    return player;
}

void UpdatePlayer(Player *player, float dt) {
    Vector2 direction = {0.0f, 0.0f};

    if (IsKeyDown(KEY_D)) direction.x += 1.0f;
    if (IsKeyDown(KEY_A)) direction.x -= 1.0f;
    if (IsKeyDown(KEY_S)) direction.y += 1.0f;
    if (IsKeyDown(KEY_W)) direction.y -= 1.0f;

    Vector2 dir = Vector2Normalize(direction);
    player->entity.vel = Vector2Scale(dir, PLAYER_SPEED);
    player->entity.pos = Vector2Add(player->entity.pos, Vector2Scale(player->entity.vel, dt));

    Vector2 mousePos = GetMousePosition();
    player->entity.rotation = atan2f(mousePos.y - player->entity.pos.y, mousePos.x - player->entity.pos.x) * RAD2DEG;
}

void DrawPlayer(const Player *player) {
    Rectangle sourceRec = {0.0f, 0.0f, (float) player->texture.width, (float) player->texture.height};
    Rectangle destRec = {
        player->entity.pos.x, player->entity.pos.y, (float) player->texture.width, (float) player->texture.height
    };
    Vector2 origin = {(float) player->texture.width / 2.0f - 8, (float) player->texture.height / 2.0f};

    DrawTexturePro(player->texture, sourceRec, destRec, origin, player->entity.rotation, WHITE);
}

void UnloadPlayer(Player *player) {
    UnloadTexture(player->texture);
}
