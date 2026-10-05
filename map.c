#include "map.h"

#include <raylib.h>
#include <stdbool.h>

// Compass directions in clockwise order. The stripe sits on the east edge
// at 0 degrees, so (direction * 90) is the rotation that puts it on that side.
enum { EAST, SOUTH, WEST, NORTH, DIRECTION_COUNT };

static const int STEP_X[DIRECTION_COUNT] = {1, 0, -1, 0};
static const int STEP_Y[DIRECTION_COUNT] = {0, 1, 0, -1};

static int next_node_index = 1;
static int next_building_id = 1;

static int get_next_node_index(void) {
	int index = next_node_index;

	next_node_index = (next_node_index + 1) % MAX_BSP_NODES;

	return index;
}

typedef enum { SIDE_TOP, SIDE_RIGHT, SIDE_BOTTOM, SIDE_LEFT } Side;

static inline int min_int(int a, int b) { return a < b ? a : b; }

static bool is_map_edge(int x, int y) {
	return x == 0 || y == 0 || x == MAP_WIDTH - 1 || y == MAP_HEIGHT - 1;
}

static TileRect inset_rect(TileRect r, int amount) {
	return (TileRect){
		r.x + amount, r.y + amount,
		r.width - 2 * amount, r.height - 2 * amount
	};
}

// Splits r in two along one axis. `gap` tiles between the halves are left
// uncovered (0 for BSP cuts, 1 for an alley strip).
static void split_rect(TileRect r, bool vertical, int gap, int percent,
                       TileRect* a, TileRect* b) {
	if (vertical) {
		int size = r.width * percent / 10;
		*a = (TileRect){r.x, r.y, size, r.height};
		*b = (TileRect){r.x + size + gap, r.y, r.width - size - gap, r.height};
	} else {
		int size = r.height * percent / 10;
		*a = (TileRect){r.x, r.y, r.width, size};
		*b = (TileRect){r.x, r.y + size + gap, r.width, r.height - size - gap};
	}
}

// Road ring -> sidewalk ring -> fill. Also resets stale building ids.
static void paint_block(Map* map, TileRect rect, TileType fill) {
	for (int x = rect.x; x < rect.x + rect.width; x++) {
		for (int y = rect.y; y < rect.y + rect.height; y++) {
			int ring = min_int(min_int(x - rect.x, rect.x + rect.width - 1 - x),
			                   min_int(y - rect.y, rect.y + rect.height - 1 - y));
			TileType t = ring == 0 ? TILE_ROAD : ring == 1 ? TILE_SIDEWALK : fill;
			map->tiles[x][y] = is_map_edge(x, y) ? TILE_NONE : t;
			map->building_id[x][y] = NO_BUILDING_ID;
		}
	}
}

static void place_door(Map* map, TileRect part, Side side) {
	int x = part.x + part.width / 2;
	int y = part.y + part.height / 2;
	switch (side) {
		case SIDE_TOP: y = part.y;
			break;
		case SIDE_BOTTOM: y = part.y + part.height - 1;
			break;
		case SIDE_LEFT: x = part.x;
			break;
		case SIDE_RIGHT: x = part.x + part.width - 1;
			break;
	}
	map->tiles[x][y] = TILE_DOOR;
}

// Optionally cuts the footprint in two with an alley, then gives every
// resulting part its own building id and its own door.
static void build_building(Map* map, TileRect footprint) {
	TileRect parts[2] = {footprint};
	int part_count = 1;

	if ((footprint.width > MIN_ALLEY_BUILDING_SIZE ||
	     footprint.height > MIN_ALLEY_BUILDING_SIZE) && GetRandomValue(0, 1)) {
		bool vertical = footprint.width > footprint.height;
		split_rect(footprint, vertical, 1, GetRandomValue(4, 6), &parts[0], &parts[1]);
		part_count = 2;

		// The alley is the 1-tile strip between the two parts.
		if (vertical) {
			int x = parts[0].x + parts[0].width;
			for (int y = footprint.y; y < footprint.y + footprint.height; y++)
				map->tiles[x][y] = TILE_ALLEY;
		} else {
			int y = parts[0].y + parts[0].height;
			for (int x = footprint.x; x < footprint.x + footprint.width; x++)
				map->tiles[x][y] = TILE_ALLEY;
		}
	}

	for (int i = 0; i < part_count; i++) {
		const TileRect p = parts[i];
		const int id = next_building_id++;
		for (int x = p.x; x < p.x + p.width; x++)
			for (int y = p.y; y < p.y + p.height; y++)
				map->building_id[x][y] = id;

		place_door(map, p, (Side) GetRandomValue(SIDE_TOP, SIDE_LEFT));
	}
}

