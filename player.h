#ifndef PLAYER_H
#define PLAYER_H

#include "common.h"

#define PLAYER_SPEED 200.0f

typedef struct {
    Entity entity;
    Texture2D texture;
} Player;

Player InitPlayer(const char *texturePath);

void UpdatePlayer(Player *player, float dt);

void DrawPlayer(const Player *player);

void UnloadPlayer(Player *player);

#endif // PLAYER_H
