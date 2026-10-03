#include "map.h"

#include <raylib.h>
#include <stdbool.h>

// Compass directions in clockwise order. The stripe sits on the east edge
// at 0 degrees, so (direction * 90) is the rotation that puts it on that side.
enum { EAST, SOUTH, WEST, NORTH, DIRECTION_COUNT };

static const int STEP_X[DIRECTION_COUNT] = {1, 0, -1, 0};
static const int STEP_Y[DIRECTION_COUNT] = {0, 1, 0, -1};
static int next_node_index = 1;

static int get_next_node_index(void) {
	int index = next_node_index;

	next_node_index = (next_node_index + 1) % MAX_BSP_NODES;

	return index;
}

static void bsp_recursive_split(Map* map, int node_index, int depth) {
	if (map->nodes[node_index].left != -1 || map->nodes[node_index].right != -1) {
		return;
	}
	const TileRect rect = map->nodes[node_index].rect;
	TileType type = GetRandomValue(0, 10) > 3 ? TILE_BUILDING : TILE_GRASS;
	for (int x = rect.x; x < rect.x + rect.width; x++) {
		for (int y = rect.y; y < rect.y + rect.height; y++) {
			if (x == rect.x || x == rect.x + rect.width - 1 ||
			    y == rect.y || y == rect.y + rect.height - 1)
				map->tiles[x][y] = TILE_ROAD;
			else if (x == rect.x + 1 || x == rect.x + rect.width - 2 ||
			         y == rect.y + 1 || y == rect.y + rect.height - 2)
				map->tiles[x][y] = TILE_SIDEWALK;
			else
				map->tiles[x][y] = type;

			if (x == 0 || x == MAP_WIDTH - 1 || y == 0 || y == MAP_HEIGHT - 1) map->tiles[x][y] = TILE_NONE;
		}
	}
	// Add a door on a random side of a building
	if (type == TILE_BUILDING) {
		enum Side { SIDE_TOP, SIDE_RIGHT, SIDE_BOTTOM, SIDE_LEFT };
		const int door_side = GetRandomValue(SIDE_TOP, SIDE_LEFT);
		int rect_x = rect.x + 2;
		int rect_y = rect.y + 2;
		int width = rect.width - 4;
		int height = rect.height - 4;

		int x = door_side == SIDE_TOP || door_side == SIDE_BOTTOM
			        ? width / 2 + rect_x
			        : door_side == SIDE_RIGHT
				          ? rect_x + width - 1
				          : rect_x;
		int y = door_side == SIDE_RIGHT || door_side == SIDE_LEFT
			        ? height / 2 + rect_y
			        : door_side == SIDE_TOP
				          ? rect_y
				          : rect_y + height - 1;

		map->tiles[x][y] = TILE_DOOR;
	}

	if (depth >= MAX_BSP_DEPTH || rect.width < MIN_CITY_BLOCK || rect.height < MIN_CITY_BLOCK) {
		return;
	}

	bool is_vertical_cut = false; // otherwise horizontal cut
	if (rect.width > rect.height * 1.25) is_vertical_cut = true;
	else if (rect.height > rect.width * 1.25) is_vertical_cut = false;
	else is_vertical_cut = GetRandomValue(0, 1);

	int cut_percent = GetRandomValue(3, 7);

	TileRect rect1 = {0};
	TileRect rect2 = {0};

	if (is_vertical_cut) {
		int split_size = (rect.width * cut_percent) / 10;

		// Left Child
		rect1.x = rect.x;
		rect1.y = rect.y;
		rect1.width = split_size;
		rect1.height = rect.height;

		// Right Child
		rect2.x = rect.x + split_size;
		rect2.y = rect.y;
		rect2.width = rect.width - split_size;
		rect2.height = rect.height;
	} else {
		int split_size_y = (rect.height * cut_percent) / 10;

		// Top Child
		rect1.x = rect.x;
		rect1.y = rect.y;
		rect1.width = rect.width;
		rect1.height = split_size_y;

		// Bottom Child
		rect2.x = rect.x;
		rect2.y = rect.y + split_size_y;
		rect2.width = rect.width;
		rect2.height = rect.height - split_size_y;
	}

	int left_index = get_next_node_index();
	int right_index = get_next_node_index();

	map->nodes[node_index].left = left_index;
	map->nodes[node_index].right = right_index;

	map->nodes[left_index] = (BSPNode){rect1, -1, -1};
	map->nodes[right_index] = (BSPNode){rect2, -1, -1};

	bsp_recursive_split(map, left_index, depth + 1);
	bsp_recursive_split(map, right_index, depth + 1);
}