static void bsp_recursive_split(Map* map, int node_index, int depth) {
	const TileRect rect = map->nodes[node_index].rect;

	bool vertical_cut;
	if (rect.width > rect.height * 1.25) vertical_cut = true;
	else if (rect.height > rect.width * 1.25) vertical_cut = false;
	else vertical_cut = GetRandomValue(0, 1);

	// The smaller child gets 40% of the cut axis; it must still be a valid block.
	const int cut_axis_len = vertical_cut ? rect.width : rect.height;
	const bool can_split = depth < MAX_BSP_DEPTH &&
	                       cut_axis_len * 4 / 10 >= MIN_CITY_BLOCK &&
	                       rect.width >= MIN_CITY_BLOCK && rect.height >= MIN_CITY_BLOCK;

	if (!can_split) {
		// Only leaves are painted; internal nodes are fully covered by their children.
		const TileRect footprint = inset_rect(rect, BLOCK_INSET);
		const bool has_building = footprint.width >= MIN_BUILDING_SIZE &&
		                          footprint.height >= MIN_BUILDING_SIZE &&
		                          GetRandomValue(1, 100) <= BUILDING_CHANCE_PERCENT;

		paint_block(map, rect, has_building ? TILE_BUILDING : TILE_GRASS);
		if (has_building) build_building(map, footprint);
		return;
	}

	TileRect rect1, rect2;
	split_rect(rect, vertical_cut, 0, GetRandomValue(4, 6), &rect1, &rect2);

	const int left_index = get_next_node_index();
	const int right_index = get_next_node_index();
	map->nodes[node_index].left = left_index;
	map->nodes[node_index].right = right_index;
	map->nodes[left_index] = (BSPNode){rect1, -1, -1};
	map->nodes[right_index] = (BSPNode){rect2, -1, -1};

	bsp_recursive_split(map, left_index, depth + 1);
	bsp_recursive_split(map, right_index, depth + 1);
}

void map_generate_city(Map* map) {
	map->nodes[0].rect = (TileRect){0, 0, MAP_WIDTH, MAP_HEIGHT};
	map->nodes[0].left = -1;
	map->nodes[0].right = -1;

	bsp_recursive_split(map, 0, 0);
}

void world_enter_building(GameWorld* world, uint8_t building_id) {
	// TODO: implement interiors
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

static bool in_bounds(int x, int y) {
	return x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT;
}

TileType tile_at(const Map* map, int x, int y) {
	TileType type = map->tiles[x][y];
	// return type ? type : TILE_ROAD;
	return type;
}

Map* get_active_map(GameWorld* world) {
	return world->is_player_inside ? &world->interiors[world->current_building].map : &world->city;
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

static void draw_normal_tile(const TileTextures* atlas, TileType type,
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

void world_draw(const GameWorld* world, const TileTextures* tile_textures) {
	for (int tile_y = 0; tile_y < MAP_HEIGHT; tile_y++) {
		for (int tile_x = 0; tile_x < MAP_WIDTH; tile_x++) {
			TileType type = tile_at(get_active_map(world), tile_x, tile_y);
			if (type == TILE_ROAD) draw_road_tile(&world->city, tile_textures, tile_x, tile_y);
			else if (type == TILE_BUILDING) draw_roof_tile(&world->city, tile_textures, tile_x, tile_y);
			else if (type == TILE_DOOR) draw_door_tile(&world->city, tile_textures, tile_x, tile_y);
			else draw_normal_tile(tile_textures, type, tile_x, tile_y);
		}
	}
}
