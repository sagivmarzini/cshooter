#ifndef PLAYER_H
#define PLAYER_H

#include "bullet.h"
#include "common.h"

#define PLAYER_SPEED 200.0f
#define GUN_BARREL_OFFSET 34 // the distance between the center of the sprite and the tip of its gun

typedef struct {
    Entity entity;
    Texture2D texture;
} Player;

Player init_player(const char* texture_path);

void update_player(Player* player, BulletManager* bm, float dt);

void draw_player(const Player* player);

void unload_player(Player* player);

#endif // PLAYER_H
