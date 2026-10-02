#include "map.h"

#include <raylib.h>
#include <stdbool.h>

#include "common.h"

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
				map->map[x][y] = TILE_ROAD;
			else
				map->map[x][y] = type;
		}
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

Map map_init(const char* road_texture, const char* striped_road_texture) {
	Map map = {0};
	map.road_texture = LoadTexture(road_texture);
	map.striped_road_texture = LoadTexture(striped_road_texture);

	map_generate_city(&map);

	return map;
}

void map_draw(const Map* map) {
	enum { TILE_SIZE = 192, MAX_LOOKAHEAD = 4 };

	// Compass directions in clockwise order. The stripe sits on the east edge
	// at 0 degrees, so (direction * 90) is the rotation that puts it on that side.
	enum { EAST, SOUTH, WEST, NORTH, DIRECTION_COUNT };
	static const int STEP_X[DIRECTION_COUNT] = {1, 0, -1, 0};
	static const int STEP_Y[DIRECTION_COUNT] = {0, 1, 0, -1};

	for (int tile_y = 0; tile_y < MAP_HEIGHT; tile_y++) {
		for (int tile_x = 0; tile_x < MAP_WIDTH; tile_x++) {
			TileType tile_type = map->map[tile_y][tile_x];
			if (!tile_type) tile_type = TILE_ROAD;

			Rectangle cell_rect = {
				(float) (tile_x * TILE_SIZE), (float) (tile_y * TILE_SIZE),
				TILE_SIZE, TILE_SIZE
			};

			if (tile_type != TILE_ROAD) {
				Color fill_color = (tile_type == TILE_GRASS)
					                   ? GREEN
					                   : (tile_type == TILE_BUILDING)
						                     ? MAROON
						                     : BLANK;
				DrawRectangleRec(cell_rect, fill_color);
				continue;
			}

			// How many road tiles in a row continue from this tile in each direction.
			int road_tiles_ahead[DIRECTION_COUNT] = {0};
			for (int direction = 0; direction < DIRECTION_COUNT; direction++) {
				for (int distance = 1; distance < MAX_LOOKAHEAD; distance++) {
					int neighbor_x = tile_x + STEP_X[direction] * distance;
					int neighbor_y = tile_y + STEP_Y[direction] * distance;

					bool out_of_bounds = neighbor_x < 0 || neighbor_x >= MAP_WIDTH ||
					                     neighbor_y < 0 || neighbor_y >= MAP_HEIGHT;
					if (out_of_bounds) break;

					TileType neighbor_type = map->map[neighbor_y][neighbor_x];
					if (neighbor_type && neighbor_type != TILE_ROAD) break;

					road_tiles_ahead[direction]++;
				}
			}

			int horizontal_extent = 1 + road_tiles_ahead[EAST] + road_tiles_ahead[WEST];
			int vertical_extent = 1 + road_tiles_ahead[SOUTH] + road_tiles_ahead[NORTH];
			bool runs_vertically = vertical_extent > horizontal_extent;
			int road_width = runs_vertically ? horizontal_extent : vertical_extent;

			Texture2D texture = map->road_texture;
			float rotation = runs_vertically ? 0.0f : 90.0f; // align plain road with its direction

			bool has_clear_direction = horizontal_extent != vertical_extent;
			if (has_clear_direction && road_width == 2) {
				// Two-lane road: the stripe faces the neighbouring lane.
				texture = map->striped_road_texture;
				if (runs_vertically) rotation = road_tiles_ahead[EAST] > 0 ? 0.0f : 180.0f;
				else rotation = road_tiles_ahead[SOUTH] > 0 ? 90.0f : 270.0f;
			}

			Rectangle source_rect = {0, 0, (float) texture.width, (float) texture.height};
			Rectangle dest_rect = {
				cell_rect.x + TILE_SIZE / 2.0f, cell_rect.y + TILE_SIZE / 2.0f,
				TILE_SIZE, TILE_SIZE
			};
			Vector2 rotation_pivot = {TILE_SIZE / 2.0f, TILE_SIZE / 2.0f}; // rotate around the tile centre

			DrawTexturePro(texture, source_rect, dest_rect, rotation_pivot, rotation, WHITE);
		}
	}
}
