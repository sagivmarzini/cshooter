#ifndef CSHOOTER_MAP_H
#define CSHOOTER_MAP_H

#include <raylib.h>
#include <stdint.h>

#define MAP_WIDTH 64
#define MAP_HEIGHT 64
#define MIN_CITY_BLOCK 10
#define MAX_BSP_DEPTH 4
#define MAX_BSP_NODES ((1 << (MAX_BSP_DEPTH + 1)) - 1) // max nodes of a binary tree is 2^(d+1) - 1
#define TILE_SIZE 144
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

	// Interior tiles
	TILE_FLOOR,
	TILE_WALL,
	TILE_EXIT, // the interior's own door, distinct from city's TILE_DOOR

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

	// Interior
	[TILE_FLOOR] = {"../assets/map/sidewalk.png"},
	[TILE_WALL] = {"../assets/map/road.png"},
	[TILE_EXIT] = {"../assets/map/striped_road.png"},
};

typedef struct {
	Texture2D tiles[TILE_COUNT];
	Texture2D striped_road; // draw-time variant, not a map tile
	Texture2D roof_edge;
	Texture2D roof_corner;
} TileTextures;


typedef struct {
	uint8_t tiles[MAP_WIDTH][MAP_HEIGHT];
	uint8_t building_id[MAP_WIDTH][MAP_HEIGHT]; // 0 = none, else index into interiors[]
	BSPNode nodes[MAX_BSP_NODES];
} Map;

typedef struct {
	Map map;
	TileRect building;
	bool generated;
	int return_x, return_y; // city tile coords to place player on exit
} Interior;

typedef struct {
	Map city;
	Interior interiors[MAX_BSP_NODES];
	bool is_player_inside;
	int current_building; // valid only is inside
} GameWorld;


void tile_textures_load(TileTextures* a);

void tile_textures_unload(TileTextures* a);

void map_generate_city(Map* map);

void world_enter_building(GameWorld* world, uint8_t building_id);

void world_draw(const GameWorld* world, const TileTextures* tile_textures);

TileType tile_at(const Map* map, int x, int y);

Map* get_active_map(GameWorld* world);

#endif //CSHOOTER_MAP_H
