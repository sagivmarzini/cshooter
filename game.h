#pragma once

#include "bullet.h"
#include "enemy.h"
#include "map.h"
#include "player.h"

typedef struct GameContext {
	GameWorld world;
	TileTextures tile_textures;
	Player player;
	EnemyManager enemy_manager;
	BulletManager bullet_manager;
} GameContext;

void game_context_init(GameContext* game_context);

void game_context_unload(GameContext* game_context);
