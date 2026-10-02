//
// Created by Sagiv Marzini on 01/10/2026.
//

#ifndef CSHOOTER_MAP_H
#define CSHOOTER_MAP_H

#define MAP_WIDTH 64
#define MAP_HEIGHT 64
#define MIN_CITY_BLOCK 8
#define MAX_BSP_DEPTH 4
#define MAX_BSP_NODES ((1 << (MAX_BSP_DEPTH + 1)) - 1) // max nodes of a binary tree is 2^(d+1) - 1
#include <raylib.h>

typedef struct {
	int x, y;
	int width, height;
} TileRect;

typedef struct {
	TileRect rect;
	int left, right; // indices in node pool
} BSPNode;

typedef enum { TILE_GRASS = 1, TILE_BUILDING, TILE_ROAD, TILE_ALLEY } TileType;

typedef struct {
	TileType map[MAP_WIDTH][MAP_HEIGHT];
	BSPNode nodes[MAX_BSP_NODES];
	Texture2D road_texture;
	Texture2D striped_road_texture;
	Texture2D grass_texture;
	Texture2D roof_texture;
} Map;

Map map_init(const char* road_texture, const char* striped_road_texture, const char* grass_texture,
             const char* roof_texture);

void map_draw(const Map* map);


#endif //CSHOOTER_MAP_H
