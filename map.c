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

Map map_init() {
	Map map = {0};

	map_generate_city(&map);

	return map;
}

void map_draw(Map* map) {
	// int tile_size = SCREEN_HEIGHT / MAP_HEIGHT;
	int tile_size = 192;
	for (int x = 0; x < MAP_WIDTH; x++) {
		for (int y = 0; y < MAP_HEIGHT; ++y) {
			TileType type = map->map[x][y];
			if (!type) type = TILE_ROAD;

			Color color = WHITE;
			if (type == TILE_GRASS) color = GREEN;
			if (type == TILE_ROAD) color = GRAY;
			if (type == TILE_BUILDING) color = MAROON;
			DrawRectangle(x * tile_size, y * tile_size, tile_size, tile_size, color);
		}
	}
}