static void map_generate_city(Map* map) {
	map->nodes[0].rect = (TileRect){0, 0, MAP_WIDTH, MAP_HEIGHT};
	map->nodes[0].left = -1;
	map->nodes[0].right = -1;

	bsp_recursive_split(map, 0, 0);
}

void tile_textures_load(TileTextures* a) {
	for (int i = 0; i < TILE_COUNT; i++)
		if (TILE_DEFS[i].path) a->tiles[i] = LoadTexture(TILE_DEFS[i].path);
	a->striped_road = LoadTexture("../assets/map/striped_road.png");
	a->roof_edge = LoadTexture("../assets/map/roof_edge.png");
	a->roof_corner = LoadTexture("../assets/map/roof_corner.png");
}

void tile_textures_unload(TileTextures* a) {
	for (int i = 0; i < TILE_COUNT; i++)
		if (a->tiles[i].id != 0) UnloadTexture(a->tiles[i]);
	UnloadTexture(a->striped_road);
	UnloadTexture(a->roof_edge);
	UnloadTexture(a->roof_corner);
}

void map_init(Map* map) {
	map_generate_city(map);
}

static bool in_bounds(int x, int y) {
	return x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT;
}

TileType tile_at(const Map* map, int x, int y) {
	TileType type = map->tiles[x][y];
	// return type ? type : TILE_ROAD;
	return type;
}


// Draws a texture stretched over one map cell, rotated around the cell centre.
static void draw_tile(Texture2D texture, int tile_x, int tile_y, float rotation) {
	Rectangle source = {0, 0, (float) texture.width, (float) texture.height};
	Rectangle dest = {
		tile_x * TILE_SIZE + TILE_SIZE / 2.0f,
		tile_y * TILE_SIZE + TILE_SIZE / 2.0f,
		TILE_SIZE, TILE_SIZE
	};
	Vector2 pivot = {TILE_SIZE / 2.0f, TILE_SIZE / 2.0f};
	DrawTexturePro(texture, source, dest, pivot, rotation, WHITE);
}

// How many consecutive road tiles continue from this tile in each direction.
static void count_road_ahead(const Map* map, int tile_x, int tile_y,
                             int road_tiles_ahead[DIRECTION_COUNT]) {
	for (int direction = 0; direction < DIRECTION_COUNT; direction++) {
		road_tiles_ahead[direction] = 0;
		for (int distance = 1; distance < MAX_LOOKAHEAD; distance++) {
			int neighbor_x = tile_x + STEP_X[direction] * distance;
			int neighbor_y = tile_y + STEP_Y[direction] * distance;

			if (!in_bounds(neighbor_x, neighbor_y)) break;
			if (tile_at(map, neighbor_x, neighbor_y) != TILE_ROAD) break;

			road_tiles_ahead[direction]++;
		}
	}
}

static void draw_non_road_tile(const TileTextures* atlas, TileType type,
                               int tile_x, int tile_y) {
	float rotation = ((tile_x * tile_y) % 3) * 90;
	draw_tile(atlas->tiles[type], tile_x, tile_y, rotation);
}

