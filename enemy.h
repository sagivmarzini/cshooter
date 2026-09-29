//
// Created by sagivmarzini on 29/09/2026.
//

#ifndef CSHOOTER_ENEMY_H
#define CSHOOTER_ENEMY_H
#include "common.h"
#include "player.h"

#define ENEMY_SPEED 100.0f

typedef struct {
    Entity entity;
    Texture2D texture;
} Enemy;

Enemy init_enemy(const char* texture_path);

void update_enemy(Enemy* enemy, const Player* player, float dt);

void draw_enemy(const Enemy* enemy);

void unload_enemy(Enemy* enemy);

#endif //CSHOOTER_ENEMY_H
