#ifndef CSHOOTER_MAP_H
#define CSHOOTER_MAP_H

#include <raylib.h>
#include <stdint.h>

#define MAP_WIDTH 64
#define MAP_HEIGHT 64
#define MIN_CITY_BLOCK 10
#define MAX_BSP_DEPTH 4
#define MAX_BSP_NODES ((1 << (MAX_BSP_DEPTH + 1)) - 1) // max nodes of a binary tree is 2^(d+1) - 1
#define TILE_SIZE     192
#define MAX_LOOKAHEAD 4

typedef struct {
	int x, y;
	int width, height;
} TileRect;

typedef struct {
	TileRect rect;
	int left, right; // indices in node pool
} BSPNode;

typedef enum {
	TILE_NONE = 0, // unset; drawn as road
	TILE_GRASS,
	TILE_BUILDING,
	TILE_DOOR,
	TILE_ROAD,
	TILE_SIDEWALK,
	TILE_ALLEY,
	TILE_COUNT
} TileType;

typedef struct {
	const char* path; // NULL = no texture
} TileDef;

static const TileDef TILE_DEFS[TILE_COUNT] = {
	[TILE_GRASS] = {"../assets/map/grass.png"},
	[TILE_BUILDING] = {"../assets/map/roof.png"},
	[TILE_DOOR] = {"../assets/map/door.png"},
	[TILE_ROAD] = {"../assets/map/road.png"},
	[TILE_SIDEWALK] = {"../assets/map/sidewalk.png"},
	[TILE_ALLEY] = {"../assets/map/road.png"},
};

typedef struct {
	Texture2D tiles[TILE_COUNT];
	Texture2D striped_road; // draw-time variant, not a map tile
	Texture2D roof_edge;
	Texture2D roof_corner;
} TileTextures;

typedef struct {
	uint8_t tiles[MAP_WIDTH][MAP_HEIGHT];
	BSPNode nodes[MAX_BSP_NODES];
} Map;

void tile_textures_load(TileTextures* a);

void tile_textures_unload(TileTextures* a);

void map_init(Map* map);

void map_draw(const Map* map, const TileTextures* tile_textures);

TileType tile_at(const Map* map, int x, int y);

#endif //CSHOOTER_MAP_H