static void draw_road_tile(const Map* map, const TileTextures* atlas,
                           int tile_x, int tile_y) {
	int ahead[DIRECTION_COUNT];
	count_road_ahead(map, tile_x, tile_y, ahead);

	int horizontal_extent = 1 + ahead[EAST] + ahead[WEST];
	int vertical_extent = 1 + ahead[SOUTH] + ahead[NORTH];
	bool runs_vertically = vertical_extent > horizontal_extent;
	int road_width = runs_vertically ? horizontal_extent : vertical_extent;

	Texture2D texture = atlas->tiles[TILE_ROAD];
	float rotation = runs_vertically ? 0.0f : 90.0f; // align plain road with its direction

	bool has_clear_direction = horizontal_extent != vertical_extent;
	if (has_clear_direction && road_width == 2) {
		// Two-lane road: the stripe faces the neighbouring lane.
		texture = atlas->striped_road;
		if (runs_vertically) rotation = ahead[EAST] > 0 ? 0.0f : 180.0f;
		else rotation = ahead[SOUTH] > 0 ? 90.0f : 270.0f;
	}

	draw_tile(texture, tile_x, tile_y, rotation);
}

static void draw_roof_tile(const Map* map, const TileTextures* atlas, int tile_x, int tile_y) {
	// A side is "open" if the neighbour there is not a building (the map border counts as open).
	bool open_side[DIRECTION_COUNT];
	int open_count = 0;
	int first_open = -1;

	for (int direction = 0; direction < DIRECTION_COUNT; direction++) {
		int neighbor_x = tile_x + STEP_X[direction];
		int neighbor_y = tile_y + STEP_Y[direction];

		bool is_building = in_bounds(neighbor_x, neighbor_y) &&
		                   (tile_at(map, neighbor_x, neighbor_y) == TILE_BUILDING ||
		                    tile_at(map, neighbor_x, neighbor_y) == TILE_DOOR);

		open_side[direction] = !is_building;
		if (open_side[direction]) {
			open_count++;
			if (first_open == -1) first_open = direction;
		}
	}

	Texture2D texture = atlas->tiles[TILE_BUILDING]; // plain roof
	float rotation = (tile_x * tile_y) % 3 * 90.f;

	if (open_count == 1) {
		// Straight edge, facing the single open side.
		texture = atlas->roof_edge;
		rotation = first_open * 90.0f;
	} else if (open_count == 2) {
		// Corner if the two open sides are adjacent: direction + the one counter-clockwise from it.
		for (int direction = 0; direction < DIRECTION_COUNT; direction++) {
			int counter_clockwise = (direction + 3) % DIRECTION_COUNT;
			if (open_side[direction] && open_side[counter_clockwise]) {
				texture = atlas->roof_corner;
				rotation = direction * 90.0f;
				break;
			}
		}
		// Opposite sides open (a 1-tile-wide building): no sprite for it yet, falls back to the plain roof.
	} else if (open_count >= 3) {
		// Tip or lone block: no dedicated sprite yet, so reuse the edge facing the first open side.
		texture = atlas->roof_edge;
		rotation = first_open * 90.0f;
	}

	draw_tile(texture, tile_x, tile_y, rotation);
}

static void draw_door_tile(const Map* map, const TileTextures* tile_textures, int tile_x, int tile_y) {
	float rotation = 0;
	if (in_bounds(tile_x, tile_y + 1) && tile_at(map, tile_x, tile_y + 1) != TILE_BUILDING) rotation = 90;
	if (in_bounds(tile_x - 1, tile_y) && tile_at(map, tile_x - 1, tile_y) != TILE_BUILDING) rotation = 180;
	if (in_bounds(tile_x, tile_y - 1) && tile_at(map, tile_x, tile_y - 1) != TILE_BUILDING) rotation = 270;

	draw_tile(tile_textures->tiles[TILE_DOOR], tile_x, tile_y, rotation);
}

void map_draw(const Map* map, const TileTextures* tile_textures) {
	for (int tile_y = 0; tile_y < MAP_HEIGHT; tile_y++) {
		for (int tile_x = 0; tile_x < MAP_WIDTH; tile_x++) {
			TileType type = tile_at(map, tile_x, tile_y);
			if (type == TILE_ROAD) draw_road_tile(map, tile_textures, tile_x, tile_y);
			else if (type == TILE_BUILDING) draw_roof_tile(map, tile_textures, tile_x, tile_y);
			else if (type == TILE_DOOR) draw_door_tile(map, tile_textures, tile_x, tile_y);
			else draw_non_road_tile(tile_textures, type, tile_x, tile_y);
		}
	}
}
